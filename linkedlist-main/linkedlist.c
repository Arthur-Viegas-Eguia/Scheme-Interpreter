#include "value.h"
#include "linkedlist.h"

// Create a new NULL_TYPE value node.
Value *makeNull(){
    Value *nullValue = malloc(sizeof(Value)); 
    nullValue->type = NULL_TYPE;
    return nullValue;

}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr){
    Value *consCell = malloc(sizeof(Value)); 
    consCell->type = CONS_TYPE;
    consCell->c.car = newCar;
    consCell->c.cdr = newCdr;
    return consCell;
}

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list){
    if(list->type != NULL_TYPE){
        Value *curr = list->c.car;
    }
    while (list->c.cdr != NULL_TYPE)
    {
        switch (list->type) {
            case INT_TYPE:
                printf("(%i)->", list->i);
                break;

            case DOUBLE_TYPE:
                printf("(%f)->", list->d);
                break;
                
            case STR_TYPE:
                printf("(%s)->", list->s);
                break;
        }
    }
    printf("()\n");
    
}




Value *reverse(Value *list){
    Value *previous, *current, *temp;
    previous->type = NULL_TYPE;
    current->c.car = list->c.car;
    while (current->c.cdr != NULL_TYPE){
        temp->c.car 
    }
}