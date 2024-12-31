#include "lexer.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void error_exit(const char *message, Lexer *lexer) {
    free(lexer);
    fprintf(stderr, "Error: %s\n", message);
    exit(2);
}

Lexer *initialize_lexer(const char *text) {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->text = text;
    lexer->pos = 0;
    lexer->current_char = text[0];
    if (!areBracketsBalanced(text)) {
        error_exit("wrong usage of brackets! Ensure that brackets are balanced and used in right positions", lexer);
    }
    return lexer;
}

int areBracketsBalanced(const char *expression) {
    const int MAX_STACK_SIZE = 100;
    char stack[MAX_STACK_SIZE];
    int top = -1;

    for (int i = 0; expression[i] != '\0'; i++) {
        const char current = expression[i];

        if (current == '(') {
            if (top == MAX_STACK_SIZE - 1) {
                return 0;
            }
            stack[++top] = current;
        } else if (current == ')') {
            if (top == -1) {
                return 0;
            }
            top--;
        }
    }

    return top == -1;
}

void advance(Lexer *lexer) {
    lexer->pos++;
    if (lexer->pos < strlen(lexer->text)) {
        lexer->current_char = lexer->text[lexer->pos];
    } else {
        lexer->current_char = '\0';
    }
}

void skip_whitespace(Lexer *lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
        advance(lexer);
    }
}


Token process_number(Lexer *lexer) {
    Token token = {TOKEN_NUM, .num = 0.0};
    double fraction = 1.0;
    int is_fraction = 0;

    while (isdigit(lexer->current_char) || lexer->current_char == '.') {
        if (lexer->current_char == '.') {
            if (is_fraction) {
                error_exit("wrong function input", lexer);
            }
            is_fraction = 1;
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

    if (lexer->current_char == 'E' || lexer->current_char == 'e') {
        advance(lexer);
        int exponent_sign = 1;

        if (lexer->current_char == '+' || lexer->current_char == '-') {
            if (lexer->current_char == '-') {
                exponent_sign = -1;
            }
            advance(lexer);
        }

        if (!isdigit(lexer->current_char)) {
            error_exit("wrong exp usage", lexer);
        }

        int exponent = 0;
        while (isdigit(lexer->current_char)) {
            exponent = exponent * 10 + (lexer->current_char - '0');
            advance(lexer);
        }

        if (lexer->current_char == '.') {
            error_exit("wrong exp usage. Exponent cannot be a floating point number", lexer);
        }
        if (isalpha(lexer->current_char)) {
            error_exit("wrong exp usage. Exponent cannot contain letters", lexer);
        }
        if (lexer->current_char == '(') {
            error_exit("wrong exp usage. Exponent cannot contain '('", lexer);
        }
        if (exponent != 0) {
            token.num *= pow(10, exponent_sign * exponent);
        }
    }

    return token;
}

Token process_identifier(Lexer *lexer) {
    Token token;
    size_t len = 0;
    while (isalpha(lexer->current_char)) {
        if (len < 63) {
            token.func[len++] = lexer->current_char;
        }
        advance(lexer);
    }
    if (len >= 10) {
        error_exit("Identifier too long", lexer);
    }
    token.func[len] = '\0';
    if (strcmp(token.func, "x") != 0) {
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
        error_exit("unknown identifier", lexer);
    } else {
        token.type = TOKEN_ID;
        return token;
    }
}

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

Token process_operator(Lexer *lexer) {
    const char operator = lexer->current_char;
    advance(lexer);
    switch (operator) {
        case '+': return (Token){TOKEN_PLUS};
        case '-': return (Token){TOKEN_MINUS};
        case '*': return (Token){TOKEN_MUL};
        case '/': return (Token){TOKEN_DIV};
        case '^': return (Token){TOKEN_POW};
        default: error_exit("Unknown operator", lexer);
    }
}

int is_bracket(char c) {
    return c == '(' || c == ')';
}

Token process_bracket(Lexer *lexer) {
    if (lexer->current_char == '(') {
        advance(lexer);
        return (Token){TOKEN_LPAREN};
    } else {
        advance(lexer);
        return (Token){TOKEN_RPAREN};
    }
}

Token get_next_token(Lexer *lexer) {
    while (lexer->current_char != '\0') {
        if (isspace(lexer->current_char)) {
            skip_whitespace(lexer);
            continue;
        }

        if (isdigit(lexer->current_char) || lexer->current_char == '.') {
            return process_number(lexer);
        }

        if (isalpha(lexer->current_char)) {
            return process_identifier(lexer);
        }
        if (is_operator(lexer->current_char)) {
            return process_operator(lexer);
        }
        if (is_bracket(lexer->current_char)) {
            return process_bracket(lexer);
        }

        error_exit("unknown character", lexer);
    }

    return (Token){TOKEN_EOF};
}
