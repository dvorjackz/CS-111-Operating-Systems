// NAME: Jack Zhang
// EMAIL: dvorjackz@ucla.edu
// ID: 004993345

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "SortedList.h"

int numThreads = 1;
int numIterations = 1;
int yieldFlag = 0;

SortedListElement_t* elements;
SortedList_t* list;

char lockType = 'n';
pthread_mutex_t mutex;
int spin = 0;
long long *threadLockTimes;

int opt_yield = 0;
char opt_tag[5] = "";

void segHandler(){
  
  fprintf(stderr, "Segmentation fault\n");
  exit(2);
  
}

void* run_thread(void* ind) {

  struct timespec lockStartTime;
  struct timespec lockEndTime;
  
  int start = *((int*) ind);
  for (int i = start; i < start + numIterations; i++) {
    if (lockType == 'n') {
      SortedList_insert(list, &elements[i]);
    }
    else if (lockType == 'm') {
      clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
      pthread_mutex_lock(&mutex);
      clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
	(lockEndTime.tv_nsec - lockStartTime.tv_nsec);
      SortedList_insert(list, &elements[i]);
      pthread_mutex_unlock(&mutex);
    }
    else if (lockType == 's') {
      clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
      while (__sync_lock_test_and_set(&spin, 1)) {
	continue;
      }
      clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
	(lockEndTime.tv_nsec - lockStartTime.tv_nsec);
      SortedList_insert(list, &elements[i]);
      __sync_lock_release(&spin);
    }
  }
  
  int length = 0;
  if (lockType == 'n') {
    length = SortedList_length(list);
  }
  else if (lockType == 'm') {
    clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
    pthread_mutex_lock(&mutex);
    clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
        (lockEndTime.tv_nsec - lockStartTime.tv_nsec);
    length = SortedList_length(list);
    pthread_mutex_unlock(&mutex);
  }
  else if (lockType == 's') {
    clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
    while (__sync_lock_test_and_set(&spin, 1)) {
      continue;
    }
    clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
        (lockEndTime.tv_nsec - lockStartTime.tv_nsec);
    length = SortedList_length(list);
    __sync_lock_release(&spin);
  }
  if (length < 0) {
    fprintf(stderr, "Error: length is negative\n");
    exit(1);
  }

  SortedListElement_t* toDelete;
  for (int i = start; i < start + numIterations; i++) {
    if (lockType == 'n') {
      toDelete = SortedList_lookup(list, elements[i].key);
      if (SortedList_delete(toDelete)) {
        fprintf(stderr, "Error: could not delete list element\n");
      }
    }
    else if (lockType == 'm') {
      clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
      pthread_mutex_lock(&mutex);
      clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
        (lockEndTime.tv_nsec - lockStartTime.tv_nsec);
      toDelete = SortedList_lookup(list, elements[i].key);
      if (SortedList_delete(toDelete)) {
        fprintf(stderr, "Error: could not delete list element\n");
      }
      pthread_mutex_unlock(&mutex);
    }
    else if (lockType == 's') {
      clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
      while (__sync_lock_test_and_set(&spin, 1)) {
	continue;
      }
      clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
        (lockEndTime.tv_nsec - lockStartTime.tv_nsec);
      toDelete = SortedList_lookup(list, elements[i].key);
      if (SortedList_delete(toDelete)) {
	fprintf(stderr, "Error: could not delete list element\n");
      }
      __sync_lock_release(&spin);
    }
  }
  
  return NULL;
  
}

int main(int argc, char **argv){
  
  opterr = 0; // suppress automatic stock error message

  static struct option long_options[] = {
					 {"threads", required_argument, 0, 't'},
					 {"iterations", required_argument, 0, 'i'},
					 {"yield", required_argument, 0, 'y'},
					 {"sync", required_argument, 0, 's'},
					 {0, 0, 0, 0}
  };

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
      if (strlen(optarg) > 3) {
	fprintf(stderr, "Error: too many arguments for yield\n");
	exit(1);
      }
      for (int i = 0; i < (int) strlen(optarg); i++) {
	if (optarg[i] == 'i') {
	  opt_yield |= INSERT_YIELD;
	  strcat(opt_tag, "i");
	}
	else if (optarg[i] == 'd') {
	  opt_yield |= DELETE_YIELD;
	  strcat(opt_tag, "d");
	}
	else if (optarg[i] == 'l') {
	  opt_yield |= LOOKUP_YIELD;
	  strcat(opt_tag, "l");
	}
	else {
	  fprintf(stderr, "Error: invalid argument for yield: %c\n", optarg[i]);
	  exit(1);
	}
      }
    }
    else if (opt == 's') {
      lockType = *optarg;
      if (lockType == 'm')
	pthread_mutex_init(&mutex, NULL);
    }
    else {
      fprintf(stderr, "Error: invalid option or missing argument\n");
      exit(1);
    }
  }
  
  // finish up parsing opt_tag
  if (strlen(opt_tag) == 0) {
    strcat(opt_tag, "none");
  }

  // first create the dummy head node
  list = malloc(sizeof(SortedList_t));
  list->key = NULL;
  list->next = list->prev = list;
  
  // allocate the random keys into the thread elements
  int numElements = numThreads * numIterations;
  elements = malloc(numElements * sizeof(SortedListElement_t));
  srand((unsigned int) time(NULL));
  for (int i = 0; i < numElements; i++){
    char* key = malloc(2 * sizeof(char));
    key[0] = rand() % 26 + 'A';
    key[1] = '\0';
    elements[i].key = key;
  }

  signal(SIGSEGV, segHandler);

  // for getting total time spent on locking for each thread
  threadLockTimes = malloc(sizeof(long long) * numThreads);
  
  // make sure only the thread creation and joining occur between start and end time
  struct timespec startTime;
  clock_gettime(CLOCK_MONOTONIC, &startTime);
  
  // create the threads
  pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
  int *starts = malloc(numThreads * sizeof(int));
	
  // start the threads
  for (int i = 0; i < numThreads; i++){
    starts[i] = i * numIterations;
    if (pthread_create(&threads[i], NULL, run_thread, &starts[i])){
      fprintf(stderr, "Error: could not create thread\n");
      exit(1);
    }
  }

  // wait for threads to finish
  for (int i = 0; i < numThreads; i++){
    if (pthread_join(threads[i], NULL)){
      fprintf(stderr, "Error: could not call join() on threads\n");
      exit(1);
    }
  }
	
  struct timespec endTime;
  clock_gettime(CLOCK_MONOTONIC, &endTime);

  long long totalLockTime = 0;
  for (int i = 0; i < numThreads; i++) {
    totalLockTime += threadLockTimes[i]; 
  }

  int listLength = SortedList_length(list);
  if (listLength > 0) {
    fprintf(stderr, "Error: list length is not 0\n");
    exit(2);
  }
  else if (listLength < 0) {
    fprintf(stderr, "Error: list length is negative\n");
    exit(2);
  }
  
  long long elapsedTime = (endTime.tv_sec - startTime.tv_sec)*1000000000 + (endTime.tv_nsec - startTime.tv_nsec);
  long long noOperations = 3 * numThreads * numIterations;
  int avgTime = elapsedTime/noOperations;
  long long avgLockTime = totalLockTime/noOperations;
  
  char testName[100] = "list-";
  strcat(testName, opt_tag);
  strcat(testName, "-");
  switch(lockType) {
  case 'n':
    strcat(testName, "none");
    break;
  case 'm':
    strcat(testName, "m");
    break;
  case 's':
    strcat(testName, "s");
    break;
  default:
    fprintf(stderr, "Could not find the specified lock type: %c\n", lockType);
  }

  fprintf(stdout, "%s,%d,%d,1,%lld,%lld,%d,%lld\n", testName, numThreads, numIterations, noOperations, elapsedTime, avgTime, avgLockTime);
  
  free(threads);
  free(starts);
  free(list);
  for (int i = 0; i < numElements; i++)
    free((char*) elements[i].key);
  free(elements);

  exit(0);
  
}
