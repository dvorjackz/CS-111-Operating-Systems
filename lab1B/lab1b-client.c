// NAME: Jack Zhang
// EMAIL: dvorjackz@ucla.edu

#define _GNU_SOURCE
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
#include <netdb.h>
#include <zlib.h>

// terminal attributes
struct termios initial_attr;

// file descriptors
int socketfd;
int logfd;

// option flags
int port_flag;
int log_flag;
int compress_flag;

// for compression
z_stream to_server;
z_stream from_server;

// etc.
int port_no;

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

void logSent(char buf[], int bufLen) {
  write(logfd, "SENT ", 5*sizeof(char));
  char num_bytes[20];
  sprintf(num_bytes, "%d", bufLen);
  write(logfd, num_bytes, strlen(num_bytes)*sizeof(char));
  write(logfd, " BYTES: ", 8*sizeof(char));
  write(logfd, buf, bufLen*sizeof(char));
  write(logfd, "\n", sizeof(char));
}

void logReceived(char buf[], int bufLen) {
  write(logfd, "RECEIVED ", 9*sizeof(char));
  char num_bytes[20];
  sprintf(num_bytes, "%d", bufLen);
  write(logfd, num_bytes, strlen(num_bytes)*sizeof(char));
  write(logfd, " BYTES: ", 8*sizeof(char));
  write(logfd, buf, bufLen*sizeof(char));
  write(logfd, "\n", sizeof(char));
}

void ends() {
  deflateEnd(&to_server);
  inflateEnd(&from_server);
}

void compressionSetup() {
  // end deflation when program ends
  atexit(ends);
  
  to_server.zalloc = Z_NULL;
  to_server.zfree = Z_NULL;
  to_server.opaque = Z_NULL;
  if (deflateInit(&to_server, Z_DEFAULT_COMPRESSION) != Z_OK) {
    fprintf(stderr, "Error: could not initiate deflation\n");
    exit(1);
  }
  from_server.zalloc = Z_NULL;
  from_server.zfree = Z_NULL;
  from_server.opaque = Z_NULL;
  if (inflateInit(&from_server) != Z_OK) 
    {
      fprintf(stderr, "Error: could not initiate inflation\n");
      exit(1);
    }
}

void run() {
  // set up poll infrastructure
  struct pollfd polls[2];
  // poll for input from stdin
  polls[0].fd = 0;
  polls[0].events = POLLIN | POLLERR | POLLHUP | POLLRDHUP;
  // poll for input from read end of child-parent pipe
  polls[1].fd = socketfd;
  polls[1].events = POLLIN | POLLERR | POLLHUP | POLLRDHUP;

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
      if ((ret = read(0, buf, sizeof(char)*128)) == -1) {
	fprintf(stderr, "Error: could not read from stdin\n");
	exit(1);
      }

      // echo typed input to stdout (always occurs regardless of compression)
      for (int i = 0; i < ret; i++) {
	switch (buf[i]) {
	case '\n':
	case '\r':
	  write(1, "\r\n", 2*sizeof(char));
	  break;
	default:
	  write(1, &buf[i], sizeof(char));
	}
      }

      // compression
      if (compress_flag) {
	int compression_buf_size = 1024;
	char compression_buf[compression_buf_size];

	// place buf into the zstream
	to_server.avail_in = ret; 
	to_server.next_in = (Bytef*) buf;
	to_server.avail_out = compression_buf_size;
	to_server.next_out = (Bytef*) compression_buf;
	
	// compress
	do {
	  deflate(&to_server, Z_SYNC_FLUSH);
	} while (to_server.avail_in > 0);
	  
	// replace buf with the compressed buf
	ret = compression_buf_size - to_server.avail_out;
	memcpy(buf, compression_buf, ret);
	write(socketfd, buf, ret);
      } 
      // no compression
      else {
	// send uncompressed input to socket one character at a time
	for (int i = 0; i < ret; i++) {
	  switch (buf[i]) {
	  case '\n':
	  case '\r':
	    write(socketfd, "\r\n", 2*sizeof(char));
	    break;
	  default:
	    write(socketfd, &buf[i], sizeof(char));
	  }
	}
      }
      
      // log the sent bytes all at a time
      if (log_flag) {
	logSent(buf, ret);
      }
    }

    // CASE 2: input from child-parent pipe is detected
    if (polls[1].revents & POLLIN) {
      char buf[256];
      int ret;
      if ((ret = read(socketfd, buf, sizeof(char)*256)) == -1) {
	fprintf(stderr, "Error: could not read from socket\n");
	exit(1);
      }

      // log all received bytes at a time
      if (log_flag) {
	logReceived(buf, ret);
      }
      
      // decompress if compressed
      if (compress_flag) {
        int compression_buf_size = 1024;
        char compression_buf[compression_buf_size];

        // place buf into the zstream
        from_server.avail_in = ret;
        from_server.next_in = (Bytef*) buf;
        from_server.avail_out = compression_buf_size;
        from_server.next_out = (Bytef*) compression_buf;

        // decompress
        do {
      	  inflate(&from_server, Z_SYNC_FLUSH);
        } while (from_server.avail_in > 0);
	
        // replace buf with the decompressed buf
        ret = compression_buf_size - from_server.avail_out;
        memcpy(buf, compression_buf, ret);
      }
      
      // receive uncompressed data from socket
      for (int i = 0; i < ret; i++) {
	if (buf[i] == '\n' || buf[i] == '\r') {
	  write(1, "\r\n", 2*sizeof(char));
	}
	else {
	  write(1, &buf[i], sizeof(char));
	}
      }
    }
    
    // CASE 3: checks for POLLERR and POLLHUP last so that the last bits of output can be received
    if (polls[1].revents & (POLLERR | POLLHUP | POLLRDHUP)) {
      exit(0);
    }
  }
}

int main(int argc, char **argv) {

  // first parse options
  opterr = 0; // suppress automatic stock error message
  struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"log", required_argument, 0, 'l'},
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
    else if (opt == 'l') {
      log_flag = 1;
      logfd = open(optarg, O_CREAT | O_WRONLY, 0666);
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
  // for the client socket setup:
  // http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/client.c
  
  // data structures needed for socket setup and communication
  struct sockaddr_in serv_addr;
  struct hostent *server;
  
  // establish socket
  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Error: could not open socket\n");
    exit(1);
  }
  
  // get server name
  if ((server = gethostbyname("localhost")) == NULL) {
    fprintf(stderr, "Error: could not locate localhost\n");
    exit(1);
  }
  
  // set up serv_addr
  memset((char*) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char*) &serv_addr.sin_addr.s_addr,
	 (char*) server->h_addr, server->h_length);
  serv_addr.sin_port = htons(port_no);
  
  // set up connection
  if (connect(socketfd,(struct sockaddr*) &serv_addr,
	      sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error: could not establish connection to server\n");
    exit(1);
  }
  
  // set up terminal for no echo and non-canonical input mode
  terminalSetup();
  fprintf(stderr, "run\n");
  run();
  
  exit(0);
  
}
