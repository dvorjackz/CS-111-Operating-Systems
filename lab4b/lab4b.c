// NAME: Jack Zhang
// EMAIL: dvorjackz@ucla.edu
// ID: 004993345

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#define FAHRENHEIT 0
#define CELSIUS 1

// option variables
int logFlag = 0;
int logFd;
int period = 1;
int scale = FAHRENHEIT;

void segHandler(){
  fprintf(stderr, "Segmentation fault\n");
  exit(2); 
}

int main(int argc, char **argv){
  
  opterr = 0; // suppress automatic stock error message

  static struct option long_options[] = {
					 {"period", required_argument, 0, 'p'},
					 {"scale", required_argument, 0, 's'},
					 {"log", required_argument, 0, 'l'},
					 {0, 0, 0, 0}
  };

  // parse options
  char opt;
  while (1) {
    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == -1) {
      break;
    }
    else if (opt == 'p') {
      period = atoi(optarg);
    }
    else if (opt == 's') {
      char sc = *optarg;
      switch (sc) {
      case 'C':
	scale = CELSIUS;
	break;
      case 'F':
	break;
      default:
	fprintf(stderr, "Incorrect scale option arguments. Please pass in either 'C' or 'F'.\n");
	exit(1);
      }
    }
    else if (opt == 'l') {
      logFlag = 1;
      if ((logFd = open(optarg, O_CREAT | O_WRONLY, 0666)) == -1) {
	fprintf(stderr, "Could not open/create file for logging purposes.\n");
      }
    }
    else {
      fprintf(stderr, "Error: invalid option or missing argument.\n");
      exit(1);
    }
  }

  struct timeval currTime; 
  if ((gettimeofday(&currTime, NULL)) == -1) {
    fprintf(stderr, "Error: could not retrieve time.\n");
  }
  time_t nextTime = currTime.tv_sec + period;
  

  while (1) {
    if ((gettimeofday(&currTime, NULL)) == -1) {
      fprintf(stderr, "Error: could not retrieve time.\n");
    }
    if (currTime.tv_sec >= nextTime) {
      nextTime = currTime.tv_sec + period;
      fprintf(stdout, "%d second(s) have elapsed.\n", period);
    }
  }
  
  exit(0);
  
}
