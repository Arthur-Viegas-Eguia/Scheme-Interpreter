#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "tokenizer.h"

//Prints the respective scheme program on the screen
void printTree(Value *tree){
    Value *curr = tree;
    while (!isNull(curr))
    {
        if(car(curr)->type == INT_TYPE){
            printf("%i ", car(curr)->i);
        }
        else if(car(curr)->type == DOUBLE_TYPE){
            printf("%lf ", car(curr)->d);
        }

        else if(car(curr)->type == STR_TYPE){
            printf("%s ", car(curr)->s);
        }
        else if(car(curr)->type == BOOL_TYPE){
            if(car(curr)->i == 1){
                printf("#t ");
            }
            else{
                printf("#f ");
            }
        }
        else if(car(curr)->type == SYMBOL_TYPE){
            printf("%s ", car(curr)->s);
        }
        else if(car(curr)->type == NULL_TYPE){
            printf("() ");
        }
        else if(car(curr)->type == CONS_TYPE){
            printf("( ");
            printTree(car(curr));
            printf(") ");
        }
        curr = cdr(curr);
    }   
}


//Adds a given token/expression to the parse tree
Value *addToParseTree(Value *tree, int *depth, Value *Token){
    //If the token isn't a close parens, it just adds it to the stack
    if(Token->type != CLOSE_TYPE){
        //If it is an open parens, we increase the number of depth levels in the tree by one
        if(Token->type == OPEN_TYPE){
            (*depth)++;
        }
        return cons(Token, tree);
    }
    else{ 
        //If it is a close parens, it groups everything until the last open
        //Parens in a linked list (which represents an expression), and adds this list/expression to the stack
        Value *list = makeNull();
        while(tree->type != NULL_TYPE && car(tree)->type != OPEN_TYPE){
            list = cons(car(tree), list);
            tree = cdr(tree);
        }
        //If we did not reach the end of the tree we concatenate the expression to the stack
        if(tree->type != NULL_TYPE){
            tree = cdr(tree);
            tree = cons(list, tree);
            (*depth)--;
        }
        //If we have, it means that we don't have enough closing parens, in which case the code launches an error code
        else{
            printf("Syntax error: too many close parentheses\n");
            texit(0);
        }
        return tree;
    }
}

//Builds the parse tree of the program
Value *parse(Value *tokens) {
    //Creates the stack/parse tree
    Value *tree = makeNull();
    int depth = 0;

    Value *current = tokens;
    assert(current != NULL && "Error (parse): null pointer");
    //Iterates through all the tokes, adding them to the stack
    while (current->type != NULL_TYPE) {
        Value *token = car(current);
        tree = addToParseTree(tree, &depth, token);
        current = cdr(current);
    }
    //If we did not address one or more levels of the tree, it means that there were not enough closing parens
    if (depth > 0) {
        printf("Syntax error: not enough close parentheses\n");
        texit(0);
    }
    return reverse(tree);
}
