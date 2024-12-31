#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

// Lexer structure to store the current state of lexical analysis
typedef struct Lexer {
    const char *text; // Input text being parsed
    size_t pos; // Current position in the text
    char current_char; // Current character being analyzed
    int amount_of_left_brackets;
    int amount_of_right_brackets;
} Lexer;

// Token types
typedef enum TokenType {
    TOKEN_NUM, // Numeric token
    TOKEN_ID, // Identifier token (e.g., variable names)
    TOKEN_FUNC, // Function name token
    TOKEN_PLUS, // '+' operator token
    TOKEN_MINUS, // '-' operator token
    TOKEN_MUL, // '*' operator token
    TOKEN_DIV, // '/' operator token
    TOKEN_LPAREN, // Left parenthesis token '('
    TOKEN_RPAREN, // Right parenthesis token ')'
    TOKEN_POW, // '^' operator token (for exponentiation)
    TOKEN_EOF // End of input token
} TokenType;

// Token structure
typedef struct Token {
    TokenType type; // Type of token
    union {
        double num; // For numeric values
        char id[64]; // For variable identifiers
        char func[10]; // For function names
    };
} Token;


// Function declarations
Lexer *initialize_lexer(const char *text);

void advance(Lexer *lexer);

void skip_whitespace(Lexer *lexer);

Token get_next_token(Lexer *lexer);

Token process_number(Lexer *lexer);

Token process_identifier(Lexer *lexer);

Token process_operator(Lexer *lexer);

Token process_bracket(Lexer *lexer);

int is_operator(char c);

int is_bracket(char c);

int areBracketsBalanced(const char *expression);

void error_exit(const char *message, Lexer *lexer);

#endif // LEXER_H
