#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

// Lexer structure to store the current state of lexical analysis
typedef struct Lexer {
    const char* text;  // Input text being parsed
    size_t pos;        // Current position in the text
    char current_char; // Current character being analyzed
} Lexer;

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

Lexer* create_lexer(const char* text);
void advance(Lexer* lexer);
void skip_whitespace(Lexer* lexer);
Token get_next_token(Lexer* lexer);

#endif // LEXER_H
