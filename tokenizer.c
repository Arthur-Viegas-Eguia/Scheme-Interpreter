#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"

//Checks if the character is in the list of allowed symbols of the grammar
int isSymbol(char charRead){
    //The list of allowed special characters
    char values[14] = {'!', '$', '%', '&', '*', '/', ':', '<', '=', '>', '?', '~', '_', '^'};
    //If the character is a letter it is an allowed symbol, so the function returns true
    if(isalpha(charRead)){
        return 1;
    }
    //Otherwise the program will check if it is a special character
    for(int i = 0; i < 14; i++){
        //If it is, the program will return true
        if(values[i] == charRead){
            return 1;
        }
    }
    //Otherwise, the program will return false.
    return 0;
}

//Parses a closing parens
Value *handleParens(Value *list){
    //Creates token and assigns it close type
    Value *closeCell = talloc(sizeof(Value));
  	closeCell->type = CLOSE_TYPE;
  	closeCell->s = talloc(sizeof(char) * 2);
  	strcpy(closeCell->s, ")");
    //Adds value to the list
    return cons(closeCell, list);
}

//Reads a string, converts it to an number, tokenizes it and adds it to a list.
//If the number is positive, sign = 1. Otherwise, sign = 0
Value *numberHelper(char charRead, int sign, Value *list){
    // allocate space for it
    char str[300];
    //If sign is positive, then it is a positive number
    if(sign == 1){
        str[0] = '+';
    }
    //Otherwise it is a negative number
    else{
        str[0] = '-'; 
    }
    // index and int counters
    int i = 1;
    int isFloat = 0;
    // while we haven't reached the end of the number/file
    do{           		
        // store it in the array
        str[i] = charRead;
        // check if it is a float
        if(charRead == '.'){
            isFloat++;
        }
        // move forward
        i++;
        // read the next character
        charRead = (char)fgetc(stdin); 
    } while (charRead != EOF && charRead != ')' && !isspace(charRead));
    // add the null-terminating character
    str[i] = '\0';
    //Creates the numerical value
    Value *numCell = talloc(sizeof(Value));
    //Creates an end string required by strtod and strtol
    char *end = talloc(sizeof(char) * 2);
    // if it is still an int
    if(isFloat == 0){
        // Assign it type INT and add value from this string
        numCell->type = INT_TYPE;
        numCell->i = strtol(str, &end, 10);
    }
    //If it is a float
    else if(isFloat == 1){
        // Assign a float type cons cell with this string's value
        numCell->type = DOUBLE_TYPE;
        numCell->d = strtod(str, &end);

    }
    // else if it neither
    else{
        //print an error message and exit the program
        printf("Syntax error: not a valid type of number\n");
        texit(0);
    }
    //Adds it to the list
    list = cons(numCell, list);
    //If the next character is a closing parens, tokenize it
    if(charRead == ')'){
        list = handleParens(list);
    }
    //Return updated list to new program
    return list;
}

Value *tokenize() {
	// initializing the values we'll use later
    char charRead;
    Value *list = makeNull();
    // reading the first character from the input
    charRead = (char)fgetc(stdin);
	
	// while we haven't reached the end of the file
    while (charRead != EOF) {
		// if it is a string
        if (charRead == '"') {
        	// allocate space for it
            char str[300];
            // index counter
            int i = 0;
			// while we haven't reached the end of the string/file
           do{
            	// store it in the array
                str[i] = charRead;
                // move the index forward
                i++;
                // read the next character
            	charRead = (char)fgetc(stdin);
            } while (charRead != EOF && charRead != '"');
            //Handles the edge case of a string which is not closed
            if(charRead != '"'){
                printf("Syntax error: a string must be terminated by quotations\n");
                texit(0);
            }
            //Code that adds the final quote to the string
            str[i] = charRead;
            i++;
            // add the null-terminating character
            str[i] = '\0';
            // create a cons cell with this string
            Value *strCell = talloc(sizeof(Value));
  			strCell->type = STR_TYPE;
  			strCell->s = talloc(sizeof(char)*(i + 1));
  			strcpy(strCell->s, str);
  			
            // add it to the list
            list = cons(strCell, list);
		}
		
		// if it is a number (int/double), both positive or negative
        else if (isdigit(charRead)) {
        	//Calls a helper function to read a positive number
            list = numberHelper(charRead, 1, list);
		}
        // else if it is a comment    
         else if (charRead == ';') {
            //skips the line
            while (charRead != EOF && charRead != '\n'){
                charRead = (char)fgetc(stdin);
            }
        }
        //else if it is an open parens
        else if (charRead == '(') {
            //Assign it type open and add value to cons cell
            Value *openCell = talloc(sizeof(Value));
  	        openCell->type = OPEN_TYPE;
  	        openCell->s = talloc(sizeof(char) * 2);
  	        strcpy(openCell->s, "(");
            //Adds value to the list
            list = cons(openCell, list);
  		} 	
  		// else if it is a closed parens, tokenizes it
        else if (charRead == ')') {
            //Calls helper function to tokenize it
            list = handleParens(list);
  		} 
  		// else if it is a boolean 
        else if (charRead == '#') {
            //Creates the Boolean type cons cell and reads its value
        	charRead = (char)fgetc(stdin);
            Value *boolCell = talloc(sizeof(Value));
  			boolCell->type = BOOL_TYPE;
        	// if it is false
        	if (charRead == 'f') {
  				boolCell->i = 0;
        	}
        	// if it is true
        	else if (charRead == 't') {
  				boolCell->i = 1;
        	}
        	// else print a syntax error
            else {
            	printf("Syntax error: invalid Boolean value passed\n");
            	texit(0);
            }
            //Adds value to list
            list = cons(boolCell, list);
        } 
        //If it is a plus sign, it might be an integer or a symbol. This tokenizes each case correctly
        else if(charRead == '+'){
            //Reads the next character
            charRead = (char)fgetc(stdin);
            //If it is a number, it calls a helper function to tokenize it as a positive number
            if(isdigit(charRead)){
                list = numberHelper(charRead, 1, list);
            } 
            //If it is a symbol, it is tokenized accordingly
            else if(isspace(charRead) || charRead == ')'){
                //Allocates space for, and makes it type symbol
                Value *sign = talloc(sizeof(Value));
                sign->type = SYMBOL_TYPE;
                sign->s = talloc(sizeof(char) * 2);
                strcpy(sign->s, "+");
                //Adds it to the list
                list = cons(sign, list);
                //If the character after the symbol is a closing parens, the code tokenizes it accordingly
                if(charRead == ')'){
                    list = handleParens(list);
                }
            } 
            //If it is not a symbol, nor a number, the program prints an error message
            else {
                printf("Syntax error: + must go alone or preced a number\n");
                texit(0);
            }
        }
        //If it is a minus sign, it might be an integer or a symbol. This tokenizes each case correctly
        else if(charRead == '-'){
            charRead = (char)fgetc(stdin);
            //If it is a number, it calls a helper function to tokenize it as a negative number
            if(isdigit(charRead)){
                list = numberHelper(charRead, 0, list);
            } 
            //If it is a symbol, it is tokenized accordingly
            else if(isspace(charRead) || charRead == ')'){
                //Allocates space for, and makes it type symbol
                Value *sign = talloc(sizeof(Value));
                sign->type = SYMBOL_TYPE;
                sign->s = talloc(sizeof(char) * 2);
                strcpy(sign->s, "-");
                //Adds it to the list
                list = cons(sign, list);
                //If the character after the symbol is a closing parens, the code tokenizes it accordingly
                if(charRead == ')'){
                    list = handleParens(list);
                }
            } 
            //If it is not a symbol, nor a number, the program prints an error message
            else{
                printf("Syntax error: - must go alone or preced a number\n");
                texit(0);
            }
        } 
        //Checks for all other symbols which do not start with + nor -
        else if(isSymbol(charRead)) {
            // allocate space for it
            char str[300];
            // index counter
            int i = 0;
			// while we haven't reached the end of the symbol/file
           do{
            	// store it in the array
                str[i] = charRead;
                // move the index forward
                i++;
                // read the next character
            	charRead = (char)fgetc(stdin);
            } while (charRead != EOF && !isspace(charRead) && charRead != ')');
            // add the null-terminating character
            str[i] = '\0';
            // create a cons cell with this symbol
            Value *symbolCell = talloc(sizeof(Value));
  			symbolCell->type = SYMBOL_TYPE;
  			symbolCell->s = talloc(sizeof(char)*(i + 1));
  			strcpy(symbolCell->s, str);
  			
            // add it to the list
            list = cons(symbolCell, list);

            //If the character after it is a closing parens, tokenizes it appropriately
            if(charRead == ')'){
                list = handleParens(list);
            }
        } 
        //If the character is not one of the mentioned above, and it is not a space, it is not tokenizeable.
        //This prompts the code to print an error message
        else if(!isspace(charRead)){
            printf("Syntax error: unknown character, so not tokenizeable\n");
            texit(0);
        }
        charRead = (char)fgetc(stdin);
    }
    Value *revList = reverse(list);
    return revList;
}

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list){
	Value *curr = list;
    while (!isNull(curr))
    {
        if(car(curr)->type == INT_TYPE){
            printf("%i:integer\n", car(curr)->i);
        }
        else if(car(curr)->type == DOUBLE_TYPE){
            printf("%lf:double\n", car(curr)->d);
        }

        else if(car(curr)->type == STR_TYPE){
            printf("%s:string\n", car(curr)->s);
        }
        else if(car(curr)->type == OPEN_TYPE){
            printf("%s:open\n", car(curr)->s);
        }
        else if(car(curr)->type == CLOSE_TYPE){
            printf("%s:close\n", car(curr)->s);
        }
        else if(car(curr)->type == BOOL_TYPE){
            if(car(curr)->i == 1){
                printf("#t:boolean\n");
            } else {
                printf("#f:boolean\n");
            }
        }
        else if(car(curr)->type == SYMBOL_TYPE){
            printf("%s:symbol\n", car(curr)->s);
        }
        curr = cdr(curr);
    }
}
