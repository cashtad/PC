#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "parser.h"
#include <float.h>



/**
 * @brief Evaluates the expression represented by the abstract syntax tree (AST).
 *
 * The `evaluate` function computes the value of an expression based on the AST. The expression
 * is evaluated recursively, starting from the root node. The function handles different types of
 * nodes (numeric values, variable identifiers, functions, and operators) and computes the result
 * accordingly. The variable `x_value` is used when the expression contains an identifier (`x`).
 *
 * @param node Pointer to the root of the AST or a sub-expression node.
 * @param x_value The value of the variable `x`, which is used when the expression contains an identifier.
 *
 * @return The computed result of the expression represented by the AST.
 *
 * @note The function supports various mathematical functions such as sine, cosine, tangent,
 *       logarithms, and others. It also handles binary operators (addition, subtraction,
 *       multiplication, division, and exponentiation). If the expression contains an invalid
 *       operator, function, or logarithmic domain error (e.g., log of a non-positive number),
 *       the program will terminate with an error message.
 */
double evaluate(const Node *node, double x_value);

#endif // EVALUATOR_H
