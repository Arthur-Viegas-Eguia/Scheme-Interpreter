//Changes the value of variable var to assign
Value *updateVals(Value *var, Value *assign, Frame *frame)
{
    Value *curr = frame->bindings;
    //Iterates through all the variables in the frame to find var
    while (curr->type != NULL_TYPE)
    {
        //If it is var
        if (!strcmp(var->s, car(car(curr))->s))
        {
            //Sets its value to assign and return void
            curr->c.car->c.cdr = assign;
            Value *nodeVoid = talloc(sizeof(Value));
            nodeVoid->type = VOID_TYPE;
            return nodeVoid;
        }
        curr = cdr(curr);
    }
    //If the variable wasnt found in the current frame, looks for it in the parent frame
    if (frame->parent != NULL)
    {
        updateVals(var, assign, frame->parent);
    }
    //Error if the variable is undefined
    else
    {
        printf("Evaluation error: Cannot apply set! on a variable which was not yet defined\n");
        texit(0);
    }
    return makeNull();
}
//Primitive minus function for scheme
Value *minus(Value *args)
{
    Value *sub = talloc(sizeof(Value));
    //Checks if the length is correct
    if (length(args) == 2)
    {
        //Checks for the correct type of the two arguments and subtracts the second one from the first
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
    //If the wrong number of elements were passed, prints an error
    else
    {
        printf("Evaluation error: wrong number of arguments for minus\n");
        texit(0);
    }
    //Returns the result of the subtraction
    return sub;
}
//Primitive function for scheme division
Value *divide(Value *args)
{
    Value *div = talloc(sizeof(Value));
    //Checks if the length is correct
    if (length(args) == 2)
    {
        //Checks for the type of the two arguments
        //Once the type is found, sees if the division returns an integer value
        //If so divides the numbers and saves their result as an integer
        //If their division does not return an integer, save the result of the
        //Division in a float
        if (car(args)->type != INT_TYPE && car(cdr(args))->type == INT_TYPE)
        {
            if(fmod(car(args)->d, car(cdr(args))->i) == 0){
                div->type = INT_TYPE;
                div->i = car(args)->d / car(cdr(args))->i;
            }
            else{
            div->type = DOUBLE_TYPE;
            div->d = car(args)->d / car(cdr(args))->i;
            }
        }
        else if (car(args)->type == INT_TYPE && car(cdr(args))->type != INT_TYPE)
        {
            if(fmod(car(args)->i, car(cdr(args))->d) == 0){
                div->type = INT_TYPE;
                div->i = car(args)->i / car(cdr(args))->d;
            }
            else{
            div->type = DOUBLE_TYPE;
            div->d = car(args)->i / car(cdr(args))->d;
            }
        }
        else if (car(args)->type != INT_TYPE && car(cdr(args))->type != INT_TYPE)
        {
            if(fmod(car(args)->d, car(cdr(args))->d) == 0){
                div->type = INT_TYPE;
                div->i = car(args)->d / car(cdr(args))->d;
            }
            else{
            div->type = DOUBLE_TYPE;
            div->d = car(args)->d / car(cdr(args))->d;
            }
        }
        else
        {
            if(fmod(car(args)->i, car(cdr(args))->i) == 0){
                div->type = INT_TYPE;
                div->i = car(args)->i / car(cdr(args))->i;
            }
            else{
                //Handles edge case of integer division in c
            double frac = car(args)->i, tion = car(cdr(args))->i;
            div->type = DOUBLE_TYPE;
            div->d = frac / tion;
            }
        }
    }
    //Handles the case if the wrong number of arguments were passed
    else
    {
        printf("Evaluation error: wrong number of arguments for minus\n");
        texit(0);
    }
    //Returns the result of the division
    return div;
}

//Primitive multiplication for scheme
Value *times(Value *args)
{
    //If no arguments are passed, returns 0
    if (isNull(args))
    {
        return 0;
    }
    //Otherwise initializes the value of the multiplication as 1
    //And of type integer
    Value *curr = args;
    Value *total = talloc(sizeof(Value));
    total->type = INT_TYPE;
    total->i = 1;
    //This indicates that the value is not a float
    int isDouble = 0;
    //Multiplies every element in the list of arguments
    while (!isNull(curr))
    {
        //If multiplying by an integer
        if (car(curr)->type == INT_TYPE)
        {
            //If the total is an integer just multiplies them together
            if (!isDouble)
            {
                total->i = total->i * car(curr)->i;
            }
            //If it is a double multiply them together
            else
            {
                total->d = total->d * car(curr)->i;
            }
        }
        //If we are multiplying a double and the total is an integer
        else if (car(curr)->type == DOUBLE_TYPE && !isDouble)
        {
            //We convert the total to double and multiply them together
            double d = total->i;
            total->type = DOUBLE_TYPE;
            total->d = d * car(curr)->d;
            isDouble = 1;
        }
        //If we are multiplying a double and the total is a double
        else if (car(curr)->type == DOUBLE_TYPE)
        {
            //Multiply them together
            total->d = total->d * car(curr)->d;
        }
        //If the value is not a number returns an error
        else
        {
            printf("Evaluation error: cannot add a non numeric value :V!\n");
            texit(0);
        }
        curr = cdr(curr);
    }
    return total;
}
//Primitive equals for scheme
//Checks if two values are equal to each other
Value *equals(Value *args)
{
    Value *boolVal = talloc(sizeof(Value));
    //Initializes the default return value to false
    boolVal->type = BOOL_TYPE;
    boolVal->i = 0;
    //Checks if the values have the same type, if so checks if they are equal. If that is the case set 
    //Return value to true.
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
    //Handles edge cases of comparing integer to double or double to integer
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
//Primitive module function between two integers
//Returns the mod of the two arguments passed
Value *mod(Value *args)
{
    Value *mod = talloc(sizeof(Value));
    mod->type = INT_TYPE;
    mod->i = car(args)->i % car(cdr(args))->i;
    return mod;
}
//Primitive greater than function for scheme
//Checks if the first element is greater than the second element
Value *greater(Value *args)
{
    //sets default value to false
    Value *boolVal = talloc(sizeof(Value));
    boolVal->type = BOOL_TYPE;
    boolVal->i = 0;
    //Checks and finds the correct argument types. Once the code knows the type 
    //Of the arguments, it sets the return result to true if the first argument is greater
    //Than the second argument
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
//Primitive less than function for scheme
//Checks if the first element is less than the second element
Value *less(Value *args)
{
    //sets default value to false
    Value *boolVal = talloc(sizeof(Value));
    boolVal->type = BOOL_TYPE;
    boolVal->i = 0;
    //Checks and finds the correct argument types. Once the code knows the type 
    //Of the arguments, it sets the return result to true if the first argument is less
    //Than the second argument
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
//Helper function to let* creates a new frame for each local variable
//Returns the frame of the last local variable
Frame *letAux(Value *curr, Frame *fr)
{
    //Creates a new frame and a cons cell for the variable.
    //The car of the cons cell is the name of the variable
    //The cdr of the cons cell is the evaluated value of the variable
    Value *val = talloc(sizeof(Value));
    val->type = CONS_TYPE;
    val->c.car = car(car(curr));
    val->c.cdr = eval(car(cdr(car(curr))), fr);
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = fr;
    newFrame->bindings = makeNull();
    //Binds cons cell to frame
    newFrame->bindings = cons(val, newFrame->bindings);
    curr = cdr(curr);
    //If there are more variables in the list, calls fucntion recursively
    if (curr->type != NULL_TYPE)
    {
        return letAux(curr, newFrame);
    }
    //Base case, returns latest frame
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
//Binds arguments for letrec
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
                //If so, creates an undefined value, sets variable to (varName(symbol) unspecified)
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
    //Evaluates all the values of each variables and saves them
    while (!isNull(curr2))
    {
        Value *var = talloc(sizeof(Value));
        var->type = CONS_TYPE;
        var->c.car = car(car(curr2));
        //If the variable evaluates to unspecified type returns an error message
        Value *check = eval(car(cdr(car(curr2))), frame);
        if(check->type == UNSPECIFIED_TYPE){
            printf("Evaluation error: cannot use type unspecified\n");
            texit(0);
        }
        //Otherwise save the evaluated result in the list
        var->c.cdr = eval(car(cdr(car(curr2))), frame);
        semiEvaluated = cons(var, semiEvaluated);
        curr2 = cdr(curr2);
    }
    Value *curr3 = semiEvaluated;
    //Binds each value to the appropriate variable
    while(!isNull(curr3)){
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