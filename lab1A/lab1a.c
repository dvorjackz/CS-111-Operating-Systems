// NAME: Jack Zhang
// EMAIL: dvorjackz@ucla.edu

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <getopt.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

struct termios initial_attr;

int shell_mode = 0;
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

void childShell() {
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

void parentShell() {
  // set up poll infrastructure
  struct pollfd polls[2];
  // poll for input from stdin
  polls[0].fd = 0;
  polls[0].events = POLLIN | POLLERR | POLLHUP;
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
      if ((ret = read(0, buf, sizeof(char)*128)) == -1) {
	fprintf(stderr, "Error: could not read from stdin\n");
	exit(1);
      }
      for (int i = 0; i < ret; i++) {
	switch (buf[i]) {
	case '\3':
	  write(1, "^C", 2*sizeof(char));
	  kill(pid, SIGINT); // pid of the child process since we are in the parent process
	  break;
	case '\4':
	  write(1, "^D", 2*sizeof(char));
	  close(pipepc[1]);
	  break;
	case '\n':
	case '\r':
	  write(1, "\r\n", 2*sizeof(char));
	  write(pipepc[1], "\n", sizeof(char));
	  break;
	default:
	  write(1, &buf[i], sizeof(char));
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
      for (int i = 0; i < ret; i++) {
	if (buf[i] == '\n') {
	  write(1, "\r\n", 2*sizeof(char));
	}
	else {
	  write(1, &buf[i], sizeof(char));
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

void noShell() {
  char buf[128];
  int ret;
  while ((ret = read(0, buf, sizeof(char)*128)) > 0) {
    for (int i = 0; i < ret; i++) {
      switch (buf[i]) {
      case '\3':
	write(1, "^C", 2*sizeof(char));
	exit(0);
	break;
      case '\4':
	write(1, "^D", 2*sizeof(char));
	exit(0);
	break;
      case '\n':
      case '\r':
	write(1, "\r\n", 2*sizeof(char));
	break;
      default:
	write(1, &buf[i], sizeof(char));
      }
    }
  }
}

int main(int argc, char **argv) {

  // first parse options
  opterr = 0; // suppress automatic stock error message
  struct option long_options[] = {
    {"shell", no_argument, 0, 's'},
    {0, 0, 0, 0}
  };

  char opt = getopt_long(argc, argv, "", long_options, NULL);
  int shell = 0;

  if (opt == 's') {
    shell = 1;
  }
  else if (opt == '?') {
    fprintf(stderr, "Error: Unrecognized option. Only available option is --shell\n");
    exit(1);
  }

  // set up terminal for no echo and non-canonical input mode
  terminalSetup();

  // activate shell mode if option has been specified
  if (shell == 1) {
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
      childShell();
    }
    // parent process
    else {
      parentShell();
    }
  }
  else {
    noShell();
  }
}
