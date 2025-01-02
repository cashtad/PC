#include "lexer.h"

Lexer *initialize_lexer(const char *text) {
    if (!are_brackets_balanced(text)) {
        error_exit(ERROR_BRACKETS_TEXT, ERROR_FUNCTION);
    }
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->text = text;
    lexer->pos = 0;
    lexer->current_char = text[0];

    return lexer;
}

int are_brackets_balanced(const char *expression) {
    int top = 0;

    for (int i = 0; expression[i] != END_OF_FILE; i++) {
        const char current = expression[i];

        if (current == LEFT_PAREN) {
            top++;
        } else if (current == RIGHT_PAREN) {
            if (top == 0) {
                return 0;
            }
            top--;
        }
    }
    return top == 0;
}

void advance(Lexer *lexer) {
    lexer->pos++;
    if (lexer->pos < strlen(lexer->text)) {
        lexer->current_char = lexer->text[lexer->pos];
    } else {
        lexer->current_char = END_OF_FILE;
    }
}

void skip_whitespace(Lexer *lexer) {
    while (lexer->current_char != END_OF_FILE && isspace(lexer->current_char)) {
        advance(lexer);
    }
}

Token process_number(Lexer *lexer) {
    Token token = {TOKEN_NUM, .num = 0.0};
    double fraction = 1.0;
    int is_fraction = 0;

    // For lexing int and float numbers
    while (isdigit(lexer->current_char) || lexer->current_char == DOT) {
        if (lexer->current_char == DOT) {
            if (is_fraction) {
                return (Token){TOKEN_ERROR};
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

    // For exponential part
    if (lexer->current_char == EXPONENT_SIGN_CAP || lexer->current_char == EXPONENT_SIGN) {
        advance(lexer);
        int exponent_sign = 1;

        if (lexer->current_char == PLUS || lexer->current_char == MINUS) {
            if (lexer->current_char == MINUS) {
                exponent_sign = -1;
            }
            advance(lexer);
        }

        if (!isdigit(lexer->current_char)) {
            return (Token){TOKEN_ERROR};
        }

        int exponent = 0;
        while (isdigit(lexer->current_char)) {
            exponent = exponent * 10 + (lexer->current_char - '0');
            advance(lexer);
        }

        if (lexer->current_char == DOT) {
            return (Token){TOKEN_ERROR};
        }
        if (isalpha(lexer->current_char)) {
            return (Token){TOKEN_ERROR};
        }
        if (lexer->current_char == LEFT_PAREN) {
            return (Token){TOKEN_ERROR};
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
        if (len < MAX_IDENTIFIER_LENGTH) {
            token.func[len++] = lexer->current_char;
        }
        advance(lexer);
    }
    if (len >= MAX_IDENTIFIER_LENGTH) {
        return (Token){TOKEN_ERROR};
    }
    token.func[len] = END_OF_FILE;
    if (strcmp(token.func, X) == 0) {
        token.type = TOKEN_ID;
        return token;
    }

    if (strcmp(token.func, COS) == 0 ||
        strcmp(token.func, SIN) == 0 ||
        strcmp(token.func, TAN) == 0 ||
        strcmp(token.func, ABS) == 0 ||
        strcmp(token.func, LN) == 0 ||
        strcmp(token.func, LOG) == 0 ||
        strcmp(token.func, ASIN) == 0 ||
        strcmp(token.func, ACOS) == 0 ||
        strcmp(token.func, ATAN) == 0 ||
        strcmp(token.func, SINH) == 0 ||
        strcmp(token.func, COSH) == 0 ||
        strcmp(token.func, TANH) == 0 ||
        strcmp(token.func, EXP) == 0) {
        token.type = TOKEN_FUNC;
        return token;
    }
    return (Token){TOKEN_ERROR};
}


int is_operator(const char c) {
    return c == PLUS || c == MINUS || c == MULT || c == DIVISION || c == POWER;
}

Token process_operator(Lexer *lexer) {
    const char operator = lexer->current_char;
    advance(lexer);
    switch (operator) {
        case PLUS: return (Token){TOKEN_PLUS};
        case MINUS: return (Token){TOKEN_MINUS};
        case MULT: return (Token){TOKEN_MUL};
        case DIVISION: return (Token){TOKEN_DIV};
        case POWER: return (Token){TOKEN_POW};
        default: return (Token){TOKEN_ERROR};
    }
}

int is_bracket(const char c) {
    return c == LEFT_PAREN || c == RIGHT_PAREN;
}

Token process_bracket(Lexer *lexer) {
    if (lexer->current_char == LEFT_PAREN) {
        advance(lexer);
        return (Token){TOKEN_LPAREN};
    }
    advance(lexer);

    return (Token){TOKEN_RPAREN};
}

Token get_next_token(Lexer *lexer) {
    while (lexer->current_char != END_OF_FILE) {
        if (isspace(lexer->current_char)) {
            skip_whitespace(lexer);
            continue;
        }

        if (isdigit(lexer->current_char) || lexer->current_char == DOT) {
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
        return (Token){TOKEN_ERROR};
    }
    return (Token){TOKEN_ERROR};
}
