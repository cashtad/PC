//
// Created by lemal on 24.12.2024.
//

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parses a full expression
Node* parse_expr(Lexer* lexer) {
    Node* node = parse_term(lexer);
    Token token = get_next_token(lexer);

    while (token.type == TOKEN_PLUS || token.type == TOKEN_MINUS) {
        Node* new_node = malloc(sizeof(Node));
        new_node->type = NODE_OP;
        new_node->op.op = token.type == TOKEN_PLUS ? '+' : '-';
        new_node->op.left = node;
        new_node->op.right = parse_term(lexer);
        node = new_node;
        //printf("Parsed expression with operator: %c\n", new_node->op.op);
        token = get_next_token(lexer);
    }

    lexer->pos--; // Push back the last token
    lexer->current_char = lexer->text[lexer->pos];
    return node;
}



// Parses a term
Node* parse_term(Lexer* lexer) {
    Node* node = parse_factor(lexer);
    Token token = get_next_token(lexer);

    // Handle multiplication, division, and exponentiation
    while (token.type == TOKEN_MUL || token.type == TOKEN_DIV || token.type == TOKEN_POW) {
        Node* new_node = malloc(sizeof(Node));
        new_node->type = NODE_OP;

        if (token.type == TOKEN_MUL) {
            new_node->op.op = '*';
        } else if (token.type == TOKEN_DIV) {
            new_node->op.op = '/';
        } else {
            new_node->op.op = '^';
        }

        new_node->op.left = node;
        new_node->op.right = parse_factor(lexer);
        node = new_node;
        token = get_next_token(lexer);
    }

    lexer->pos--; // Return last token
    lexer->current_char = lexer->text[lexer->pos];
    return node;
}


// Parses a factor (number, identifier, function, or parenthesized expression)
Node* parse_factor(Lexer* lexer) {
    Token token = get_next_token(lexer);
    Node* node = NULL;

    // Handle unary minus
    if (token.type == TOKEN_MINUS) {
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_OP;
        node->op.op = '-';
        node->op.left = NULL; // // No left operand for unary operators
        node->op.right = parse_factor(lexer); // Parse right operand
    } else if (token.type == TOKEN_NUM) {
        // Handle numeric literals
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_NUM;
        node->num = token.num;
    } else if (token.type == TOKEN_ID) {
        // Handle identifiers
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_ID;
        strcpy(node->id, token.id);
    } else if (token.type == TOKEN_FUNC) {
        // Handle function calls
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_FUNC;
        strcpy(node->func.func, token.func);

        token = get_next_token(lexer); // Expect '('
        if (token.type != TOKEN_LPAREN) {
            fprintf(stderr, "Error: expected '(' after function '%s'\n", node->func.func);
            exit(EXIT_FAILURE);
        }
        node->func.arg = parse_expr(lexer); // Parse function argument
        token = get_next_token(lexer); // Expect ')'
        if (token.type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: expected closing parenthesis\n");
            exit(EXIT_FAILURE);
        }
    } else if (token.type == TOKEN_LPAREN) {
        // Handle parenthesized expressions
        node = parse_expr(lexer);
        token = get_next_token(lexer); // Expect ')'
        if (token.type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: expected closing parenthesis\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Error: unexpected token\n");
        exit(EXIT_FAILURE);
    }
    return node;
}



// Frees memory used by the abstract syntax tree (AST)
void free_node(Node* node) {
    if (node == NULL) return;
    if (node->type == NODE_OP) {
        free_node(node->op.left);
        free_node(node->op.right);
    } else if (node->type == NODE_FUNC) {
        free_node(node->func.arg);
    }
    free(node);
}
