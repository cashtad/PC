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

Limits parse_limits(const char* limits_str) {
    Limits limits = {-10, 10, -10, 10};  // Значения по умолчанию
    sscanf(limits_str, "%lf:%lf:%lf:%lf", &limits.x_min, &limits.x_max, &limits.y_min, &limits.y_max);
    return limits;
}

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

// Advances the lexer's position by one character
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

// Parses a full expression (supporting addition and subtraction)
Node* parse_expr(Lexer* lexer);

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
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->type = NODE_OP;

        if (token.type == TOKEN_MUL) {
            new_node->op.op = '*';
        } else if (token.type == TOKEN_DIV) {
            new_node->op.op = '/';
        } else if (token.type == TOKEN_POW) {
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

// Evaluates the abstract syntax tree (AST) recursively
double evaluate(Node* node) {
    if (node->type == NODE_NUM) {
        return node->num;
    } else if (node->type == NODE_ID) {
        // Handle variables here if needed
        return 0.0; // Placeholder
    } else if (node->type == NODE_FUNC) {
        double arg_value = evaluate(node->func.arg);
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
    } else if (node->type == NODE_OP) {
        if (node->op.left == NULL) {
            // Unarnian operator
            double right_value = evaluate(node->op.right);
            switch (node->op.op) {
                case '-':
                    return -right_value;
                default:
                    fprintf(stderr, "Error: unknown unary operator '%c'\n", node->op.op);
                exit(EXIT_FAILURE);
            }
        } else {
            // Binary operator
            double left_value = evaluate(node->op.left);
            double right_value = evaluate(node->op.right);
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

char* replace_x_in_function(const char* function_str, double number) {
    char number_str[32];
    snprintf(number_str, sizeof(number_str), "%.2f", number);  // Преобразуем число в строку с точностью до 2 знаков

    // Вычисляем новый размер строки: заменяем каждый 'x' на длину number_str
    int new_len = 0;
    for (int i = 0; function_str[i] != '\0'; i++) {
        new_len += (function_str[i] == 'x') ? strlen(number_str) : 1;
    }

    // Выделяем память для новой строки
    char* result = (char*)malloc(new_len + 1);  // +1 для завершающего нуля
    if (!result) {
        return NULL;  // Проверка на успешное выделение памяти
    }

    // Заполняем новую строку, заменяя 'x' на number_str
    int pos = 0;
    for (int i = 0; function_str[i] != '\0'; i++) {
        if (function_str[i] == 'x') {
            strcpy(&result[pos], number_str);  // Вставляем number_str на место 'x'
            pos += strlen(number_str);
        } else {
            result[pos++] = function_str[i];  // Копируем символ, если это не 'x'
        }
    }
    result[pos] = '\0';  // Завершаем строку

    return result;
}

// Main part of program, gets everything ready, uses test cases
int main(int argc, char* argv[]) {
    clock_t start, end;
    double cpu_time_used;

    // Проверка на обязательные параметры
    if (argc < 3) {
        fprintf(stderr, "Использование: %s <expression> <output_file> [limits]\n", argv[0]);
        return 1;
    }

    const char* expression = argv[1];
    const char* output_file = argv[2];
    Limits limits = {-10, 10, -10, 10};  // Значения по умолчанию

    // Проверка на наличие третьего параметра limits
    if (argc == 4) {
        limits = parse_limits(argv[3]);
    }

    printf("Expression: %s\n", expression);
    printf("Output file: %s\n", output_file);
    printf("Limits: x_min=%.2f, x_max=%.2f, y_min=%.2f, y_max=%.2f\n",
           limits.x_min, limits.x_max, limits.y_min, limits.y_max);

    start = clock();  // Старт замера времени

    // Открываем .ps файл для записи
    FILE* file = fopen(output_file, "w");
    if (!file) {
        fprintf(stderr, "Не удалось открыть файл для записи: %s\n", output_file);
        return 1;
    }

    // Пишем заголовок PostScript файла
    fprintf(file, "%%!PS-Adobe-3.0\n");
    fprintf(file, "%%BoundingBox: %d %d %d %d\n", 0, 0, 500, 500);
    fprintf(file, "/x_min %f def\n/x_max %f def\n", limits.x_min, limits.x_max);
    fprintf(file, "/y_min %f def\n/y_max %f def\n", limits.y_min, limits.y_max);
    fprintf(file, "/x_scale 500 x_max x_min sub div def\n");
    fprintf(file, "/y_scale 500 y_max y_min sub div def\n");
    fprintf(file, "/translate { %d %d translate } def\n", 250, 250);
    fprintf(file, "0.5 setlinewidth\n");

    // Начало пути для графика функции
    fprintf(file, "newpath\n");

    // Вычисление и запись точек графика
    int first_point = 1;
    for (float x = limits.x_min; x <= limits.x_max; x += 0.01) {
        // Замена 'x' в выражении на текущее значение x
        char* expression_new = replace_x_in_function(expression, x);
        Lexer* lexer = create_lexer(expression_new);
        Node* syntax_tree = parse_expr(lexer);
        double result = evaluate(syntax_tree);

        // Проверка, попадает ли результат в пределы y
        if (result >= limits.y_min && result <= limits.y_max) {
            // Преобразование координат в координаты для PostScript
            double x_pos = (x - limits.x_min) * (500 / (limits.x_max - limits.x_min));
            double y_pos = (result - limits.y_min) * (500 / (limits.y_max - limits.y_min));

            if (first_point) {
                fprintf(file, "%.2f %.2f moveto\n", x_pos, y_pos);
                first_point = 0;
            } else {
                fprintf(file, "%.2f %.2f lineto\n", x_pos, y_pos);
            }
        }

        // Освобождение памяти
        free(expression_new);
        free_node(syntax_tree);
        free(lexer);
    }

    // Завершение пути и рисование графика
    fprintf(file, "stroke\n");
    fprintf(file, "showpage\n");

    fclose(file);

    end = clock();  // Конец замера времени
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Время выполнения: %f секунд\n", cpu_time_used);

    return 0;
}

