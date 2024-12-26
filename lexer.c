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
                    if (is_fraction) {
                        fprintf(stderr, "Error: wrong function input.");
                        exit(2);
                    }
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

            // Handle exponent part (E or e)
            if (lexer->current_char == 'E' || lexer->current_char == 'e') {
                advance(lexer); // Skip E or e
                int exponent_sign = 1; // Assume positive exponent by default

                // Handle optional sign for the exponent
                if (lexer->current_char == '+' || lexer->current_char == '-') {
                    if (lexer->current_char == '-') {
                        exponent_sign = -1; // Negative exponent
                    }
                    advance(lexer); // Skip the sign
                }

                // Handle the digits of the exponent
                if (!isdigit(lexer->current_char)) {
                    // If no digits follow the E or e, it's an error
                    fprintf(stderr, "Error: wrong exp usage.");
                    exit(2);
                }

                int exponent = 0;
                while (isdigit(lexer->current_char)) {
                    exponent = exponent * 10 + (lexer->current_char - '0');
                    advance(lexer);
                }

                // After processing digits, check for any additional characters (e.g., a decimal point)
                if (lexer->current_char == '.') {
                    fprintf(stderr, "Error: wrong exp usage. Exponent cannot be a floating point number.\n");
                    exit(2);
                }
                if (isalpha(lexer->current_char)) {
                    fprintf(stderr, "Error: wrong exp usage. Exponent cannot contain letters.\n");
                    exit(2);
                }

                // Apply the exponent to the number
                while (exponent > 0) {
                    if (exponent_sign > 0) {
                        token.num *= 10;
                    } else {
                        token.num /= 10;
                    }
                    exponent--;
                }
            }

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
            }
            if (strcmp(token.func, "x") == 0) {
                token.type = TOKEN_ID;
                //printf("Token: ID(%s)\n", token.func);
                return token;
            } else {
                fprintf(stderr, "Error: unknown token: '%s'\n", token.func);
                exit(2);
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
        exit(2);
    }

    //printf("Token: EOF\n");
    return (Token){TOKEN_EOF};
}
