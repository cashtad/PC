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
 * @brief Parses a mathematical expression from the lexer.
 *
 * This function processes the expression provided by the lexer. It first attempts to parse a low-priority expression
 * and then checks for any remaining characters in the input to ensure the entire expression has been processed.
 * If any unexpected characters are found, an error is raised and the program exits.
 *
 * @param lexer A pointer to the lexer, which contains the expression to be parsed.
 * @return A pointer to the root node of the parsed abstract syntax tree (AST) representing the expression.
 */
Node *parse(Lexer *lexer);

/**
 * @brief Parses low priority expressions (addition and subtraction) in the mathematical expression.
 *
 * This function handles the parsing of low-priority operations, which are addition and subtraction,
 * by recursively calling higher priority expression parsing functions. The function creates a new node
 * for each operation and recursively builds the abstract syntax tree (AST).
 *
 * @param lexer A pointer to the lexer that provides tokens for parsing.
 * @return A pointer to the root node of the AST representing the parsed expression.
 */
Node *parse_low_priority_expression(Lexer *lexer);

/**
 * @brief Parses high priority expressions (multiplication, division, and exponentiation).
 *
 * This function handles the parsing of high-priority operations, such as multiplication, division,
 * and exponentiation, by recursively calling operand parsing functions. The function creates a new node
 * for each operation and builds the AST for high priority expressions.
 *
 * @param lexer A pointer to the lexer that provides tokens for parsing.
 * @return A pointer to the root node of the AST representing the parsed high-priority expression.
 */
Node *parse_high_priority_expression(Lexer *lexer);

/**
 * @brief Parses operands in the mathematical expression.
 *
 * This function parses the basic elements (operands) of the expression, such as numeric literals,
 * identifiers, function calls, and sub-expressions inside parentheses. It handles unary minus operations
 * and recursively calls the appropriate function to parse the right operand if necessary.
 *
 * @param lexer A pointer to the lexer that provides tokens for parsing.
 * @return A pointer to the node representing the parsed operand.
 */
Node *parse_operand(Lexer *lexer);

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
