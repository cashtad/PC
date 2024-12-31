#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"


/**
 * @brief Represents a node in the abstract syntax tree (AST).
 *
 * This structure defines a node in the AST, which is used to represent mathematical expressions.
 * It can represent different types of nodes:
 * - **NODE_NUM**: A numeric value (e.g., a constant).
 * - **NODE_ID**: A variable identifier (e.g., 'x').
 * - **NODE_FUNC**: A function call, including the function name and its argument.
 * - **NODE_OP**: A binary operator (e.g., addition, subtraction), including the operator symbol and two child nodes.
 *
 * The structure uses a `union` to store the specific value associated with the node's type:
 * - `num`: Stores the numeric value for `NODE_NUM` nodes.
 * - `id`: Stores the identifier string for `NODE_ID` nodes (up to 64 characters).
 * - `func`: A structure containing the function name and the function's argument for `NODE_FUNC` nodes.
 * - `op`: A structure containing the operator symbol (`op`), the left operand (`left`), and the right operand (`right`) for `NODE_OP` nodes.
 *
 * This structure is used to build and manipulate the AST during the parsing process, with each node representing a part of the mathematical expression.
 */typedef struct Node {
    enum type { NODE_NUM, NODE_ID, NODE_FUNC, NODE_OP } type;
    union {
        double num;
        char id[64];
        struct {
            char func[64];
            struct Node *arg;
        } func;
        struct {
            char op;
            struct Node *left;
            struct Node *right;
        } op;
    };
} Node;

/**
 * @brief Parses an expression from the lexer input and builds an abstract syntax tree (AST).
 *
 * This function parses a mathematical expression according to operator precedence. It starts by parsing
 * a factor (the basic building block of an expression) and then processes operators in order of precedence
 * (multiplication, division, exponentiation, addition, and subtraction). For each operator encountered,
 * it creates a new node in the abstract syntax tree, with the operator as the parent node and the operands
 * as child nodes. The function continues parsing as long as it encounters operators, chaining them together
 * to form a complete expression.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character in the input text.
 * @return A pointer to the root node of the abstract syntax tree representing the parsed expression.
 */
Node *parse_expr(Lexer *lexer);

/**
 * @brief Parses a factor from the lexer input and creates a corresponding node in the abstract syntax tree (AST).
 *
 * A factor is a basic building block of an expression, such as a number, identifier, unary operator,
 * function call, or a parenthesized expression. This function handles each of these cases:
 * - Unary minus: It creates a node representing the unary operator with no left operand.
 * - Numeric literals: It creates a node with the number value.
 * - Identifiers: It creates a node with the identifier name.
 * - Function calls: It creates a node for the function and its argument, which is recursively parsed.
 * - Parenthesized expressions: It parses the expression inside the parentheses and ensures the correct closing parenthesis.
 *
 * The function returns a pointer to the created node, which is part of the abstract syntax tree representing the parsed expression.
 * If an unexpected token or syntax error is encountered, the program will terminate with an error message.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character in the input text.
 * @return A pointer to the node representing the parsed factor (e.g., number, identifier, function call, or parenthesized expression).
 * @throws Exits the program if a syntax error occurs, such as an unexpected token or missing parentheses.
 */Node *parse_factor(Lexer *lexer);

/**
 * @brief Recursively frees a node and its child nodes in the abstract syntax tree (AST).
 *
 * This function is responsible for deallocating the memory used by a node and all of its descendants in the AST.
 * It handles different node types:
 * - For operation nodes (NODE_OP), it recursively frees both the left and right operands.
 * - For function nodes (NODE_FUNC), it recursively frees the argument node.
 * - For number (NODE_NUM) and identifier (NODE_ID) nodes, no child nodes exist, so only the node itself is freed.
 *
 * The function ensures that all dynamically allocated memory for the AST is properly cleaned up to avoid memory leaks.
 *
 * @param node A pointer to the node to be freed. If the node is NULL, the function does nothing.
 */
void free_node(Node *node);

#endif //PARSER_H
