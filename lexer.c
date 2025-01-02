#include "lexer.h"


/**
 * @brief Initializes a lexer for tokenizing a given text.
 *
 * This function allocates memory for a new lexer, sets its initial state, and checks
 * if the brackets in the provided text are balanced. If the brackets are not balanced,
 * the program will terminate with an error message.
 *
 * @param text The text to be tokenized by the lexer.
 * @return A pointer to the initialized Lexer.
 * @throws Exits the program if the brackets in the text are not balanced.
*/
Lexer *initialize_lexer(const char *text) {
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!are_brackets_balanced(text)) {
        error_exit(ERROR_BRACKETS_TEXT, 2);
    }
    lexer->text = text;
    lexer->pos = 0;
    lexer->current_char = text[0];

    return lexer;
}

/**
 * @brief Checks if the brackets in the given expression are balanced.
 *
 * This function iterates through the characters of the input expression, using a counter
 * to track opening and closing brackets. It returns 1 if the brackets are balanced
 * (i.e., each opening bracket has a corresponding closing bracket in the correct order),
 * and 0 if they are not.
 *
 * @param expression The expression to be checked for balanced brackets.
 * @return 1 if the brackets are balanced, 0 otherwise.
 */
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

/**
 * @brief Advances the lexer to the next character in the text.
 *
 * This function increments the lexer’s position and updates the current character
 * to the next character in the text. If the end of the text is reached, the current
 * character is set to the null character (END_OF_FILE) to indicate the end of the input.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character.
*/
void advance(Lexer *lexer) {
    lexer->pos++;
    if (lexer->pos < strlen(lexer->text)) {
        lexer->current_char = lexer->text[lexer->pos];
    } else {
        lexer->current_char = END_OF_FILE;
    }
}

/**
 * @brief Skips over any whitespace characters in the text.
 *
 * This function advances the lexer through the text, skipping all whitespace characters
 * (such as spaces, tabs, and newlines), until a non-whitespace character or the end of
 * the text is reached.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character.
 */
void skip_whitespace(Lexer *lexer) {
    while (lexer->current_char != END_OF_FILE && isspace(lexer->current_char)) {
        advance(lexer);
    }
}

/**
 * @brief Processes a number token from the lexer input.
 *
 * This function reads characters from the lexer input, processes a number (including
 * decimal and scientific notation), and returns a token representing the number.
 * It supports both integer and floating-point numbers, as well as exponential notation
 * (e.g., 1.23e+10). If the input is malformed (e.g., multiple decimal points, invalid exponents),
 * the program will exit with an error message.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character in the input text.
 * @return A Token representing the number parsed from the input.
 * @throws Exits the program if the input number or exponent is invalid.
*/
Token process_number(Lexer *lexer) {
    Token token = {TOKEN_NUM, .num = 0.0};
    double fraction = 1.0;
    int is_fraction = 0;

    // For lexing int and float numbers
    while (isdigit(lexer->current_char) || lexer->current_char == DOT) {
        if (lexer->current_char == DOT) {
            if (is_fraction) {
                error_exit(ERROR_NUMBER_TEXT, 2);
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
            error_exit(ERROR_EXPONENT_TEXT, 2);
        }

        int exponent = 0;
        while (isdigit(lexer->current_char)) {
            exponent = exponent * 10 + (lexer->current_char - '0');
            advance(lexer);
        }

        if (lexer->current_char == DOT) {
            error_exit(ERROR_EXPONENT_TEXT, 2);
        }
        if (isalpha(lexer->current_char)) {
            error_exit(ERROR_EXPONENT_TEXT, 2);
        }
        if (lexer->current_char == LEFT_PAREN) {
            error_exit(ERROR_EXPONENT_TEXT, 2);
        }
        if (exponent != 0) {
            token.num *= pow(10, exponent_sign * exponent);
        }
    }

    return token;
}

/**
 * @brief Processes an identifier token from the lexer input.
 *
 * This function reads characters from the lexer input to identify a valid function or variable name.
 * If the identifier is a recognized mathematical function (e.g., "cos", "sin", "tan"), it is classified
 * as a function token. If the identifier is "x", it is treated as a variable identifier. If the identifier
 * is unknown or exceeds the allowed length, an error is raised and the program exits.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character in the input text.
 * @return A Token representing the identifier or function found in the input.
 * @throws Exits the program if the identifier is unknown, too long, or otherwise invalid.
*/
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
        error_exit(ERROR_IDENTIFIER_TEXT, 2);
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
    error_exit(ERROR_IDENTIFIER_TEXT, 2);

    token.type = TOKEN_ID;
    return token; // never reached, but required for compiler to know that the function returns a value
}

/**
 * @brief Checks if a character is a valid operator.
 *
 * This function checks whether the given character is one of the valid mathematical operators:
 * addition ('+'), subtraction ('-'), multiplication ('*'), division ('/'), or exponentiation ('^').
 *
 * @param c The character to be checked.
 * @return 1 if the character is a valid operator, 0 otherwise.
*/
int is_operator(const char c) {
    return c == PLUS || c == MINUS || c == MULT || c == DIVISION || c == POWER;
}

/**
 * @brief Processes an operator token from the lexer input.
 *
 * This function identifies and returns a token corresponding to a mathematical operator
 * such as addition, subtraction, multiplication, division, or exponentiation. If the
 * operator is not recognized, the function will exit the program with an error message.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character in the input text.
 * @return A Token representing the operator found in the input.
 * @throws Exits the program if the operator is unknown.
*/
Token process_operator(Lexer *lexer) {
    const char operator = lexer->current_char;
    advance(lexer);
    switch (operator) {
        case PLUS: return (Token){TOKEN_PLUS};
        case MINUS: return (Token){TOKEN_MINUS};
        case MULT: return (Token){TOKEN_MUL};
        case DIVISION: return (Token){TOKEN_DIV};
        case POWER: return (Token){TOKEN_POW};
        default: error_exit(ERROR_UNKNOWN_OPERATOR_TEXT, 2);
    }
    return (Token){TOKEN_EOF}; // never reached, but required for compiler to know that the function returns a value
}

/**
 * @brief Checks if a character is a bracket.
 *
 * This function checks whether the given character is a left or right parenthesis ('(' or ')').
 *
 * @param c The character to be checked.
 * @return 1 if the character is a bracket, 0 otherwise.
*/
int is_bracket(const char c) {
    return c == LEFT_PAREN || c == RIGHT_PAREN;
}

/**
 * @brief Processes a bracket token from the lexer input.
 *
 * This function checks the current character to determine if it is a left or right parenthesis ('(' or ')'),
 * advances the lexer to the next character, and returns the corresponding token for the parenthesis.
 * If the current character is a left parenthesis, it returns a `TOKEN_LPAREN` token; if it is a right
 * parenthesis, it returns a `TOKEN_RPAREN` token.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character in the input text.
 * @return A Token representing either a left parenthesis (TOKEN_LPAREN) or a right parenthesis (TOKEN_RPAREN).
*/
Token process_bracket(Lexer *lexer) {
    if (lexer->current_char == LEFT_PAREN) {
        advance(lexer);
        return (Token){TOKEN_LPAREN};
    }
    advance(lexer);
    return (Token){TOKEN_RPAREN};
}

/**
 * @brief Retrieves the next token from the lexer input.
 *
 * This function examines the current character in the lexer input and determines its type
 * (whitespace, number, identifier, operator, or bracket). It processes the character accordingly,
 * skipping whitespace, parsing numbers or identifiers, and returning the corresponding token.
 * If an unrecognized character is encountered, the program will exit with an error message.
 * The function continues processing until the end of the input is reached, returning an
 * end-of-file (EOF) token when no more tokens are available.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character in the input text.
 * @return A Token representing the next token in the input. Returns a `TOKEN_EOF` token at the end of the input.
 * @throws Exits the program if an unknown character is encountered.
*/
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

        error_exit(ERROR_UNKNOWN_CHARACTER, 2);
    }

    return (Token){TOKEN_EOF};
}
