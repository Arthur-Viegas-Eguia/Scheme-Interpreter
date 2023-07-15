#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "value.h"
#include "linkedlist.h"

// global variable - linked list with the pointers
Value *pointers;

// code below is copied from the linkedlist assignment to support the functioning of talloc
// creates a new NULL_TYPE value node
Value *makeNullHelper(){
	// allocate space on the heap
    Value *nullValue = malloc(sizeof(Value)); 
    // make a pointer there
    nullValue->type = NULL_TYPE;
    return nullValue;
}

// creates a new CONS_TYPE value node
Value *consHelper(Value *newCar, Value *newCdr){
	// allocate the space
    Value *consCell = malloc(sizeof(Value));
    // specifying the type 
    consCell->type = CONS_TYPE;
    // initializing car and cdr
    consCell->c.car = newCar;
    consCell->c.cdr = newCdr;
    return consCell;
}


// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *carHelper(Value *list){
    assert(list->type == CONS_TYPE);
	return list->c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdrHelper(Value *list){
	assert(list->type == CONS_TYPE);
	return list->c.cdr;
}

// cleans up the list
void cleanupHelper(Value *list){
    Value *curr = list;
    while (curr->type != NULL_TYPE) {
        Value *next = cdrHelper(curr);
        free(carHelper(curr));
        free(curr);
        curr = next;
    }
    free(curr);
    // AND REINITIALIZES THE POINTERS VARIABLE
    pointers = NULL;
}

// all subsequent functions belong to the talloc assignment
// replacement for malloc that saves the pointers
void *talloc(size_t size){
	// creating a pointer
	Value *pointer = malloc(size);
	// adding it to the list
	// if it hasn't been initialized yet
	if (!pointers){
		// initialize it as an empty list
		pointers = makeNullHelper();
		// and then cons the existing pointer there
		pointers = consHelper(pointer, pointers);
	}
	// if it exists
	else{
		// cons the existing pointer there
		pointers = consHelper(pointer, pointers);
	}
	return pointer;
}

// frees all pointers and memory allocated by talloc
void tfree(){
	// simply runs the cleanupHelper on the existing list
	if (pointers){
		cleanupHelper(pointers);
	}
}

// calls tfree before using exit
void texit(int status){
	tfree();
	exit(status);
}
