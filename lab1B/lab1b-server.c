// NAME: Jack Zhang
// EMAIL: dvorjackz@ucla.edu

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <getopt.h>
#include <poll.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <zlib.h>

// terminal attributes 
struct termios initial_attr;

// file descriptors
int socketfd;
int cliSocketfd;

// option flags                                                                                                                                                                                            
int port_flag;
int compress_flag;

// for compression                                                                                                                                                                                         
z_stream from_client;
z_stream to_client;;

// etc.
int port_no;
pid_t pid;

int pipepc[2]; // parent to child pipe
int pipecp[2]; // child to parent pipe

void terminalReset() {
  tcsetattr(0, TCSANOW, &initial_attr);
}

void terminalSetup() {
  // save initial terminal state
  tcgetattr(0, &initial_attr);
  atexit(terminalReset);
  
  struct termios t_attr;
  tcgetattr(0, &t_attr);
  t_attr.c_iflag = ISTRIP;
  t_attr.c_oflag = 0;
  t_attr.c_lflag = 0;
  
  tcsetattr(0, TCSANOW, &t_attr);
}

void ends() {
  deflateEnd(&to_client);
  inflateEnd(&from_client);
}

void compressionSetup() {
  // end deflation when program ends
  atexit(ends);

  to_client.zalloc = Z_NULL;
  to_client.zfree = Z_NULL;
  to_client.opaque = Z_NULL;
  if (deflateInit(&to_client, Z_DEFAULT_COMPRESSION) != Z_OK) {
    fprintf(stderr, "Error: could not initiate deflation\n");
    exit(1);
  }
  from_client.zalloc = Z_NULL;
  from_client.zfree = Z_NULL;
  from_client.opaque = Z_NULL;
  if (inflateInit(&from_client) != Z_OK)
    {
      fprintf(stderr, "Error: could not initiate inflation\n");
      exit(1);
    }
}

void runShell() {
  // close unnecessary ends of each pipe
  close(pipepc[1]);
  close(pipecp[0]);
  // redirect the read end of the parent-child pipe to stdin
  dup2(pipepc[0], 0);
  close(pipepc[0]);
  // redirect the write end of the child-parent pipe to stdout
  dup2(pipecp[1], 1);
  dup2(pipecp[1], 2);
  close(pipecp[1]);

  // preliminary setup is complete, now we spin up the duplex terminal on the child
  char* arg[] = {"/bin/bash", NULL};
  if (execvp(arg[0], arg) == -1) {
    fprintf(stderr, "Execvp() failed. Error code: %d. Error message: %s.\n", errno, strerror(errno));
    exit(1);
  }
}

void run() {
  // set up poll infrastructure
  struct pollfd polls[2];
  // poll for input from stdin
  polls[0].fd = cliSocketfd;
  polls[0].events = POLLIN | POLLERR | POLLHUP; // POLLRDHUP not needed since there is no socket interaction between server and shell
  // poll for input from read end of child-parent pipe
  polls[1].fd = pipecp[0];
  polls[1].events = POLLIN | POLLERR | POLLHUP;
  
  // close unnecessary ends of each pipe
  close(pipepc[0]);
  close(pipecp[1]);

  while (1) {
    // block until either stdin or read end of child-parent pipe register input
    if (poll(polls, 2, 0) == -1) {
      fprintf(stderr, "Error: poll() failed\n");
      exit(1);
    }
    
    // CASE 1: input from stdin is detected
    if (polls[0].revents & POLLIN) {
      char buf[128];
      int ret;
      if ((ret = read(cliSocketfd, buf, sizeof(char)*128)) == -1) {
	fprintf(stderr, "Error: could not read from stdin\n");
	exit(1);
      }

      if (compress_flag) {
        int compression_buf_size = 1024;
        char compression_buf[compression_buf_size];

	// place buf into the zstream
        from_client.avail_in = ret;
        from_client.next_in = (Bytef*) buf;
        from_client.avail_out = compression_buf_size;
        from_client.next_out = (Bytef*) compression_buf;

	// decompress
	do {
                inflate(&from_client, Z_SYNC_FLUSH);
        } while (from_client.avail_in > 0);

	// replace buf with the decompressed buf 
	ret = compression_buf_size - from_client.avail_out;
        memcpy(buf, compression_buf, ret);
      }
      
      for (int i = 0; i < ret; i++) {
	switch (buf[i]) {
	case '\3':
	  // write(1, "^C", 2*sizeof(char));
	  kill(pid, SIGINT); // pid of the child process since we are in the parent process
	  break;
	case '\4':
	  // write(1, "^D", 2*sizeof(char));
	  close(pipepc[1]);
	  break;
	case '\n':
	case '\r':
	  // write(1, "\r\n", 2*sizeof(char));
	  write(pipepc[1], "\n", sizeof(char));
	  break;
	default:
	  // write(1, &buf[i], sizeof(char));
	  write(pipepc[1], &buf[i], sizeof(char));
	}
      }
    }

    // CASE 2: input from child-parent pipe is detected
    if (polls[1].revents & POLLIN) {
      char buf[128];
      int ret;
      if ((ret = read(pipecp[0], buf, sizeof(char)*128)) == -1) {
	fprintf(stderr, "Error: could not read from shell pipe\n");
	exit(1);
      }
      if (compress_flag) {
        int compression_buf_size = 1024;
        char compression_buf[compression_buf_size];

        // place buf into the zstream
        to_client.avail_in = ret;
        to_client.next_in = (Bytef*) buf;
        to_client.avail_out = compression_buf_size;
        to_client.next_out = (Bytef*) compression_buf;

        // compress
        do {
          deflate(&to_client, Z_SYNC_FLUSH);
        } while (to_client.avail_in > 0);

        // replace buf with the compressed buf
        ret = compression_buf_size - to_client.avail_out;
        memcpy(buf, compression_buf, ret);
        write(cliSocketfd, buf, ret);
      }
      else {
	for (int i = 0; i < ret; i++) {
	  write(cliSocketfd, &buf[i], sizeof(char));
	  // write(1, &buf[i], sizeof(char));
	}
      }
    }
    
    // CASE 3: checks for POLLERR and POLLHUP last so that the last bits of output can be received
    if (polls[1].revents & (POLLERR | POLLHUP)) {
      int sig;
      if ((waitpid(pid, &sig, 0)) == -1) {
	fprintf(stderr, "Error: waitpid() failed\n");
	exit(1);
      }
      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(sig), WEXITSTATUS(sig));
      exit(0);
    }
  }
}

int main(int argc, char **argv) {

  // first parse options
  opterr = 0; // suppress automatic stock error message
  struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"compress", no_argument, 0, 'c'},
    {0, 0, 0, 0}
  };

  char opt;
  while (1) {
    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == -1) {
      break;
    }
    else if (opt == 'p') {
      port_no = atoi(optarg);
      port_flag = 1;
    }
    else if (opt == 'c') {
      compress_flag = 1;
      compressionSetup();
    }
    else {
      fprintf(stderr, "Error: unrecognized option or missing argument\n");
      exit(1);
    }
  }

  if (port_flag != 1) {
    fprintf(stderr, "Error: missing --port option and argument\n");
    exit(1);
  }

  // Referenced the following code, provided by the instructor,
  // for the server socket setup:
  // http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/server.c
  
  // data structures needed for socket setup and communication
  struct sockaddr_in serv_addr, cli_addr;
  unsigned int clilen;
  
  // establish socket
  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Error: could not open socket\n");
    exit(1);
  }
  
  // set up serv_addr
  memset((char*) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port_no);

  // bind server address to server socket fd
  if (bind(socketfd, (struct sockaddr*) &serv_addr,
	   sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error: binding failure\n");
    exit(1);
  }

  // block until a connection request is received
  listen(socketfd, 5);
  clilen = sizeof(cli_addr);
  if ((cliSocketfd = accept(socketfd, (struct sockaddr*) &cli_addr, &clilen)) < 0) {
    fprintf(stderr, "Error: could not establish connection with client\n");
    exit(1);
  }
  
  // set up terminal for no echo and non-canonical input mode
  terminalSetup();

  //    activate shell mode if option has been specified
  if (pipe(pipepc) == -1 || pipe(pipecp) == -1) {
    fprintf(stderr, "Error: could not create pipe\n");
    exit(1);
  }
  pid = fork();
  if (pid == -1) {
    fprintf(stderr, "Error: unable to fork new process\n");
    exit(1);
  }
  // child process
  else if (pid == 0) {
    runShell();
  }
  // parent process
  else {
    run();
  }
  
  exit(0);
  
}
