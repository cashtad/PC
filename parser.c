#include "parser.h"

Node *parse_expr(Lexer *lexer) {
    Node *node = parse_factor(lexer);
    if (node->type == NODE_ERROR) return node;
    Token token = get_next_token(lexer);

    while (token.type == TOKEN_PLUS || token.type == TOKEN_MINUS || token.type == TOKEN_MUL || token.type == TOKEN_DIV
           || token.type == TOKEN_POW) {
        Node *new_node = malloc(sizeof(Node));
        new_node->type = NODE_OP;
        if (token.type == TOKEN_MUL) {
            new_node->op.op = MULT;
        } else if (token.type == TOKEN_DIV) {
            new_node->op.op = DIVISION;
        } else if (token.type == TOKEN_POW) {
            new_node->op.op = POWER;
        } else if (token.type == TOKEN_PLUS) {
            new_node->op.op = PLUS;
        } else {
            new_node->op.op = MINUS;
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

Node *parse_factor(Lexer *lexer) {
    Token token = get_next_token(lexer);
    Node *node = NULL;

    // Handle unary minus
    if (token.type == TOKEN_MINUS) {
        node = (Node *) malloc(sizeof(Node));
        node->type = NODE_OP;
        node->op.op = MINUS;
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
        // Expect '('
        token = get_next_token(lexer);
        if (token.type != TOKEN_LPAREN) {
            return node;
        }
        // Parse function argument
        node->func.arg = parse_expr(lexer);
        token = get_next_token(lexer);
        // Expect ')'
        if (token.type != TOKEN_RPAREN) {
            return node;
        }
    } else if (token.type == TOKEN_LPAREN) {
        // Handle parenthesized expressions
        node = parse_expr(lexer);
        token = get_next_token(lexer);
        // Expect ')'
        if (token.type != TOKEN_RPAREN) {
            return node;
        }
    } else {
        node = (Node *) malloc(sizeof(Node));
        node->type = NODE_ERROR;
        return node;
    }
    return node;
}

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
