//If it is an and
else if (!strcmp(first->s, "and"))
{
    //Checks to see if the right number of arguments were passed
    if (length(args) >= 1)
    {
        //Initializes the return result with value true
        Value *boolVal = talloc(sizeof(Value));
        boolVal->type = BOOL_TYPE;
        boolVal->i = 1;
        Value *curr = args;
        //Ands all arguments together
        while (!isNull(curr))
        {
            boolVal->i = boolVal->i && eval(car(curr), frame)->i;
            curr = cdr(curr);
        }
        //returns the result
        return boolVal;
    }
    // If and does not have enough arguments, it returns an error message and quits
    else
    {
        printf("Evaluation error: wrong number of arguments in an AND clause!\n");
        texit(0);
    }
}
//If it is an or
else if (!strcmp(first->s, "or"))
{
    //Checks to see if the right number of arguments were passed
    if (length(args) >= 1)
    {
        //Initializes the return result with value false
        Value *boolVal = talloc(sizeof(Value));
        boolVal->type = BOOL_TYPE;
        boolVal->i = 0;
        Value *curr = args;
        //Ors all arguments together
        while (!isNull(curr))
        {
            boolVal->i = boolVal->i || eval(car(curr), frame)->i;
            curr = cdr(curr);
        }
        //returns the result
        return boolVal;
    }
    // If or does not have enough arguments, it returns an error message and quits
    else
    {
        printf("Evaluation error: wrong number of arguments in an OR clause!\n");
        texit(0);
    }
}
//If the command is begin
else if (!strcmp(first->s, "begin"))
{
    // If no arguments were passed, returns void
    if (length(args) < 1)
    {
        Value *voidVal = talloc(sizeof(Value));
        voidVal->type = VOID_TYPE;
        return voidVal;
    }
    //Otherwise, evaluates each argument passed
    Value *currEval = args;
               while (!isNull(cdr(currEval)))
               {
                  eval(car(currEval), frame);
                  currEval = cdr(currEval);
               }
               return eval(car(currEval), frame);
}
//If it is a set!
else if (!strcmp(first->s, "set!"))
{
    Value *var = car(args);
    Value *assign = eval(car(cdr(args)), frame);
    //Uses helper function to set the value of vars to assign
    return updateVals(var, assign, frame);
}
//If it is a cond
else if (!strcmp(first->s, "cond"))
{
    //If no arguments were passed, returns void
        if (length(args) < 1)
    {
        Value *voidVal = talloc(sizeof(Value));
        voidVal->type = VOID_TYPE;
        return voidVal;
    }
    Value *curr = args;
    //Checks if the some statement (except for the last in the list) is true
    //if so evaluates the clause and returns its result
    while(!isNull(cdr(curr))){
        if(eval(car(car(curr)), frame)->i){
            return eval(car(cdr(car(curr))), frame);
        }
        curr = cdr(curr);
    }
    //Evaluates edge case if no statements were true and the last statement is an else
    if(car(car(curr))->type == SYMBOL_TYPE && !strcmp(car(car(curr))->s, "else")){
        return eval(car(cdr(car(curr))), frame);
    }
    //If the last statement is not an else evaluates it, returning it only if it is true
    else{
        if(eval(car(car(curr)), frame)->i){
            return eval(car(cdr(car(curr))), frame);
        }
    }
}
//If the command is a let*
else if (!strcmp(first->s, "let*"))
{
    //Checks if the correct number of arguments were passed
    if (length(args) >= 2)
    {
        //Calls helper function to add each local variable to their own frame
        Value *curr = car(args);
        Frame *letf = letAux(curr, frame);
       Value *currEval = cdr(args);
       //Executes every command after variable declaration
               while (!isNull(cdr(currEval)))
               {
                  eval(car(currEval), letf);
                  currEval = cdr(currEval);
               }
               return eval(car(currEval), letf);
    }
    //If the wrong number of arguments were passed, returns an error
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
            //Call a helper function to bind variables in letrec
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