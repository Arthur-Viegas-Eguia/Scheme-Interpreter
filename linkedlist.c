#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"

// creates a new NULL_TYPE value node
Value *makeNull(){
	// allocate space on the heap
    Value *nullValue = talloc(sizeof(Value)); 
    // make a pointer there
    nullValue->type = NULL_TYPE;
    return nullValue;
}

// creates a new CONS_TYPE value node
Value *cons(Value *newCar, Value *newCdr){
	// allocate the space
    Value *consCell = talloc(sizeof(Value));
    // specifying the type 
    consCell->type = CONS_TYPE;
    // initializing car and cdr
    consCell->c.car = newCar;
    consCell->c.cdr = newCdr;
    return consCell;
}


// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list){
    assert(list->type == CONS_TYPE);
	return list->c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list){
	assert(list->type == CONS_TYPE);
	return list->c.cdr;
}

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value){
    if(value->type == NULL_TYPE){
	    return 1;
    }
    else{
    	return 0;
    }
}

// displays the contents of the linked list to the screen in readable format
void display(Value *list){

    Value *curr = list;
    while (!isNull(curr))
    {
        if(car(curr)->type == INT_TYPE){
            printf("%i\n", car(curr)->i);
        }
        else if(car(curr)->type == DOUBLE_TYPE){
            printf("%lf\n", car(curr)->d);
        }

        else if(car(curr)->type == STR_TYPE){
            printf("%s\n", car(curr)->s);
        }
        curr = cdr(curr);
    }       
}

// reverses the original list
Value *reverse(Value *list){
	Value *newList = makeNull();
    Value *curr = list;
    while (curr->type != NULL_TYPE) {
    	Value *newCar = talloc(sizeof(Value)); 
        *newCar = *car(curr);
        newList = cons(newCar, newList);
        curr = cdr(curr);
    }
    return newList;
}

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value){
    int i = 0;
    Value *curr = value;
    while (!isNull(curr))
    {
        i++;
        curr = cdr(curr);
    }   
    return i;
}
