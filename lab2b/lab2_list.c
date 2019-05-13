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

// essential vars
int numThreads = 1;
int numIterations = 1;
int yieldFlag = 0;

// list vars
SortedListElement_t *elements;
SortedList_t *lists;
int numSublists = 1; // default is no sublist partitions

// lock vars
char lockType = 'n';
pthread_mutex_t *mutexes;
int *spins;
long long *threadLockTimes;

// output formatting vars
int opt_yield = 0;
char opt_tag[5] = "";

void segHandler(){
  
  fprintf(stderr, "Segmentation fault\n");
  exit(2);
  
}

int hash(char key) {
  return key % numSublists;
}

void* run_thread(void* ind) {

  struct timespec lockStartTime;
  struct timespec lockEndTime;

  // INSERTION
  int start = *((int*) ind);
  for (int i = start; i < start + numIterations; i++) {
    int subListInd = hash(elements[i].key[0]); // index at zero to neglect the null byte
    if (lockType == 'n') {
      SortedList_insert(&lists[subListInd], &elements[i]);
    }
    else if (lockType == 'm') {
      clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
      pthread_mutex_lock(&mutexes[subListInd]);
      clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
	(lockEndTime.tv_nsec - lockStartTime.tv_nsec);
      SortedList_insert(&lists[subListInd], &elements[i]);
      pthread_mutex_unlock(&mutexes[subListInd]);
    }
    else if (lockType == 's') {
      clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
      while (__sync_lock_test_and_set(&spins[subListInd], 1)) {
	continue;
      }
      clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
	(lockEndTime.tv_nsec - lockStartTime.tv_nsec);
      SortedList_insert(&lists[subListInd], &elements[i]);
      __sync_lock_release(&spins[subListInd]);
    }
  }

  // LENGTH
  // checks the length of subList with index 'start'
  int length = 0;
  int randomListIndex = rand() % numSublists;
  if (lockType == 'n') {
    length = SortedList_length(&lists[randomListIndex]);
  }
  else if (lockType == 'm') {
    clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
    pthread_mutex_lock(&mutexes[randomListIndex]);
    clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
        (lockEndTime.tv_nsec - lockStartTime.tv_nsec);
    length = SortedList_length(&lists[randomListIndex]);
    pthread_mutex_unlock(&mutexes[randomListIndex]);
  }
  else if (lockType == 's') {
    clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
    while (__sync_lock_test_and_set(&spins[randomListIndex], 1)) {
      continue;
    }
    clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
        (lockEndTime.tv_nsec - lockStartTime.tv_nsec);
    length = SortedList_length(&lists[randomListIndex]);
    __sync_lock_release(&spins[randomListIndex]);
  }
  if (length < 0) {
    fprintf(stderr, "Error: length is negative\n");
    exit(1);
  }

  // LOOKUP & DELETION
  SortedListElement_t* toDelete;
  for (int i = start; i < start + numIterations; i++) {
    int subListInd = hash(elements[i].key[0]);
    if (lockType == 'n') {
      toDelete = SortedList_lookup(&lists[subListInd], elements[i].key);
      if (SortedList_delete(toDelete)) {
        fprintf(stderr, "Error: could not delete list element\n");
      }
    }
    else if (lockType == 'm') {
      clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
      pthread_mutex_lock(&mutexes[subListInd]);
      clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
        (lockEndTime.tv_nsec - lockStartTime.tv_nsec);
      toDelete = SortedList_lookup(&lists[subListInd], elements[i].key);
      if (SortedList_delete(toDelete)) {
        fprintf(stderr, "Error: could not delete list element\n");
      }
      pthread_mutex_unlock(&mutexes[subListInd]);
    }
    else if (lockType == 's') {
      clock_gettime(CLOCK_MONOTONIC, &lockStartTime);
      while (__sync_lock_test_and_set(&spins[subListInd], 1)) {
	continue;
      }
      clock_gettime(CLOCK_MONOTONIC, &lockEndTime);
      threadLockTimes[start/numIterations] += (lockEndTime.tv_sec - lockStartTime.tv_sec)*1000000000 +
        (lockEndTime.tv_nsec - lockStartTime.tv_nsec);
      toDelete = SortedList_lookup(&lists[subListInd], elements[i].key);
      if (SortedList_delete(toDelete)) {
	fprintf(stderr, "Error: could not delete list element\n");
      }
      __sync_lock_release(&spins[subListInd]);
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
					 {"lists", required_argument, 0, 'l'},
					 {0, 0, 0, 0}
  };

  // parse options
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
    }
    else if (opt == 'l') {
      numSublists = atoi(optarg);
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
  lists = malloc(numSublists * sizeof(SortedList_t));
  for (int i = 0; i < numSublists; i++) {
    lists[i].key = NULL;
    lists[i].next = &lists[i];
    lists[i].prev = &lists[i];
  }

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

  // initialize (if option is set) a mutex for each sublist
  mutexes = malloc(numSublists * sizeof(pthread_mutex_t));
  if (lockType == 'm') {
    for (int i = 0; i < numSublists; i++) {
      pthread_mutex_init(&mutexes[i], NULL);
    }
  }

  // initialize (if option is set) a spin lock for each sublist
  spins = malloc(numSublists * sizeof(int));
  if (lockType == 's') {
    for (int i = 0; i < numSublists; i++) {
      spins[i] = 0;
    }
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

  // add up the wait-for-lock times for each thread
  long long totalLockTime = 0;
  for (int i = 0; i < numThreads; i++) {
    totalLockTime += threadLockTimes[i]; 
  }

  // add up the lengths of all of the sublists (should add up to 0)
  int listLength = 0;
  for (int i = 0; i < numSublists; i++) {
    listLength += SortedList_length(&lists[i]);
  }
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

  fprintf(stdout, "%s,%d,%d,%d,%lld,%lld,%d,%lld\n", testName, numThreads, numIterations, numSublists, noOperations, elapsedTime, avgTime, avgLockTime);
  
  free(threads);
  free(starts);
  free(lists);
  for (int i = 0; i < numElements; i++)
    free((char*) elements[i].key);
  free(elements);
  free(mutexes);
  free(spins);

  exit(0);
  
}
