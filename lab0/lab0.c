/* NAME: Jack Zhang */
/* EMAIL: dvorjackz@ucla.edu */

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

void segFault() {
  char* a = NULL;
  *a = 'a';
}

void sigHandler() {
  fprintf(stderr, "Segmentation fault caught.\n");
  exit(4);
}

int main(int argc, char **argv) {
  struct option long_options[] = {
    {"input", 1, 0, 'i'},
    {"output", 1, 0, 'o'},
    {"segfault", 0, 0, 's'},
    {"catch", 0, 0, 'c'},
    {0, 0, 0, 0}
  };
  char* fileInput = NULL;
  char* fileOutput = NULL;
  int seg = 0;
  int catch = 0;
  char opt;
  while ((opt = (getopt_long(argc, argv, "", long_options, NULL))) != -1) {
      switch (opt) {
      case 'i':
	fileInput = optarg;
	break;
      case 'o':
	fileOutput = optarg;
	break;
      case 's':
	seg = 1;
	break;
      case 'c':
	catch = 1;
	break;
      default:
	fprintf(stderr, "Incorrect option format. Correct usage: lab0 --input=filepath --output=filepath --segfault --catch");
	exit(1);
      }
  }
  if (catch == 1) {
    signal(SIGSEGV, sigHandler);
  }
  if (seg == 1) {
    segFault();
  }
  int fdi, fdo;
  if (fileInput) {
    fdi = open(fileInput, O_RDONLY);
    if (fdi >= 0) {
      close(0);
      dup(fdi);
      close(fdi);
    }
    else {
      fprintf(stderr, "Could not open file %s. Error code: %d. Error message: %s.\n", fileInput, errno, strerror(errno));
      exit(2);
    }
  }
  if (fileOutput) {
    fdo = creat(fileOutput, 0644);
    if (fdo >= 0) {
      close(1);
      dup(fdo);
      close(fdo);
    }
    else {
      fprintf(stderr, "Could not open file %s. Error code: %d. Error message: %s.\n", fileOutput, errno, strerror(errno));
      exit(3);
    }
  }
  char buf[1];
  int r;
  while ((r = (read(0, buf, 1))) == 1) {
    write(1, buf, 1);
  }
  exit(0);
}
