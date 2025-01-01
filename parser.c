#include "parser.h"
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

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
Node *parse_expr(Lexer *lexer) {
    Node *node = parse_factor(lexer);
    Token token = get_next_token(lexer);

    while (token.type == TOKEN_PLUS || token.type == TOKEN_MINUS || token.type == TOKEN_MUL || token.type == TOKEN_DIV
           || token.type == TOKEN_POW) {
        Node *new_node = malloc(sizeof(Node));
        new_node->type = NODE_OP;
        if (token.type == TOKEN_MUL) {
            new_node->op.op = '*';
        } else if (token.type == TOKEN_DIV) {
            new_node->op.op = '/';
        } else if (token.type == TOKEN_POW) {
            new_node->op.op = '^';
        } else if (token.type == TOKEN_PLUS) {
            new_node->op.op = '+';
        } else {
            new_node->op.op = '-';
        }
        new_node->op.left = node;
        new_node->op.right = parse_factor(lexer);
        node = new_node;
        token = get_next_token(lexer);
    }

    lexer->pos--;
    lexer->current_char = lexer->text[lexer->pos];
    return node;
}


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
*/
Node *parse_factor(Lexer *lexer) {
    Token token = get_next_token(lexer);
    Node *node = NULL;

    // Handle unary minus
    if (token.type == TOKEN_MINUS) {
        node = (Node *) malloc(sizeof(Node));
        node->type = NODE_OP;
        node->op.op = '-';
        node->op.left = NULL; // // No left operand for unary operators
        node->op.right = parse_factor(lexer); // Parse right operand
    } else if (token.type == TOKEN_NUM) {
        // Handle numeric literals
        node = (Node *) malloc(sizeof(Node));
        node->type = NODE_NUM;
        node->num = token.num;
    } else if (token.type == TOKEN_ID) {
        // Handle identifiers
        node = (Node *) malloc(sizeof(Node));
        node->type = NODE_ID;
        strcpy(node->id, token.id);
    } else if (token.type == TOKEN_FUNC) {
        // Handle function calls
        node = (Node *) malloc(sizeof(Node));
        node->type = NODE_FUNC;
        strcpy(node->func.func, token.func);

        token = get_next_token(lexer); // Expect '('
        if (token.type != TOKEN_LPAREN) {
            error_exit("Error: expected '(' after function\n", 2);

        }
        node->func.arg = parse_expr(lexer); // Parse function argument
        token = get_next_token(lexer); // Expect ')'
        if (token.type != TOKEN_RPAREN) {
            error_exit("Error: expected closing parenthesis\n", 2);

        }
    } else if (token.type == TOKEN_LPAREN) {
        // Handle parenthesized expressions
        node = parse_expr(lexer);
        token = get_next_token(lexer); // Expect ')'
        if (token.type != TOKEN_RPAREN) {
            error_exit("Error: expected closing parenthesis\n", 2);
        }
    } else {
        error_exit("Error: unexpected token\n", 2);
    }
    return node;
}


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
void free_node(Node *node) {
    if (node == NULL) return;
    if (node->type == NODE_OP) {
        free_node(node->op.left);
        free_node(node->op.right);
    } else if (node->type == NODE_FUNC) {
        free_node(node->func.arg);
    }
    free(node);
}
