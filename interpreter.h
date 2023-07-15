#ifndef _INTERPRETER
#define _INTERPRETER

void interpret(Value *tree);
Value *eval(Value *tree, Frame *frame);
Value *evaluateBindings(Value *args, Frame *frame);
Value *lookUpSymbol(Value *tree, Frame *frame);
void addBindings(Value *newBindings, Frame *frame);
int alreadyDeclared(Value *tree, Value *bindings);
bool validLambda(Value *args);
void printResult(Value *curr);
void printByType(Value *val);
#endif
