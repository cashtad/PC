#include "parser.h"


Node *parse(Lexer *lexer) {
    Node *node = parse_low_priority_expression(lexer);
    const size_t length = strlen(lexer->text);
    if (lexer->pos != length - 1) {
        free_node(node);
        error_exit(ERROR_EXPRESSION_TEXT, ERROR_FUNCTION);
    }
    return node;
}



Node *parse_low_priority_expression(Lexer *lexer) {
    Node *node = parse_high_priority_expression(lexer);  // Start by parsing high priority expressions
    if (node->type == NODE_ERROR) return node;
    Token token = get_next_token(lexer);

    // Handle low priority operations: addition and subtraction
    while (token.type == TOKEN_PLUS || token.type == TOKEN_MINUS) {
        Node *new_node = malloc(sizeof(Node));
        new_node->type = NODE_OP;
        if (token.type == TOKEN_PLUS) {
            new_node->op.op = PLUS;
        } else {
            new_node->op.op = MINUS;
        }
        // Continue parsing after the operator
        new_node->op.left = node;
        new_node->op.right = parse_high_priority_expression(lexer);
        node = new_node;
        token = get_next_token(lexer);
    }

    // Backtrack to the position where the last token was not a low priority operator
    lexer->pos--;
    lexer->current_char = lexer->text[lexer->pos];
    return node;
}

Node *parse_high_priority_expression(Lexer *lexer) {
    Node *node = parse_operand(lexer);  // Parse basic elements (operands)
    if (node->type == NODE_ERROR) return node;

    Token token = get_next_token(lexer);

    // Handle high priority operations: multiplication, division, exponentiation
    while (token.type == TOKEN_MUL || token.type == TOKEN_DIV || token.type == TOKEN_POW) {
        Node *new_node = malloc(sizeof(Node));
        new_node->type = NODE_OP;

        if (token.type == TOKEN_MUL) {
            new_node->op.op = MULT;
        } else if (token.type == TOKEN_DIV) {
            new_node->op.op = DIVISION;
        } else {
            new_node->op.op = POWER;
        }

        // Continue parsing after the operator
        new_node->op.left = node;
        new_node->op.right = parse_operand(lexer);
        node = new_node;
        token = get_next_token(lexer);
    }

    // Backtrack to the position where the last token was not a high priority operator
    lexer->pos--;
    lexer->current_char = lexer->text[lexer->pos];
    return node;
}

Node *parse_operand(Lexer *lexer) {
    Token token = get_next_token(lexer);
    Node *node = NULL;

    // Handle unary minus
    if (token.type == TOKEN_MINUS) {
        node = (Node *) malloc(sizeof(Node));
        node->type = NODE_OP;
        node->op.op = MINUS;
        node->op.left = NULL;  // No left operand for unary operator
        node->op.right = parse_operand(lexer);  // Parse the right operand
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
        node->func.arg = parse_low_priority_expression(lexer);
        token = get_next_token(lexer);
        // Expect ')'
        if (token.type != TOKEN_RPAREN) {
            return node;
        }
    } else if (token.type == TOKEN_LPAREN) {
        // Handle expressions in parentheses
        node = parse_low_priority_expression(lexer);
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
