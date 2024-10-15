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

            // printf("Token: NUM(%.6f)\n", token.num);
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
                //printf("Token: FUNC(%s)\n", token.func);
                return token;
            }
            token.type = TOKEN_ID;
            //printf("Token: ID(%s)\n", token.func);
            return token;
        }

        if (lexer->current_char == '+') {
            advance(lexer);
            //printf("Token: PLUS\n");
            return (Token){ TOKEN_PLUS };
        }

        if (lexer->current_char == '-') {
            advance(lexer);
            //printf("Token: MINUS\n");
            return (Token){ TOKEN_MINUS };
        }

        if (lexer->current_char == '*') {
            advance(lexer);
            //printf("Token: MUL\n");
            return (Token){ TOKEN_MUL };
        }

        if (lexer->current_char == '/') {
            advance(lexer);
            //printf("Token: DIV\n");
            return (Token){ TOKEN_DIV };
        }

        if (lexer->current_char == '(') {
            advance(lexer);
            //printf("Token: LPAREN\n");
            return (Token){ TOKEN_LPAREN };
        }

        if (lexer->current_char == ')') {
            advance(lexer);
            //printf("Token: RPAREN\n");
            return (Token){ TOKEN_RPAREN };
        }

        fprintf(stderr, "Error: unknown character '%c'\n", lexer->current_char);
        exit(EXIT_FAILURE);
    }

    //printf("Token: EOF\n");
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
        //printf("Parsed number: %.6f\n", node->num);
    } else if (token.type == TOKEN_ID) {
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_ID;
        strcpy(node->id, token.id);
        //printf("Parsed identifier: %s\n", node->id);
    } else if (token.type == TOKEN_FUNC) {
        node = (Node*)malloc(sizeof(Node));
        node->type = NODE_FUNC;
        strcpy(node->func.func, token.func);
        //printf("Parsing function: %s\n", node->func.func);

        token = get_next_token(lexer); // Expecting '('
        if (token.type != TOKEN_LPAREN) {
            fprintf(stderr, "Error: expected '(' after function '%s'\n", node->func.func);
            exit(EXIT_FAILURE);
        }
        node->func.arg = parse_expr(lexer); // parse the argument for the function
        token = get_next_token(lexer); // Expecting ')'
        if (token.type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: expected closing parenthesis\n");
            exit(EXIT_FAILURE);
        }
        //printf("Function %s parsed successfully with argument.\n", node->func.func);
    } else if (token.type == TOKEN_LPAREN) {
        node = parse_expr(lexer);
        token = get_next_token(lexer); // Expecting ')'
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
        //printf("Parsed term with operator: %c\n", new_node->op.op);
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
        //printf("Parsed expression with operator: %c\n", new_node->op.op);
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
        // Handle variables here if needed
        return 0.0; // Placeholder
    } else if (node->type == NODE_FUNC) {
        double arg_value = evaluate(node->func.arg);
        double result;

        if (strcmp(node->func.func, "sin") == 0) {
            result = sin(arg_value);
            //printf("Evaluating function: sin with argument: %.6f, result: %.6f\n", arg_value, result);
        } else if (strcmp(node->func.func, "cos") == 0) {
            result = cos(arg_value);
            //printf("Evaluating function: cos with argument: %.6f, result: %.6f\n", arg_value, result);
        } else {
            fprintf(stderr, "Error: unknown function '%s'\n", node->func.func);
            exit(EXIT_FAILURE);
        }
        return result;
    } else if (node->type == NODE_OP) {
        double left_value = evaluate(node->op.left);
        double right_value = evaluate(node->op.right);
        double result;

        switch (node->op.op) {
            case '+':
                result = left_value + right_value;
                //printf("Evaluating: %.6f + %.6f = %.6f\n", left_value, right_value, result);
                break;
            case '-':
                result = left_value - right_value;
                //printf("Evaluating: %.6f - %.6f = %.6f\n", left_value, right_value, result);
                break;
            case '*':
                result = left_value * right_value;
                //printf("Evaluating: %.6f * %.6f = %.6f\n", left_value, right_value, result);
                break;
            case '/':
                result = left_value / right_value;
                //printf("Evaluating: %.6f / %.6f = %.6f\n", left_value, right_value, result);
                break;
            default:
                fprintf(stderr, "Error: unknown operator '%c'\n", node->op.op);
                exit(EXIT_FAILURE);
        }
        return result;
    }

    fprintf(stderr, "Error: unknown node type\n");
    exit(EXIT_FAILURE);
}

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

int main() {
    const char* expressions[] = {
        "sin(0) + cos(0)",       // 0 + 1 = 1
        "2 * sin(0) + 4",        // 0 + 4 = 4
        "cos(0) * sin(0)",       // 1 * 0 = 0
        "sin(0) + 1",            // 0 + 1 = 1
        "2 * (3 + 5)",           // 2 * 8 = 16
        "10 - 2 * cos(0) + 5",   // 10 - 2 * 1 + 5 = 13
        "3 + 5 * (2 + 1)",       // 3 + 15 = 18
        "cos(0) + sin(0)",        // 1 + 0 = 1
        "5 * 8 / (10 + 0) / (3 + 5) + 1 - 6 * (1 + 1) + 3 - 5 / 5",
    };

    const double expected_results[] = {
        1.0,   // sin(0) + cos(0)
        4.0,   // 2 * sin(0) + 4
        0.0,   // cos(0) * sin(0)
        1.0,   // sin(0) + 1
        16.0,  // 2 * (3 + 5)
        13.0,  // 10 - 2 * cos(0) + 5
        18.0,  // 3 + 5 * (2 + 1)
        1.0,    // cos(0) + sin(0)
        -8.5   // 5 * 8 / (10 + 0) / (3 + 5) + 1 - 6 * (1 + 1) + 3 - 5 / 5
    };

    size_t num_expressions = sizeof(expressions) / sizeof(expressions[0]);

    for (size_t i = 0; i < num_expressions; i++) {
        Lexer* lexer = create_lexer(expressions[i]);
        Node* syntax_tree = parse_expr(lexer);
        double result = evaluate(syntax_tree);

        printf("Expression: %s | Expected: %.6f | Got: %.6f\n", expressions[i], expected_results[i], result);

        // Free memory
        free_node(syntax_tree);
        free(lexer);
    }

    return 0;
}

