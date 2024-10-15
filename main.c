#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

// Token types
typedef enum {
    TOKEN_NUM,
    TOKEN_ID,
    TOKEN_FUNC,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    union {
        double num;  // For numeric values
        char id[64]; // For variable identifiers
        char func[64]; // For function names
    };
} Token;

// Lexer structure
typedef struct {
    const char* text;
    size_t pos;
    char current_char;
} Lexer;

// Node types for the abstract syntax tree (AST)
typedef struct Node {
    enum { NODE_NUM, NODE_ID, NODE_FUNC, NODE_OP } type;
    union {
        double num; // For numeric values
        char id[64]; // For variable identifiers
        struct { char func[64]; struct Node* arg; } func; // For functions
        struct { char op; struct Node* left; struct Node* right; } op; // For operations
    };
} Node;

// Function prototypes
Lexer* create_lexer(const char* text);
void advance(Lexer* lexer);
void skip_whitespace(Lexer* lexer);
Token get_next_token(Lexer* lexer);
Node* parse_expr(Lexer* lexer);
Node* parse_factor(Lexer* lexer);
Node* parse_term(Lexer* lexer);
double evaluate(Node* node);
void free_node(Node* node);

Lexer* create_lexer(const char* text) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->text = text;
    lexer->pos = 0;
    lexer->current_char = text[0];
    return lexer;
}

void advance(Lexer* lexer) {
    lexer->pos++;
    if (lexer->pos < strlen(lexer->text)) {
        lexer->current_char = lexer->text[lexer->pos];
    } else {
        lexer->current_char = '\0';
    }
}

void skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
        advance(lexer);
    }
}

Token get_next_token(Lexer* lexer) {
    while (lexer->current_char != '\0') {
        if (isspace(lexer->current_char)) {
            skip_whitespace(lexer);
            continue;
        }

        if (isdigit(lexer->current_char) || lexer->current_char == '.') {
            Token token = { TOKEN_NUM, .num = 0.0 };
            double fraction = 1.0;
            int is_fraction = 0;

            while (isdigit(lexer->current_char) || lexer->current_char == '.') {
                if (lexer->current_char == '.') {
                    is_fraction = 1; // Start of fractional part
                    advance(lexer);
                    continue;
                }
                if (is_fraction) {
                    fraction *= 0.1;
                    token.num += (lexer->current_char - '0') * fraction;
                } else {
                    token.num = token.num * 10 + (lexer->current_char - '0');
                }
                advance(lexer);
            }
            return token;
        }

        if (isalpha(lexer->current_char)) {
            Token token;
            size_t len = 0;
            while (isalpha(lexer->current_char)) {
                if (len < 63) {
                    token.func[len++] = lexer->current_char; // Use func for both ID and function names
                }
                advance(lexer);
            }
            token.func[len] = '\0';
            if (strcmp(token.func, "cos") == 0 || strcmp(token.func, "sin") == 0 || strcmp(token.func, "tan") == 0) {
                token.type = TOKEN_FUNC;
                return token;
            }
            token.type = TOKEN_ID;
            return token;
        }

        if (lexer->current_char == '+') {
            advance(lexer);
            return (Token){ TOKEN_PLUS };
        }

        if (lexer->current_char == '-') {
            advance(lexer);
            return (Token){ TOKEN_MINUS };
        }

        if (lexer->current_char == '*') {
            advance(lexer);
            return (Token){ TOKEN_MUL };
        }

        if (lexer->current_char == '/') {
            advance(lexer);
            return (Token){ TOKEN_DIV };
        }

        if (lexer->current_char == '(') {
            advance(lexer);
            return (Token){ TOKEN_LPAREN };
        }

        if (lexer->current_char == ')') {
            advance(lexer);
            return (Token){ TOKEN_RPAREN };
        }

        fprintf(stderr, "Error: unknown character '%c'\n", lexer->current_char);
        exit(EXIT_FAILURE);
    }

    return (Token){ TOKEN_EOF };
}

Node* parse_expr(Lexer* lexer);

Node* parse_factor(Lexer* lexer) {
    Token token = get_next_token(lexer);
    Node* node = NULL;

    if (token.type == TOKEN_NUM) {
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_NUM;
        node->num = token.num;
    } else if (token.type == TOKEN_ID) {
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_ID;
        strcpy(node->id, token.id);
    } else if (token.type == TOKEN_FUNC) {
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_FUNC;
        strcpy(node->func.func, token.func);
        node->func.arg = parse_expr(lexer);
    } else if (token.type == TOKEN_LPAREN) {
        node = parse_expr(lexer);
        token = get_next_token(lexer); // Expecting ')'
        if (token.type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: expected closing parenthesis\n");
            exit(EXIT_FAILURE);
        }
    }

    return node;
}

Node* parse_term(Lexer* lexer) {
    Node* node = parse_factor(lexer);
    Token token = get_next_token(lexer);

    while (token.type == TOKEN_MUL || token.type == TOKEN_DIV) {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->type = NODE_OP;
        new_node->op.op = (token.type == TOKEN_MUL) ? '*' : '/';
        new_node->op.left = node;
        new_node->op.right = parse_factor(lexer);
        node = new_node;
        token = get_next_token(lexer);
    }

    lexer->pos--; // Push back the last token
    lexer->current_char = lexer->text[lexer->pos];
    return node;
}

Node* parse_expr(Lexer* lexer) {
    Node* node = parse_term(lexer);
    Token token = get_next_token(lexer);

    while (token.type == TOKEN_PLUS || token.type == TOKEN_MINUS) {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->type = NODE_OP;
        new_node->op.op = (token.type == TOKEN_PLUS) ? '+' : '-';
        new_node->op.left = node;
        new_node->op.right = parse_term(lexer);
        node = new_node;
        token = get_next_token(lexer);
    }

    lexer->pos--; // Push back the last token
    lexer->current_char = lexer->text[lexer->pos];
    return node;
}

double evaluate(Node* node) {
    if (node->type == NODE_NUM) {
        return node->num;
    } else if (node->type == NODE_ID) {
        return 0.0; // For simplicity, we return 0 for variables
    } else if (node->type == NODE_FUNC) {
        double arg_value = evaluate(node->func.arg);
        if (strcmp(node->func.func, "cos") == 0) {
            return cos(arg_value);
        } else if (strcmp(node->func.func, "sin") == 0) {
            return sin(arg_value);
        } else if (strcmp(node->func.func, "tan") == 0) {
            return tan(arg_value);
        }
    } else if (node->type == NODE_OP) {
        double left_value = evaluate(node->op.left);
        double right_value = evaluate(node->op.right);
        if (node->op.op == '+') {
            return left_value + right_value;
        } else if (node->op.op == '-') {
            return left_value - right_value;
        } else if (node->op.op == '*') {
            return left_value * right_value;
        } else if (node->op.op == '/') {
            return left_value / right_value;
        }
    }

    return 0.0; // Default return
}

void free_node(Node* node) {
    if (node) {
        if (node->type == NODE_OP) {
            free_node(node->op.left);
            free_node(node->op.right);
        } else if (node->type == NODE_FUNC) {
            free_node(node->func.arg);
        }
        free(node);
    }
}

int main() {
    const char* expression = "cos(0) * sin(0) + 4"; // Example expression
    Lexer* lexer = create_lexer(expression);
    Node* syntax_tree = parse_expr(lexer);
    double result = evaluate(syntax_tree);

    printf("Result: %f\n", result);

    // Free memory
    free_node(syntax_tree);
    free(lexer);

    return 0;
}
