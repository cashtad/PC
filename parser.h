#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

/**
 * @brief Defines a structure for nodes in the Abstract Syntax Tree (AST).
 *
 * This structure represents a node in the abstract syntax tree for a mathematical expression.
 * Each node can be one of several types: a number (NODE_NUM), a variable (NODE_ID), a function (NODE_FUNC),
 * or an operator (NODE_OP). Additionally, there is a placeholder for error nodes (NODE_ERROR).
 * The `union` allows different types of data to be stored in the same memory location depending on the node's type.
 */
typedef struct Node {
    // The type of the node (number, variable, function, operator, or error)
    enum type {
        NODE_NUM, /**< Numeric constant */
        NODE_ID, /**< Variable identifier ("x") */
        NODE_FUNC, /**< Mathematical function (e.g., sin, cos) */
        NODE_OP, /**< Operator (e.g., +, -, *, /) */
        NODE_ERROR /**< Error node */
    } type;

    // A union that holds different data depending on the node type
    union {
        double num; /**< For nodes of type NODE_NUM (number) */
        char id[MAX_IDENTIFIER_LENGTH]; /**< For nodes of type NODE_ID (identifier, e.g., "x") */

        // For function nodes (NODE_FUNC), stores the function name and the argument node
        struct {
            char func[MAX_IDENTIFIER_LENGTH]; /**< Function name (e.g., "sin", "cos") */
            struct Node *arg; /**< Pointer to the argument of the function */
        } func;

        // For operator nodes (NODE_OP), stores the operator and the left and right operand nodes
        struct {
            char op; /**< Operator (e.g., '+', '-', '*', '/') */
            struct Node *left; /**< Pointer to the left operand */
            struct Node *right; /**< Pointer to the right operand */
        } op;
    };
} Node;

/**
 * @brief Parses a mathematical expression into an Abstract Syntax Tree (AST).
 *
 * This function recursively parses an expression consisting of factors, operators,
 * and handles operator precedence. The parsing process starts with parsing the first factor
 * (a number, variable, or expression), then iterates over the following tokens to build the
 * tree for operators (addition, subtraction, multiplication, division, power).
 *
 * The function handles the following operators with left-to-right precedence:
 * - Addition (+)
 * - Subtraction (-)
 * - Multiplication (*)
 * - Division (/)
 * - Power (^)
 *
 * @param lexer A pointer to the lexer used for tokenizing the input expression.
 * @return A pointer to the root of the Abstract Syntax Tree (AST) for the expression.
 */
Node *parse_expr(Lexer *lexer);

/**
 * @brief Parses a factor in a mathematical expression.
 *
 * A factor can be a number, an identifier (variable), a function call, a parenthesized expression, or a unary operator (minus).
 * This function handles these cases by recursively parsing them into an Abstract Syntax Tree (AST).
 *
 * @param lexer A pointer to the lexer used for tokenizing the input expression.
 * @return A pointer to the root node of the Abstract Syntax Tree (AST) representing the factor.
 */
Node *parse_factor(Lexer *lexer);

/**
 * @brief Frees the memory allocated for a node and its child nodes in the Abstract Syntax Tree (AST).
 *
 * This function recursively frees all nodes in the AST, starting from the given node. It handles different node types
 * such as operations (which have left and right children), functions (which have arguments), and leaf nodes (numbers and identifiers).
 *
 * @param node A pointer to the node to be freed.
 */
void free_node(Node *node);

#endif //PARSER_H
