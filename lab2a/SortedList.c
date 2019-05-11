#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "SortedList.h"

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {

  if (list == NULL || element == NULL) {
    return;
  }
  
  SortedList_t* temp = list->next;
  while (temp != list && strcmp(element->key, temp->key) > 0) {
    temp = temp->next;
  }

  if (opt_yield && INSERT_YIELD) {
    sched_yield();
  }


    element->prev = temp->prev;
    element->next = temp;
    temp->prev->next = element;
    temp->prev = element;
  
}
/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element) {

  if (opt_yield && DELETE_YIELD) {
    sched_yield();
  }
  
  if (element != NULL && element->prev->next != NULL && element->next->prev != NULL) {
    element->prev->next = element->next;
    element->next->prev = element->prev;
    return 0;
  }
  else {
    return 1;
  }
  
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {

  if (list == NULL || key == NULL) {
    return NULL;
  }

  SortedListElement_t *temp = list->next;
  
  while (temp != list) {
    if (strcmp(temp->key, key) == 0) {
      return temp;
    }
    if (opt_yield && LOOKUP_YIELD) {
      sched_yield();
    }
    temp = temp->next;
  }

  return NULL;
  
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list) {

  if (list == NULL) {
    return -1;
  }

  int counter = 0;
  SortedListElement_t *temp = list->next;
  while (temp != list) {
    if (opt_yield && LOOKUP_YIELD) {
      sched_yield();
    }
    counter++;
    temp = temp->next;
  }

  return counter;
  
}
