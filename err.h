#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Error message for an unknown function in the expression.
 *
 * This error message is used when an unrecognized function name is encountered in the mathematical expression.
 */
#define ERROR_UNKNOWN_FUNCTION_TEXT "problem with expression: unknown function"

/**
 * @brief Error message for an unknown operator in the expression.
 *
 * This error message is triggered when an unrecognized binary operator is found in the mathematical expression.
 */
#define ERROR_UNKNOWN_OPERATOR_TEXT "problem with expression: unknown binary operator"

/**
 * @brief Error message for an unknown node operator in the abstract syntax tree.
 *
 * This error message is used when an invalid or unexpected node operator is encountered during the expression parsing.
 */
#define ERROR_UNKNOWN_NODE_TEXT "problem with expression: unknown node operator"

/**
 * @brief Error message for incorrect bracket usage in the expression.
 *
 * This error occurs when there is an imbalance or incorrect positioning of brackets in the expression.
 * The user should ensure that brackets are balanced and used correctly.
 */
#define ERROR_BRACKETS_TEXT "problem with expression: wrong usage of brackets! Ensure that brackets are balanced and used in right positions"

/**
 * @brief Error message for being unable to open the output file.
 *
 * This error is triggered when the program is unable to open the specified output file for writing the graph.
 */
#define ERROR_FILE_TEXT "unable to open output file"

/**
 * @brief Error message for an invalid limits string.
 *
 * This message appears if the format of the limits string is incorrect. The user should ensure proper syntax:
 * ⟨xmin⟩:⟨xmax⟩:⟨ymin⟩:⟨ymax⟩, with xmin < xmax and ymin < ymax.
 */
#define ERROR_LIMITS_TEXT "while parsing limits string.\nCorrect usage: ⟨xmin⟩:⟨xmax⟩:⟨ymin⟩:⟨ymax⟩\nEnsure that xmin < xmax and ymin < ymax"

/**
 * @brief Error message for invalid arguments in the program.
 *
 * This message is displayed if the input arguments are invalid. The user should ensure the correct usage:
 * <func> <out-file> [<limits>], where <func> is a valid mathematical function enclosed in quotes if necessary.
 */
#define ERROR_ARGS_TEXT "invalid input. Correct usage: <func> <out-file> [<limits>].\nEnsure the function is a single-variable mathematical expression and enclosed in quotes if it contains spaces.\n"

/**
 * @brief Error code for invalid arguments.
 *
 * This code represents an error related to invalid arguments passed to the program.
 */
#define ERROR_ARGS 1

/**
 * @brief Error code for an unknown function error.
 *
 * This code represents an error related to the presence of an unknown function in the mathematical expression.
 */
#define ERROR_FUNCTION 2

/**
 * @brief Error code for file opening issues.
 *
 * This error code is returned when there is a problem with opening the output file.
 */
#define ERROR_FILE 3

/**
 * @brief Error code for limits parsing issues.
 *
 * This error code is used when there is an issue parsing the limits string provided by the user.
 */
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
