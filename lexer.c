//
// Created by lemal on 24.12.2024.
//

#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Lexer functions
Lexer *create_lexer(const char *text) {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->text = text;
    lexer->pos = 0;
    lexer->current_char = text[0];
    return lexer;
}

// Advances the lexer position by one character
void advance(Lexer *lexer) {
    lexer->pos++;
    if (lexer->pos < strlen(lexer->text)) {
        lexer->current_char = lexer->text[lexer->pos];
    } else {
        lexer->current_char = '\0'; // End of input
    }
}

// Skips whitespace characters in the input
void skip_whitespace(Lexer *lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
        advance(lexer);
    }
}

// Extracts the next token from the input stream
Token get_next_token(Lexer *lexer) {
    while (lexer->current_char != '\0') {
        //To skip whitespaces
        if (isspace(lexer->current_char)) {
            skip_whitespace(lexer);
            continue;
        }

        // Handle numbers (including floating point)
        if (isdigit(lexer->current_char) || lexer->current_char == '.') {
            Token token = {TOKEN_NUM, .num = 0.0};
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
            } else {
                token.type = TOKEN_ID;
                //printf("Token: ID(%s)\n", token.func);
                return token;
            }

        }

        // Handle operators and parentheses
        if (lexer->current_char == '+') {
            advance(lexer);
            //printf("Token: PLUS\n");
            return (Token){TOKEN_PLUS};
        }

        if (lexer->current_char == '-') {
            advance(lexer);
            //printf("Token: MINUS\n");
            return (Token){TOKEN_MINUS};
        }

        if (lexer->current_char == '*') {
            advance(lexer);
            //printf("Token: MUL\n");
            return (Token){TOKEN_MUL};
        }

        if (lexer->current_char == '/') {
            advance(lexer);
            //printf("Token: DIV\n");
            return (Token){TOKEN_DIV};
        }

        if (lexer->current_char == '(') {
            advance(lexer);
            //printf("Token: LPAREN\n");
            return (Token){TOKEN_LPAREN};
        }

        if (lexer->current_char == ')') {
            advance(lexer);
            //printf("Token: RPAREN\n");
            return (Token){TOKEN_RPAREN};
        }

        if (lexer->current_char == '^') {
            advance(lexer);
            return (Token){TOKEN_POW};
        }

        // Handle unknown characters
        fprintf(stderr, "Error: unknown character '%c'\n", lexer->current_char);
        exit(EXIT_FAILURE);
    }

    //printf("Token: EOF\n");
    return (Token){TOKEN_EOF};
}
