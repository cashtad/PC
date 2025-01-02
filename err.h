#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>

#define ERROR_UNKNOWN_FUNCTION_TEXT "unknown function"
#define ERROR_UNKNOWN_VARIABLE_TEXT "unknown variable"
#define ERROR_UNKNOWN_OPERATOR_TEXT "unknown operator"
#define ERROR_INVALID_ARGUMENT_TEXT "invalid argument"
#define ERROR_UNKNOWN_OPERATOR_TEXT "unknown binary operator"
#define ERROR_UNKNOWN_NODE_TEXT "unknown node operator"
#define ERROR_INVALID_EXPRESSION_TEXT "invalid expression"
#define ERROR_FUNCTION_LEFT_PAREN_TEXT "expected '(' after function definition"
#define ERROR_FUNCTION_RIGHT_PAREN_TEXT "expected ')' after function argument"
#define ERROR_UNEXPECTED_TOKEN_TEXT "unexpected token"
#define ERROR_BRACKETS_TEXT "wrong usage of brackets! Ensure that brackets are balanced and used in right positions"
#define ERROR_NUMBER_TEXT "wrong number input"
#define ERROR_EXPONENT_TEXT "wrong exponent input"
#define ERROR_IDENTIFIER_TEXT "unknown identifier"
#define ERROR_UNKNOWN_CHARACTER "unknown character"

#define ERROR_FILE_TEXT "unable to open output file"
#define ERROR_LIMITS_TEXT "while parsing limits string.\nCorrect usage: ⟨xmin⟩:⟨xmax⟩:⟨ymin⟩:⟨ymax⟩\nEnsure that xmin < xmax and ymin < ymax"
#define ERROR_ARGS_TEXT "invalid input. Correct usage: <func> <out-file> [<limits>].\nEnsure the function is a single-variable mathematical expression and enclosed in quotes if it contains spaces.\n"


#define ERROR_ARGS 1
#define ERROR_FUNCTION 2
#define ERROR_FILE 3
#define ERROR_LIMITS 4


/**
 * @brief Prints an error message and exits the program with the specified exit code.
 *
 * This function is used to display an error message to the standard error stream and then terminate the program with the provided exit code.
 * It is typically used when an irrecoverable error occurs, and the program needs to exit immediately.
 *
 * @param message The error message to be printed.
 * @param exit_code The exit code that the program will return upon termination.
 *
 * @note The program will terminate after this function is called, so no code after the call will be executed.
 */
void error_exit(const char *message, int exit_code);


#endif //UTILITIES_H
