#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

// Token types
typedef enum TokenType {
    TOKEN_NUM,      // Numeric token
    TOKEN_ID,       // Identifier token (e.g., variable names)
    TOKEN_FUNC,     // Function name token
    TOKEN_PLUS,     // '+' operator token
    TOKEN_MINUS,    // '-' operator token
    TOKEN_MUL,      // '*' operator token
    TOKEN_DIV,      // '/' operator token
    TOKEN_LPAREN,   // Left parenthesis token '('
    TOKEN_RPAREN,   // Right parenthesis token ')'
    TOKEN_POW,      // '^' operator token (for exponentiation)
    TOKEN_EOF       // End of input token
} TokenType;

// Token structure
typedef struct Token {
    TokenType type; // Type of token
    union {
        double num;  // For numeric values
        char id[64]; // For variable identifiers
        char func[64]; // For function names
    };
} Token;

// Lexer structure to store the current state of lexical analysis
typedef struct Lexer {
    const char* text;  // Input text being parsed
    size_t pos;        // Current position in the text
    char current_char; // Current character being analyzed
} Lexer;

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

typedef struct Limits {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
} Limits;


int parse_limits(const char* limits_str, Limits* limits) {
    char* endptr;

    limits->x_min = strtod(limits_str, &endptr);
    if (*endptr != ':') return 1;

    limits->x_max = strtod(endptr + 1, &endptr);
    if (*endptr != ':') return 1;

    limits->y_min = strtod(endptr + 1, &endptr);
    if (*endptr != ':') return 1;

    limits->y_max = strtod(endptr + 1, &endptr);
    if (*endptr != '\0') return 1;

    if (limits->x_min > limits->x_max) return 1;
    if (limits->y_min > limits->y_max) return 1;

    return 0; // Success
}

// Function prototypes
Lexer* create_lexer(const char* text);
void advance(Lexer* lexer);
void skip_whitespace(Lexer* lexer);
Token get_next_token(Lexer* lexer);
Node* parse_expr(Lexer* lexer);
Node* parse_factor(Lexer* lexer);
Node* parse_term(Lexer* lexer);
double evaluate(Node* node, double x_value);
void free_node(Node* node);

// Lexer functions
Lexer* create_lexer(const char* text) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->text = text;
    lexer->pos = 0;
    lexer->current_char = text[0];
    return lexer;
}

// Advances the lexer position by one character
void advance(Lexer* lexer) {
    lexer->pos++;
    if (lexer->pos < strlen(lexer->text)) {
        lexer->current_char = lexer->text[lexer->pos];
    } else {
        lexer->current_char = '\0';  // End of input
    }
}

// Skips whitespace characters in the input
void skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
        advance(lexer);
    }
}

// Extracts the next token from the input stream
Token get_next_token(Lexer* lexer) {
    while (lexer->current_char != '\0') {
        if (isspace(lexer->current_char)) {
            skip_whitespace(lexer);
            continue;
        }

        // Handle numbers (including floating point)
        if (isdigit(lexer->current_char) || lexer->current_char == '.') {
            Token token = { TOKEN_NUM, .num = 0.0 };
            double fraction = 1.0;
            int is_fraction = 0;

            // Handle both integer and floating-point parts
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

        // Handle identifiers and function names
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

            // Check if it's a recognized function
            if (strcmp(token.func, "cos") == 0 ||
                strcmp(token.func, "sin") == 0 ||
                strcmp(token.func, "tan") == 0 ||
                strcmp(token.func, "abs") == 0 ||
                strcmp(token.func, "ln") == 0 ||
                strcmp(token.func, "log") == 0 ||
                strcmp(token.func, "asin") == 0 ||
                strcmp(token.func, "acos") == 0 ||
                strcmp(token.func, "atan") == 0 ||
                strcmp(token.func, "sinh") == 0 ||
                strcmp(token.func, "cosh") == 0 ||
                strcmp(token.func, "tanh") == 0 ||
                strcmp(token.func, "exp") == 0) {
                token.type = TOKEN_FUNC;
                return token;
    }
            token.type = TOKEN_ID;
            //printf("Token: ID(%s)\n", token.func);
            return token;
        }

        // Handle operators and parentheses
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

        if (lexer->current_char == '^') {
            advance(lexer);
            return (Token){ TOKEN_POW };
        }

        // Handle unknown characters
        fprintf(stderr, "Error: unknown character '%c'\n", lexer->current_char);
        exit(EXIT_FAILURE);
    }

    //printf("Token: EOF\n");
    return (Token){ TOKEN_EOF };
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

// Evaluate expression with `x`
double evaluate(Node* node, const double x_value) {
    if (node->type == NODE_NUM) {
        return node->num;
    }
    if (node->type == NODE_ID) {
        if (strcmp(node->id, "x") == 0) {
            return x_value;
        }
        return 0.0; // Default value for unknown variable
    }
    if (node->type == NODE_FUNC) {
        const double arg_value = evaluate(node->func.arg, x_value);
        double result;

        // Handle standard mathematical functions
        if (strcmp(node->func.func, "sin") == 0) {
            result = sin(arg_value);
        } else if (strcmp(node->func.func, "cos") == 0) {
            result = cos(arg_value);
        } else if (strcmp(node->func.func, "tan") == 0) {
            result = tan(arg_value);
        } else if (strcmp(node->func.func, "abs") == 0) {
            result = fabs(arg_value);
        } else if (strcmp(node->func.func, "ln") == 0) {
            result = log(arg_value);
        } else if (strcmp(node->func.func, "log") == 0) {
            result = log10(arg_value);
        } else if (strcmp(node->func.func, "asin") == 0) {
            result = asin(arg_value);
        } else if (strcmp(node->func.func, "acos") == 0) {
            result = acos(arg_value);
        } else if (strcmp(node->func.func, "atan") == 0) {
            result = atan(arg_value);
        } else if (strcmp(node->func.func, "sinh") == 0) {
            result = sinh(arg_value);
        } else if (strcmp(node->func.func, "cosh") == 0) {
            result = cosh(arg_value);
        } else if (strcmp(node->func.func, "tanh") == 0) {
            result = tanh(arg_value);
        } else if (strcmp(node->func.func, "exp") == 0) {
            result = exp(arg_value);
        } else {
            fprintf(stderr, "Error: unknown function '%s'\n", node->func.func);
            exit(EXIT_FAILURE);
        }
        return result;
    } if (node->type == NODE_OP) {
        if (node->op.left == NULL) {
            // Unarnian operator
            const double right_value = evaluate(node->op.right, x_value);
            switch (node->op.op) {
                case '-':
                    return -right_value;
                default:
                    fprintf(stderr, "Error: unknown unary operator '%c'\n", node->op.op);
                exit(EXIT_FAILURE);
            }
        }
        // Binary operator
        const double left_value = evaluate(node->op.left, x_value);
        const double right_value = evaluate(node->op.right, x_value);
        double result;

        switch (node->op.op) {
            case '+':
                result = left_value + right_value;
                break;
            case '-':
                result = left_value - right_value;
                break;
            case '*':
                result = left_value * right_value;
                break;
            case '/':
                result = left_value / right_value;
                break;
            case '^':
                result = pow(left_value, right_value);
                break;
            default:
                fprintf(stderr, "Error: unknown binary operator '%c'\n", node->op.op);
                exit(EXIT_FAILURE);
        }
        return result;
    }

    fprintf(stderr, "Error: unknown node type\n");
    exit(EXIT_FAILURE);
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



// Main function
int main(const int argc, char* argv[]) {
    // Necessary arguments check
    if (argc < 3) {
        fprintf(stderr, "Wrong parameters. Use as: %s <expression> <output_file> [limits]\n", argv[0]);
        return 1;
    }

    const char* expression = argv[1];
    const char* output_file = argv[2];
    Limits limits = {-10, 10, -10, 10};  // Default values

    // Проверка на наличие третьего параметра limits
    if (argc == 4) {
        if (parse_limits(argv[3], &limits) == 0) {
            printf("Limits: x_min=%.2f, x_max=%.2f, y_min=%.2f, y_max=%.2f\n",
           limits.x_min, limits.x_max, limits.y_min, limits.y_max);
        }
        else {
            printf("Error parsing limits string.\n");
            return 1;
        }
    }

    printf("Expression: %s\n", expression);
    printf("Output file: %s\n", output_file);
    printf("Limits: x_min=%.2f, x_max=%.2f, y_min=%.2f, y_max=%.2f\n",
           limits.x_min, limits.x_max, limits.y_min, limits.y_max);


    const clock_t start = clock();  // Старт замера времени

    // Открываем .ps файл для записи
    FILE* file = fopen(output_file, "w");
    if (!file) {
        fprintf(stderr, "Unable to open output file: %s\n", output_file);
        return 1;
    }


    const double page_width = 595.0;  // Ширина страницы A4
    const double page_height = 842.0; // Высота страницы A4

    fprintf(file, "%%PageSetup\n");
    fprintf(file, "<< /PageSize [%f %f] >> setpagedevice\n", page_width, page_height);

    // Масштабирование
    const double scale_x = (page_width - 100) / (limits.x_max - limits.x_min);  // Масштаб по оси X
    const double scale_y = (page_height - 100) / (limits.y_max - limits.y_min);           // Масштаб по оси Y

    // Печать заголовка PostScript
    fprintf(file, "%%!PS\n");
    fprintf(file, "/inch {72 mul} def\n");

    // Сдвиг начала координат в центр страницы
    fprintf(file, "%f %f translate\n", page_width / 2, page_height / 2);

    // Инвертирование оси Y (для правильной ориентации графика)
    // fprintf(file, "1 -1 scale\n");

    fprintf(file, "1 0 0 setrgbcolor\n");


    fprintf(file, "%f %f moveto\n", limits.x_min * scale_x, 0.0);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x, 0.0);
    fprintf(file, "stroke\n");

    fprintf(file, "%f %f moveto\n", 0.0, limits.y_min * scale_y);
    fprintf(file, "%f %f lineto\n", 0.0, limits.y_max * scale_y);
    fprintf(file, "stroke\n");


    fprintf(file, "0 0 0 setrgbcolor\n");


    Lexer* lexer = create_lexer(expression);
    Node* root = parse_expr(lexer);
    free(lexer);

    int first_point = 1;
    for (double x = limits.x_min; x <= limits.x_max; x += 0.01) {
        const double y = evaluate(root, x);
        const double ps_x = x * scale_x; // Масштабированное значение x
        const double ps_y = y * scale_y;            // Масштабированное значение y
        if (first_point) {
            fprintf(file, "%f %f moveto\n", ps_x, ps_y);
            first_point = 0;
        } else {
            fprintf(file, "%f %f lineto\n", ps_x, ps_y);

        }
    }
    fprintf(file, "stroke\n");



    fprintf(file,"showpage\n");

    fclose(file);

    const clock_t end = clock();  // Конец замера времени
    const double cpu_time_used = (double) (end - start) / CLOCKS_PER_SEC;
    printf("Time spent: %f seconds\n", cpu_time_used);

    return 0;
}
