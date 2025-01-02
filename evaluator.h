#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "parser.h"

/**
 * @brief Evaluates the value of a mathematical expression represented by an abstract syntax tree.
 *
 * This function recursively traverses the abstract syntax tree (AST) to evaluate the value of a
 * mathematical expression at a given `x_value`. It supports various types of nodes, including numbers,
 * variables (represented by 'x'), mathematical functions (like sin, cos, etc.), and operators (like +, -, *, /).
 * The function also handles unary and binary operators, including power and logarithmic functions.
 *
 * @param node    Pointer to the root node of the abstract syntax tree (AST) representing the function.
 * @param x_value The value of `x` to evaluate the function at.
 * @return The evaluated value of the function at the given `x_value`.
 *
 * @note The function supports trigonometric, logarithmic, and other common mathematical functions.
 *       It throws an error and exits if an unknown function or operator is encountered.
 */
double evaluate(const Node *node, double x_value);

#endif // EVALUATOR_H
