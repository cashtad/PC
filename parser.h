//
// Created by lemal on 24.12.2024.
//

#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"


// Node types for the abstract syntax tree (AST)
typedef struct Node {
    enum type { NODE_NUM, NODE_ID, NODE_FUNC, NODE_OP } type; // Node type (numeric, identifier, function, operator)
    union {
        double num; // For numeric values
        char id[64]; // For variable identifiers
        struct { char func[64]; struct Node* arg; } func; // For functions and their arguments
        struct { char op; struct Node* left; struct Node* right; } op; // For binary operations
    };
} Node;


Node* parse_expr(Lexer* lexer);
Node* parse_term(Lexer* lexer);
Node* parse_factor(Lexer* lexer);
void free_node(Node* node);

#endif //PARSER_H
