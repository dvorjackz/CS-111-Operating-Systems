// /NAME: Jack Zhang
//EMAIL: dvorjackz@ucla.edu
//ID: 004993345

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

long long counter = 0;

int numThreads;
int numIterations;
int yieldFlag = 0;

char lockType = 'n';
pthread_mutex_t mutex;
int spin = 0;

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (yieldFlag) { 
    sched_yield();
  }
  *pointer = sum;
}

void* addWrapper(){
  for (int delta = 1; delta > -2; delta -= 2) {
    for (int i = 0; i < numIterations; i++) {
      if (lockType == 'n') {
	add(&counter, delta);
      }
      else if (lockType == 'm') {
	pthread_mutex_lock(&mutex);
	add(&counter, delta);
	pthread_mutex_unlock(&mutex);
      }
      else if (lockType == 's') {
	while (__sync_lock_test_and_set(&spin, 1)) {
	}
	add(&counter, delta);
	__sync_lock_release(&spin);
      }
      else if (lockType == 'c') {
	long long old;
	long long new;
	for (;;) {
	  old = counter;
	  new = old + delta;
	  if (yieldFlag) {
	    sched_yield();
	  }
	  if (__sync_val_compare_and_swap(&counter, old, new) == old) {
	    break;
	  }
	}
      }
    }
  }
  return NULL;
}

int main(int argc, char **argv){

  opterr = 0; // suppress automatic stock error message

  static struct option long_options[] = {
					 {"threads", required_argument, 0, 't'},
					 {"iterations", required_argument, 0, 'i'},
					 {"yield", no_argument, 0, 'y'},
					 {"sync", required_argument, 0, 's'},
					 {0, 0, 0, 0}
  };

  numThreads = 1;
  numIterations = 1;

  char opt;
  while (1) {
    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt == -1) {
      break;
    }
    else if (opt == 't') {
      numThreads = atoi(optarg);
    }
    else if (opt == 'i') {
      numIterations = atoi(optarg);
    }
    else if (opt == 'y') {
      yieldFlag = 1;
    }
    else if (opt == 's') {
      lockType = *optarg;
      if (lockType == 'm')
	pthread_mutex_init(&mutex, NULL);
    }
    else {
      fprintf(stderr, "Error: unrecognized option or missing argument\n");
      exit(1);
    }
  }
	
  struct timespec startTime;
  clock_gettime(CLOCK_MONOTONIC, &startTime);
	
  pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
  for (int i = 0; i < numThreads; i++){
    if (pthread_create(&threads[i], NULL, addWrapper, NULL)){
      fprintf(stderr, "Error: could not create thread\n");
      exit(1);
    }	
  }

  for (int i = 0; i < numThreads; i++){
    if (pthread_join(threads[i], NULL)){
      fprintf(stderr, "Error: could not call join() on threads\n");
      exit(1);
    }
  }
	
  struct timespec endTime;
  clock_gettime(CLOCK_MONOTONIC, &endTime);
	
  long long elapsedTime = (endTime.tv_sec - startTime.tv_sec)*1000000000 + (endTime.tv_nsec - startTime.tv_nsec);
  int noOperations = 2*numThreads*numIterations;
  int avgTime = elapsedTime/noOperations;

  char* testName;
  switch(lockType) {
  case 'n':
    testName = yieldFlag ? "add-yield-none" : "add-none";
    break;
  case 'm':
    testName = yieldFlag ? "add-yield-m" : "add-m";
    break;
  case 's':
    testName = yieldFlag ? "add-yield-s" : "add-s";
    break;
  case 'c':
    testName = yieldFlag ? "add-yield-c" : "add-c";
    break;
  default:
    fprintf(stderr, "Could not find the specified lock type: %c\n", lockType);
  }

  fprintf(stdout, "%s,%d,%d,%d,%lld,%d,%lld\n", testName, numThreads, numIterations, 
	  noOperations, elapsedTime, avgTime, counter);
	
  free(threads);

  exit(0);
}
