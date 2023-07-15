#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
// Changes the value of variable var to assign
Value *updateVals(Value *var, Value *assign, Frame *frame)
{
   Value *curr = frame->bindings;
   // Iterates through all the variables in the frame to find var
   while (curr->type != NULL_TYPE)
   {
      // If it is var
      if (!strcmp(var->s, car(car(curr))->s))
      {
         // Sets its value to assign and return void
         curr->c.car->c.cdr = assign;
         Value *nodeVoid = talloc(sizeof(Value));
         nodeVoid->type = VOID_TYPE;
         return nodeVoid;
      }
      curr = cdr(curr);
   }
   // If the variable wasnt found in the current frame, looks for it in the parent frame
   if (frame->parent != NULL)
   {
      updateVals(var, assign, frame->parent);
   }
   // Error if the variable is undefined
   else
   {
      printf("Evaluation error: Cannot apply set! on a variable which was not yet defined\n");
      texit(0);
   }
   return makeNull();
}
// Primitive minus function for scheme
Value *minus(Value *args)
{
   Value *sub = talloc(sizeof(Value));
   // Checks if the length is correct
   if (length(args) == 2)
   {
      // Checks for the correct type of the two arguments and subtracts the second one from the first
      if (car(args)->type != INT_TYPE && car(cdr(args))->type == INT_TYPE)
      {
         sub->type = DOUBLE_TYPE;
         sub->d = car(args)->d - car(cdr(args))->i;
      }
      else if (car(args)->type == INT_TYPE && car(cdr(args))->type != INT_TYPE)
      {
         sub->type = DOUBLE_TYPE;
         sub->d = car(args)->i - car(cdr(args))->d;
      }
      else if (car(args)->type != INT_TYPE && car(cdr(args))->type != INT_TYPE)
      {
         sub->type = DOUBLE_TYPE;
         sub->d = car(args)->d - car(cdr(args))->d;
      }
      else
      {
         sub->type = INT_TYPE;
         sub->i = car(args)->i - car(cdr(args))->i;
      }
   }
   // If the wrong number of elements were passed, prints an error
   else
   {
      printf("Evaluation error: wrong number of arguments for minus\n");
      texit(0);
   }
   // Returns the result of the subtraction
   return sub;
}
// Primitive function for scheme division
Value *divide(Value *args)
{
   Value *div = talloc(sizeof(Value));
   // Checks if the length is correct
   if (length(args) == 2)
   {
      // Checks for the type of the two arguments
      // Once the type is found, sees if the division returns an integer value
      // If so divides the numbers and saves their result as an integer
      // If their division does not return an integer, save the result of the
      // Division in a float
      if (car(args)->type != INT_TYPE && car(cdr(args))->type == INT_TYPE)
      {
         if (fmod(car(args)->d, car(cdr(args))->i) == 0)
         {
            div->type = INT_TYPE;
            div->i = car(args)->d / car(cdr(args))->i;
         }
         else
         {
            div->type = DOUBLE_TYPE;
            div->d = car(args)->d / car(cdr(args))->i;
         }
      }
      else if (car(args)->type == INT_TYPE && car(cdr(args))->type != INT_TYPE)
      {
         if (fmod(car(args)->i, car(cdr(args))->d) == 0)
         {
            div->type = INT_TYPE;
            div->i = car(args)->i / car(cdr(args))->d;
         }
         else
         {
            div->type = DOUBLE_TYPE;
            div->d = car(args)->i / car(cdr(args))->d;
         }
      }
      else if (car(args)->type != INT_TYPE && car(cdr(args))->type != INT_TYPE)
      {
         if (fmod(car(args)->d, car(cdr(args))->d) == 0)
         {
            div->type = INT_TYPE;
            div->i = car(args)->d / car(cdr(args))->d;
         }
         else
         {
            div->type = DOUBLE_TYPE;
            div->d = car(args)->d / car(cdr(args))->d;
         }
      }
      else
      {
         if (fmod(car(args)->i, car(cdr(args))->i) == 0)
         {
            div->type = INT_TYPE;
            div->i = car(args)->i / car(cdr(args))->i;
         }
         else
         {
            // Handles edge case of integer division in c
            double frac = car(args)->i, tion = car(cdr(args))->i;
            div->type = DOUBLE_TYPE;
            div->d = frac / tion;
         }
      }
   }
   // Handles the case if the wrong number of arguments were passed
   else
   {
      printf("Evaluation error: wrong number of arguments for minus\n");
      texit(0);
   }
   // Returns the result of the division
   return div;
}

// Primitive multiplication for scheme
Value *times(Value *args)
{
   // If no arguments are passed, returns 0
   if (isNull(args))
   {
      return 0;
   }
   // Otherwise initializes the value of the multiplication as 1
   // And of type integer
   Value *curr = args;
   Value *total = talloc(sizeof(Value));
   total->type = INT_TYPE;
   total->i = 1;
   // This indicates that the value is not a float
   int isDouble = 0;
   // Multiplies every element in the list of arguments
   while (!isNull(curr))
   {
      // If multiplying by an integer
      if (car(curr)->type == INT_TYPE)
      {
         // If the total is an integer just multiplies them together
         if (!isDouble)
         {
            total->i = total->i * car(curr)->i;
         }
         // If it is a double multiply them together
         else
         {
            total->d = total->d * car(curr)->i;
         }
      }
      // If we are multiplying a double and the total is an integer
      else if (car(curr)->type == DOUBLE_TYPE && !isDouble)
      {
         // We convert the total to double and multiply them together
         double d = total->i;
         total->type = DOUBLE_TYPE;
         total->d = d * car(curr)->d;
         isDouble = 1;
      }
      // If we are multiplying a double and the total is a double
      else if (car(curr)->type == DOUBLE_TYPE)
      {
         // Multiply them together
         total->d = total->d * car(curr)->d;
      }
      // If the value is not a number returns an error
      else
      {
         printf("Evaluation error: cannot add a non numeric value :V!\n");
         texit(0);
      }
      curr = cdr(curr);
   }
   return total;
}
// Primitive equals for scheme
// Checks if two values are equal to each other
Value *equals(Value *args)
{
   Value *boolVal = talloc(sizeof(Value));
   // Initializes the default return value to false
   boolVal->type = BOOL_TYPE;
   boolVal->i = 0;
   // Checks if the values have the same type, if so checks if they are equal. If that is the case set
   // Return value to true.
   if (car(args)->type == INT_TYPE && car(cdr(args))->type == INT_TYPE)
   {
      if (car(args)->i == car(cdr(args))->i)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == DOUBLE_TYPE && car(cdr(args))->type == DOUBLE_TYPE)
   {
      if (car(args)->d == car(cdr(args))->d)
      {
         boolVal->i = 1;
      }
   }
   // Handles edge cases of comparing integer to double or double to integer
   else if (car(args)->type == DOUBLE_TYPE && car(cdr(args))->type == INT_TYPE)
   {
      if (car(args)->d == car(cdr(args))->i)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == INT_TYPE && car(cdr(args))->type == DOUBLE_TYPE)
   {
      if (car(args)->i == car(cdr(args))->d)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == STR_TYPE && car(cdr(args))->type == STR_TYPE)
   {
      if (!strcmp(car(args)->s, car(cdr(args))->s))
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == BOOL_TYPE && car(cdr(args))->type == BOOL_TYPE)
   {
      if (car(args)->i == car(cdr(args))->i)
      {
         boolVal->i = 1;
      }
   }
   return boolVal;
}
// Primitive module function between two integers
// Returns the mod of the two arguments passed
Value *mod(Value *args)
{
   Value *mod = talloc(sizeof(Value));
   mod->type = INT_TYPE;
   mod->i = car(args)->i % car(cdr(args))->i;
   return mod;
}
// Primitive greater than function for scheme
// Checks if the first element is greater than the second element
Value *greater(Value *args)
{
   // sets default value to false
   Value *boolVal = talloc(sizeof(Value));
   boolVal->type = BOOL_TYPE;
   boolVal->i = 0;
   // Checks and finds the correct argument types. Once the code knows the type
   // Of the arguments, it sets the return result to true if the first argument is greater
   // Than the second argument
   if (car(args)->type == DOUBLE_TYPE && car(cdr(args))->type == DOUBLE_TYPE)
   {
      if (car(args)->d > car(cdr(args))->d)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == DOUBLE_TYPE && car(cdr(args))->type == INT_TYPE)
   {
      if (car(args)->d > car(cdr(args))->i)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == INT_TYPE && car(cdr(args))->type == DOUBLE_TYPE)
   {
      if (car(args)->i > car(cdr(args))->d)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == INT_TYPE && car(cdr(args))->type == INT_TYPE)
   {
      if (car(args)->i > car(cdr(args))->i)
      {
         boolVal->i = 1;
      }
   }
   return boolVal;
}
// Primitive less than function for scheme
// Checks if the first element is less than the second element
Value *less(Value *args)
{
   // sets default value to false
   Value *boolVal = talloc(sizeof(Value));
   boolVal->type = BOOL_TYPE;
   boolVal->i = 0;
   // Checks and finds the correct argument types. Once the code knows the type
   // Of the arguments, it sets the return result to true if the first argument is less
   // Than the second argument
   if (car(args)->type == DOUBLE_TYPE && car(cdr(args))->type == DOUBLE_TYPE)
   {
      if (car(args)->d < car(cdr(args))->d)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == DOUBLE_TYPE && car(cdr(args))->type == INT_TYPE)
   {
      if (car(args)->d < car(cdr(args))->i)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == INT_TYPE && car(cdr(args))->type == DOUBLE_TYPE)
   {
      if (car(args)->i < car(cdr(args))->d)
      {
         boolVal->i = 1;
      }
   }
   else if (car(args)->type == INT_TYPE && car(cdr(args))->type == INT_TYPE)
   {
      if (car(args)->i < car(cdr(args))->i)
      {
         boolVal->i = 1;
      }
   }
   return boolVal;
}
// Helper function to let* creates a new frame for each local variable
// Returns the frame of the last local variable
Frame *letAux(Value *curr, Frame *fr)
{
   // Creates a new frame and a cons cell for the variable.
   // The car of the cons cell is the name of the variable
   // The cdr of the cons cell is the evaluated value of the variable
   Value *val = talloc(sizeof(Value));
   val->type = CONS_TYPE;
   val->c.car = car(car(curr));
   val->c.cdr = eval(car(cdr(car(curr))), fr);
   Frame *newFrame = talloc(sizeof(Frame));
   newFrame->parent = fr;
   newFrame->bindings = makeNull();
   // Binds cons cell to frame
   newFrame->bindings = cons(val, newFrame->bindings);
   curr = cdr(curr);
   // If there are more variables in the list, calls fucntion recursively
   if (curr->type != NULL_TYPE)
   {
      return letAux(curr, newFrame);
   }
   // Base case, returns latest frame
   else
   {
      return newFrame;
   }
}

// void printFrame(Frame *frame)
// {
//     if (frame->parent != NULL)
//     {
//         printFrame(frame->parent);
//     }
// }
// Binds arguments for letrec
void bindLetRec(Value *args, Frame *frame)
{
   Value *curr = args;
   while (!isNull(curr))
   {
      // Initializes a value
      Value *val = talloc(sizeof(Value));
      val->type = CONS_TYPE;
      // Checks if the variable declaration is in the correct format, that is, (varName(symbol) value)
      if (car(curr)->type == CONS_TYPE && length(car(curr)) == 2 && car(car(curr))->type == SYMBOL_TYPE)
      {
         // Checks if the variable has not been declared in the same frame
         if (alreadyDeclared(car(car(curr)), frame->bindings))
         {
            // If so, creates an undefined value, sets variable to (varName(symbol) unspecified)
            Value *undefVal = talloc(sizeof(Value));
            undefVal->type = UNSPECIFIED_TYPE;
            val->c.car = car(car(curr));
            val->c.cdr = undefVal;
            // And then add the variable to the stack frame
            frame->bindings = cons(val, frame->bindings);
         }
         // If the variable has been declared more than once in the frame, the code shows and error message
         else
         {
            printf("Evaluation Error: Cannot declare a variable twice!\n");
            texit(0);
         }
      }

      // If it is not in the correct form, then it is an error
      else
      {
         printf("Evaluation Error: wrong format for variable declaration!\n");
         texit(0);
      }
      curr = cdr(curr);
   }
   Value *curr2 = args;
   Value *semiEvaluated = makeNull();
   // Evaluates all the values of each variables and saves them
   while (!isNull(curr2))
   {
      Value *var = talloc(sizeof(Value));
      var->type = CONS_TYPE;
      var->c.car = car(car(curr2));
      // If the variable evaluates to unspecified type returns an error message
      Value *check = eval(car(cdr(car(curr2))), frame);
      if (check->type == UNSPECIFIED_TYPE)
      {
         printf("Evaluation error: cannot use type unspecified\n");
         texit(0);
      }
      // Otherwise save the evaluated result in the list
      var->c.cdr = eval(car(cdr(car(curr2))), frame);
      semiEvaluated = cons(var, semiEvaluated);
      curr2 = cdr(curr2);
   }
   Value *curr3 = semiEvaluated;
   // Binds each value to the appropriate variable
   while (!isNull(curr3))
   {
      Value *curFrame = frame->bindings;
      while (!isNull(curFrame))
      {
         if (!strcmp(car(car(curFrame))->s, car(car(curr3))->s))
         {
            curFrame->c.car->c.cdr = cdr(car(curr3));
            break;
         }
         curFrame = cdr(curFrame);
      }
      curr3 = cdr(curr3);
   }
}
// evaluates each argument cell in a list
Value *evalEach(Value *args, Frame *frame)
{
   // assign the current argument
   Value *curr = args;
   Value *evaledArgs = makeNull();
   // while we still have some
   while (curr->type != NULL_TYPE && car(curr)->type != NULL_TYPE)
   {
      // evaluate its car
      evaledArgs = cons(eval(curr->c.car, frame), evaledArgs);
      // and move forward
      curr = cdr(curr);
   }
   return reverse(evaledArgs);
}
// Binds primitive functions to their respective name (i.e. +, -, /, among others) to top-level global frame
void bind(char *name, Value *(*function)(struct Value *), Frame *frame)
{
   // Allocates memory for primitive type and configures it
   Value *value = talloc(sizeof(Value));
   value->type = PRIMITIVE_TYPE;
   // associates function to primitive
   value->pf = function;
   // Creates a value to store the symbol associated with the primitive and configures it
   Value *namePrimitive = talloc(sizeof(Value));
   namePrimitive->type = SYMBOL_TYPE;
   namePrimitive->s = name;
   // Adds the pair (name, primitive) to the global frame
   value = cons(value, makeNull());
   namePrimitive = cons(namePrimitive, value);
   addBindings(evaluateBindings(namePrimitive, frame), frame);
}
// Primitive plus function. Adds two or more numbers together.
// If no parameters are passed, returns 0
Value *plus(Value *args)
{
   // Iniitalizes result of sum to 0
   Value *curr = args;
   Value *total = talloc(sizeof(Value));
   total->type = INT_TYPE;
   total->i = 0;
   // The number is by default an integer
   int isDouble = 0;
   // Iterates through all the values to sum
   while (!isNull(curr))
   {
      // If the value is an integer, just adds it up to the total
      if (car(curr)->type == INT_TYPE)
      {
         // checks to see if we have an integer or a double total
         if (!isDouble)
         {
            total->i = total->i + car(curr)->i;
         }
         else
         {
            total->d = total->d + car(curr)->i;
         }
      }
      // If it is a double, and the total is still a integer
      else if (car(curr)->type == DOUBLE_TYPE && !isDouble)
      {
         // converts the total to an integer and add the next double value to it
         double d = total->i;
         total->type = DOUBLE_TYPE;
         total->d = d + car(curr)->d;
         isDouble = 1;
      }
      // If the total is a double and we want to add a double, just add it to the total
      else if (car(curr)->type == DOUBLE_TYPE)
      {
         total->d = total->d + car(curr)->d;
      }
      // Handles errors
      else
      {
         printf("Evaluation error: cannot add a non numeric value :V!\n");
         texit(0);
      }
      curr = cdr(curr);
   }
   return total;
}
// Primitive scheme function to check if a value is null.
// If it is returns boolean #t otherwise returns boolean #f
Value *nullScheme(Value *val)
{
   // Allocates memory for result to return
   Value *boolean = talloc(sizeof(Value));
   boolean->type = BOOL_TYPE;
   // Checks if the correct number of parameters were passed
   if (length(val) == 1)
   {
      // Cheks if the parameter is null and sets the variable to be returned
      val = car(val);
      if (val->type == NULL_TYPE || (val->type == CONS_TYPE && car(val)->type == NULL_TYPE))
      {
         boolean->i = 1;
      }
      else
      {
         boolean->i = 0;
      }
   }
   // If too many parameters were passed returns an error
   else if (length(val) > 1)
   {
      printf("Evaluation error: too many arguments for null?\n");
      texit(0);
   }
   // If too few parameters were passed returns an error
   else
   {
      printf("Evaluation error: too few arguments for null?\n");
      texit(0);
   }
   return boolean;
}
// Primitive car function for scheme.
// Returns the car of the linked list
Value *carScheme(Value *val)
{
   // Checks if the correct number of parameters were passed
   if (length(val) == 1)
   {
      val = car(val);
      // Checks if the argument is a cons cell
      if (val->type == CONS_TYPE)
      {
         // returns the car of the list
         return car(val);
      }
      // If the argument has a wrong type returns an error
      else
      {
         printf("Evaluation error: the type of the argument for car is not a cons cell\n");
         texit(0);
      }
   }
   // If too many parameters were passed returns an error
   else if (length(val) > 1)
   {
      printf("Evaluation error: too many arguments for car\n");
      texit(0);
   }
   // If too few parameters were passed returns an error
   else
   {
      printf("Evaluation error: too few arguments for car\n");
      texit(0);
   }
   return makeNull();
}
// Primitive cdr function for scheme.
// Returns the cdr of the linked list
Value *cdrScheme(Value *val)
{
   // Checks if the correct number of parameters were passed
   if (length(val) == 1)
   {
      val = car(val);
      // Checks if the argument is a cons cell
      if (val->type == CONS_TYPE)
      {
         // returns the cdr of the list
         return cdr(val);
      }
      // If the argument has a wrong type returns an error
      else
      {
         printf("Evaluation error: the type of the argument for cdr is not a cons cell\n");
         texit(0);
      }
   }
   // If too many parameters were passed returns an error
   else if (length(val) > 1)
   {
      printf("Evaluation error: too many arguments for cdr\n");
      texit(0);
   }
   // If too few parameters were passed returns an error
   else
   {
      printf("Evaluation error: too few arguments for cdr\n");
      texit(0);
   }
   return makeNull();
}
// Primitive function for cons in scheme
// Creates a cons cell, which has a car pointing to the first argument
// Has a cdr pointing to the second argument
Value *consScheme(Value *args)
{
   // Checks if the correct number of parameters were passed
   if (length(args) == 2)
   {
      // returns the cons of the two arguments
      return cons(car(args), car(cdr(args)));
   }
   // If too many parameters were passed returns an error
   else if (length(args) > 2)
   {
      printf("Evaluation error: too many arguments for cons\n");
      texit(0);
   }
   // If too few parameters were passed returns an error
   else
   {
      printf("Evaluation error: too few arguments for cons\n");
      texit(0);
   }
   return makeNull();
}

// constructs the var-val pairs from two linked lists
Value *constructArgs(Value *vars, Value *vals)
{
   Value *bindings = makeNull();
   // checking if we can bind them
   if (length(vars) == length(vals))
   {
      // Effectively binds them
      while (!isNull(vars))
      {
         Value *pair = cons(car(vars), car(vals));
         bindings = cons(pair, bindings);
         vars = cdr(vars);
         vals = cdr(vals);
      }
   }
   // Returns an error if there is no match between the number of variables and values
   else
   {
      printf("Evaluation error: wrong number of arguments in argument construction!\n");
      texit(0);
   }
   return bindings;
}

Value *apply(Value *function, Value *args)
{
   // create a subframe for this instance
   Frame *subFrame = talloc(sizeof(Frame));
   subFrame->parent = function->cl.frame;
   // add the bindings
   subFrame->bindings = constructArgs(function->cl.paramNames, args);
   return eval(function->cl.functionCode, subFrame);
}

// Evaluates the tokens of a given S-Expression
Value *eval(Value *tree, Frame *frame)
{
   // switch clause for every type of token
   switch (tree->type)
   {
   // If it is an integer, returns an integer
   case INT_TYPE:
      return tree;

   // If it is a boolean, returns a boolean
   case BOOL_TYPE:
      return tree;

   // If it is a double, returns a double
   case DOUBLE_TYPE:
      return tree;

   // If it is a string, returns a string
   case STR_TYPE:
      return tree;
   // If it is null, returns null
   case NULL_TYPE:
      return tree;
   // Returns and error if you try to access an UNSPECIFIED_TYPE
   case UNSPECIFIED_TYPE:
   {
      printf("Evaluation error: cannot use type unspecified\n");
      texit(0);
   }

   // If it is a symbol look up the symbol in the stack frame,
   // going to parent frames if needed. Returns the value of the symbol.
   case SYMBOL_TYPE:
      return lookUpSymbol(tree, frame);

   // if it is a closure, returns a corresponding closure
   case CLOSURE_TYPE:
   {
      return tree;
   }
      // if it is a primitive, returns the corresponding primitive
   case PRIMITIVE_TYPE:
   {
      return tree;
   }

   // else if goes to a new level
   case CONS_TYPE:
   {
      Value *first = car(tree);
      Value *args = cdr(tree);
      // if it is an "if" statement
      if (!strcmp(first->s, "if"))
      {
         // We check whether it has the correct number of arguments
         // If so, we proceed to evaluate the if statement,
         // otherwise it prints an error message.
         if (length(args) == 3)
         {
            // if it is true
            if (eval(car(args), frame)->i)
            {
               // It evaluates the true statement
               return eval(car(cdr(args)), frame);
            }
            // else if it is false
            else
            {
               // It evaluates the false statement
               return eval(car(cdr(cdr(args))), frame);
            }
         }
         // Error for when the code if statement does not have the correct number of arguments
         else
         {
            printf("Evaluation error: wrong number of arguments in an if clause!\n");
            texit(0);
         }
      }

      else if (!strcmp(first->s, "lambda"))
      {
         // We check whether it has the correct number of arguments
         // If so, we proceed to create the function's closure,
         // otherwise it prints an error message.
         if (length(args) == 2 && validLambda(car(args)))
         {
            Value *newLambda = talloc(sizeof(Value));
            newLambda->type = CLOSURE_TYPE;
            newLambda->cl.frame = frame;
            newLambda->cl.paramNames = car(args);
            newLambda->cl.functionCode = car(cdr(args));
            return newLambda;
         }
         // Error for when the code if statement does not have the correct number of arguments
         else
         {
            printf("Evaluation error: wrong number of arguments in a lambda clause!\n");
            texit(0);
         }
      }
      // If it is an and
      else if (!strcmp(first->s, "and"))
      {
         // Checks to see if the right number of arguments were passed
         if (length(args) >= 1)
         {
            // Initializes the return result with value true
            Value *boolVal = talloc(sizeof(Value));
            boolVal->type = BOOL_TYPE;
            boolVal->i = 1;
            Value *curr = args;
            // Ands all arguments together
            while (!isNull(curr))
            {
               boolVal->i = boolVal->i && eval(car(curr), frame)->i;
               curr = cdr(curr);
            }
            // returns the result
            return boolVal;
         }
         // If and does not have enough arguments, it returns an error message and quits
         else
         {
            printf("Evaluation error: wrong number of arguments in an AND clause!\n");
            texit(0);
         }
      }
      // If it is an or
      else if (!strcmp(first->s, "or"))
      {
         // Checks to see if the right number of arguments were passed
         if (length(args) >= 1)
         {
            // Initializes the return result with value false
            Value *boolVal = talloc(sizeof(Value));
            boolVal->type = BOOL_TYPE;
            boolVal->i = 0;
            Value *curr = args;
            // Ors all arguments together
            while (!isNull(curr))
            {
               boolVal->i = boolVal->i || eval(car(curr), frame)->i;
               curr = cdr(curr);
            }
            // returns the result
            return boolVal;
         }
         // If or does not have enough arguments, it returns an error message and quits
         else
         {
            printf("Evaluation error: wrong number of arguments in an OR clause!\n");
            texit(0);
         }
      }
      // If the command is begin
      else if (!strcmp(first->s, "begin"))
      {
         // If no arguments were passed, returns void
         if (length(args) < 1)
         {
            Value *voidVal = talloc(sizeof(Value));
            voidVal->type = VOID_TYPE;
            return voidVal;
         }
         // Otherwise, evaluates each argument passed
         Value *currEval = args;
         while (!isNull(cdr(currEval)))
         {
            eval(car(currEval), frame);
            currEval = cdr(currEval);
         }
         return eval(car(currEval), frame);
      }
      // If it is a set!
      else if (!strcmp(first->s, "set!"))
      {
         Value *var = car(args);
         Value *assign = eval(car(cdr(args)), frame);
         // Uses helper function to set the value of vars to assign
         return updateVals(var, assign, frame);
      }
      // If it is a cond
      else if (!strcmp(first->s, "cond"))
      {
         // If no arguments were passed, returns void
         if (length(args) < 1)
         {
            Value *voidVal = talloc(sizeof(Value));
            voidVal->type = VOID_TYPE;
            return voidVal;
         }
         Value *curr = args;
         // Checks if the some statement (except for the last in the list) is true
         // if so evaluates the clause and returns its result
         while (!isNull(cdr(curr)))
         {
            if (eval(car(car(curr)), frame)->i)
            {
               return eval(car(cdr(car(curr))), frame);
            }
            curr = cdr(curr);
         }
         // Evaluates edge case if no statements were true and the last statement is an else
         if (car(car(curr))->type == SYMBOL_TYPE && !strcmp(car(car(curr))->s, "else"))
         {
            return eval(car(cdr(car(curr))), frame);
         }
         // If the last statement is not an else evaluates it, returning it only if it is true
         else
         {
            if (eval(car(car(curr)), frame)->i)
            {
               return eval(car(cdr(car(curr))), frame);
            }
         }
      }
      // If the command is a let*
      else if (!strcmp(first->s, "let*"))
      {
         // Checks if the correct number of arguments were passed
         if (length(args) >= 2)
         {
            // Calls helper function to add each local variable to their own frame
            Value *curr = car(args);
            Frame *letf = letAux(curr, frame);
            Value *currEval = cdr(args);
            // Executes every command after variable declaration
            while (!isNull(cdr(currEval)))
            {
               eval(car(currEval), letf);
               currEval = cdr(currEval);
            }
            return eval(car(currEval), letf);
         }
         // If the wrong number of arguments were passed, returns an error
         else
         {
            printf("Evaluation error: wrong number of arguments in let*, so error :/!\n");
            texit(0);
         }
      }

      else if (!strcmp(first->s, "letrec"))
      {
         // We check if letrec has enough arguments.
         // Letrec must have a list of local variables, and a body.
         if (length(args) >= 2)
         {
            // Checks if the list of arguments is porperly formatted.
            // If it isnt then we print an error message and exit
            if (car(args)->type == CONS_TYPE)
            {
               // Creating a new frame, and initializing it
               Frame *subFrame = talloc(sizeof(Frame));
               subFrame->bindings = makeNull();
               subFrame->parent = frame;
               // Call a helper function to bind variables in letrec
               bindLetRec(car(args), subFrame);
               // Let only returns the last tokenized lexime in its body
               Value *evaluateLet = cdr(args);
               // While it is not the last tokenized lexime, evaluate, but do not return it
               while (!isNull(cdr(evaluateLet)))
               {
                  eval(car(evaluateLet), subFrame);
                  // we get the next lexime
                  evaluateLet = cdr(evaluateLet);
               }
               // evalueates and returns the expression
               return eval(car(evaluateLet), subFrame);
            }
            // If the list of arguments is not properly formatted, we quit the program
            else
            {
               printf("Evaluation error: the first argument of the let statement is not a list of variables!\n");
               texit(0);
            }
         }
         // If let does not have enough arguments, it returns an error message and quits
         else
         {
            printf("Evaluation error: wrong number of arguments in a let clause!\n");
            texit(0);
         }
      }
      // checking for the let statement
      else if (!strcmp(first->s, "let"))
      {
         // We check if let has enough arguments.
         // Let must have at least a list of variables, a body.
         // Only the last element in the body is returned.
         // All are evaluated.
         // If it does not have enough arguments, an error message is outputted and the program quits.
         if (length(args) >= 2)
         {
            // Checks if the list of arguments is properly formatted.
            // If it isn't then we print an error message and exit.
            if (car(args)->type == CONS_TYPE || isNull(car(args)))
            {
               // Creating a new frame, and initializing it
               Frame *subFrame = talloc(sizeof(Frame));
               subFrame->bindings = makeNull();
               subFrame->parent = frame;
               Value *curr = car(args);
               while (!isNull(curr))
               {
                  addBindings(evaluateBindings(car(curr), frame), subFrame);
                  curr = cdr(curr);
               }
               // Evaluates let.
               // reaches for the very last expression and evaluates it
               Value *currEval = cdr(args);
               while (!isNull(cdr(currEval)))
               {
                  eval(car(currEval), subFrame);
                  currEval = cdr(currEval);
               }
               return eval(car(currEval), subFrame);
            }
            // If the list of arguments is not properly formatted, we quit the program
            else
            {
               printf("Evaluation error: the first argument of the let statement is not a list of variables!\n");
               texit(0);
            }
         }
         // If let does not have enough arguments, it returns an error message and quits
         else
         {
            printf("Evaluation error: wrong number of arguments in a let clause!\n");
            texit(0);
         }
      }
      else if (!strcmp(first->s, "define"))
      {
         if (length(args) == 2 && car(args)->type == SYMBOL_TYPE)
         {
            Value *var = makeNull();
            if (alreadyDeclared(car(args), frame->bindings))
            {
               // And then add the variable to the stack frame
               addBindings(evaluateBindings(args, frame), frame);
               Value *voidType = talloc(sizeof(Value));
               voidType->type = VOID_TYPE;
               return voidType;
            }
            // If the variable has been declared more than once in the frame, the code shows and error message
            else
            {
               printf("Evaluation Error: Cannot declare a variable twice!\n");
               texit(0);
            }
         }
         else
         {
            printf("Evaluation error: wrong number of arguments in define!\n");
            texit(0);
         }
      }
      // else if we encounter a quote
      else if (!strcmp(first->s, "quote"))
      {
         if (length(args) == 1)
         {
            if (isNull(car(args)))
            {
               return args;
            }
            return car(args);
         }
         else
         {
            printf("Evaluation Error: too many arguments to quote!\n");
            texit(0);
         }
      }
      // else it is not a recognized special form
      else
      {
         // Evaluates the first element from the cons cell and the cdr
         Value *evaledOperator = eval(first, frame);
         Value *evaledArgs = evalEach(args, frame);
         // If the first element is a function, handles with it
         if (evaledOperator->type == CLOSURE_TYPE)
         {
            return apply(evaledOperator, evaledArgs);
         }
         // If the first element is a primitive, handles with it
         else if (evaledOperator->type == PRIMITIVE_TYPE)
         {
            return evaledOperator->pf(evaledArgs);
         }
         // If it is not those special forms, returns an error
         else
         {
            printf("Evaluation error: unknown special form\n");
            texit(0);
         }
      }
      break;
   }
   // else exit the function
   default:
   {
      printf("Evaluation error!\n");
      return makeNull();
      texit(0);
   }
   }
   // default return -- null element
   return makeNull();
}

// Looks up a symbol in the frame
Value *lookUpSymbol(Value *tree, Frame *frame)
{
   Value *curr = frame->bindings;
   // Iterates through every variable in the stack frame looking for the correct name
   // If it is found, returns whatever is stored in the variable.
   while (!isNull(curr))
   {
      if (!strcmp(tree->s, car(car(curr))->s))
      {
         return cdr(car(curr));
      }
      curr = cdr(curr);
   }
   // Otherwise looks for it in the parent frame
   if (frame->parent != NULL)
   {
      return lookUpSymbol(tree, frame->parent);
   }
   else
   {
      // If it is not in any frame, prints an error message and stops the program
      printf("Evaluation error: unbound variable!\n");
      texit(0);
      return makeNull();
   }
}

// Checks if a variable with that name has already been declared in the current frame
int alreadyDeclared(Value *tree, Value *bindings)
{
   Value *curr = bindings;
   // If it has already been declared, returns 0 to indicate the program should not declare it twice
   while (!isNull(curr))
   {
      if (!strcmp(tree->s, car(car(curr))->s))
      {
         return 0;
      }
      curr = cdr(curr);
   }
   // If it is a new variable, returns 1 to indicate it should be declared
   return 1;
}

// checks if these parameters can be passed in lambda
bool validLambda(Value *args)
{
   // if no args are passed then it is valid
   if (isNull(args))
   {
      return 1;
   }
   // checks if all of the args passed are indeed symbols
   Value *curr = args;
   while (!isNull(curr))
   {
      // if it is not a symbol, then it is not valid
      if (car(curr)->type != SYMBOL_TYPE)
      {
         return 0;
      }
      curr = cdr(curr);
   }
   // checks if no symbol repeats
   curr = args;
   while (!isNull(curr))
   {
      Value *compared = cdr(curr);
      while (!isNull(compared))
      {
         if (!strcmp(car(curr)->s, car(compared)->s))
         {
            return 0;
         }
         compared = cdr(compared);
      }
      curr = cdr(curr);
   }
   // if passed all the checks, then it is valid
   return 1;
}

// evaluates bindings in the current frame
Value *evaluateBindings(Value *args, Frame *frame)
{
   Value *val = talloc(sizeof(Value));
   val->type = CONS_TYPE;
   // Checks if the variable declaration is in the correct format, that is, (varName(symbol) value)
   if (args->type == CONS_TYPE && length(args) == 2 && car(args)->type == SYMBOL_TYPE)
   {
      // And then add the variable to the stack frame
      val->c.car = car(args);
      val->c.cdr = eval(car(cdr(args)), frame);
      return val;
   }
   // If it is not in the correct form, then it is an error
   else
   {
      printf("Evaluation Error: wrong format for variable declaration!\n");
      texit(0);
   }
   return makeNull();
}

// adds bindings to the current frame
void addBindings(Value *newBindings, Frame *frame)
{
   // Checks if the variable has not been declared in the same frame
   if (alreadyDeclared(car(newBindings), frame->bindings))
   {
      frame->bindings = cons(newBindings, frame->bindings);
   }
   // If the variable has been declared more than once in the frame, the code shows and error message
   else
   {
      printf("Evaluation Error: Cannot declare a variable twice!\n");
      texit(0);
   }
}
// Prints a value of the valid types.
// Given a value, this function prints it to the screen
void printByType(Value *val)
{
   if (val->type == INT_TYPE)
   {
      printf("%i", val->i);
   }
   else if (val->type == DOUBLE_TYPE)
   {
      printf("%lf", val->d);
   }
   else if (val->type == STR_TYPE)
   {
      printf("%s", val->s);
   }
   else if (val->type == SYMBOL_TYPE)
   {
      printf("%s", val->s);
   }
   else if (val->type == CONS_TYPE)
   {
      printResult(val);
   }
   else if (val->type == BOOL_TYPE)
   {
      if (val->i == 1)
      {
         printf("#t");
      }
      else
      {
         printf("#f");
      }
   }
}
// Displays the result of the S-expressions on the scrreen
void printResult(Value *curr)
{
   if (curr->type == CONS_TYPE || curr->type == CLOSURE_TYPE)
   {
      int isCons = 0;
      if (curr->type == CLOSURE_TYPE)
      {
         printf("#<procedure>");
         return;
      }
      if (cdr(curr)->type != CONS_TYPE && cdr(curr)->type != NULL_TYPE)
      {
         printf("(");
         printByType(car(curr));
         printf(" . ");
         printByType(cdr(curr));
         printf(")");
         return;
      }
      if (curr->type == CONS_TYPE)
      {
         printf("(");
         isCons = 1;
      }
      while (curr->type == CONS_TYPE && !isNull(curr))
      {
         printResult(car(curr));
         curr = cdr(curr);
         if (curr->type != CONS_TYPE && curr->type != NULL_TYPE)
         {
            printf(" . ");
            printByType(curr);
            break;
         }
         if (!isNull(curr))
         {
            printf("\t");
         }
      }
      if (isCons)
      {
         printf(")");
      }
   }
   else
   {
      printByType(curr);
   }
}

// runs the eval function for each s-expression
void interpret(Value *tree)
{
   // creates a global frame
   Frame *frame = talloc(sizeof(Frame));
   frame->bindings = makeNull();
   frame->parent = NULL;
   // Binds the primitives
   bind("+", plus, frame);
   bind("null?", nullScheme, frame);
   bind("car", carScheme, frame);
   bind("cdr", cdrScheme, frame);
   bind("cons", consScheme, frame);
   bind("-", minus, frame);
   bind(">", greater, frame);
   bind("<", less, frame);
   bind("=", equals, frame);
   bind("*", times, frame);
   bind("modulo", mod, frame);
   bind("/", divide, frame);
   // Evaluates each S expression
   while (!isNull(tree))
   {
      Value *ret = eval(car(tree), frame);
      printResult(ret);
      printf("\n");
      tree = cdr(tree);
   }
}
