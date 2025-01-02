#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <math.h>
#include <string.h>
#include "err.h"

/**
 * @brief Defines string constants for mathematical functions.
 *
 * These constants represent commonly used mathematical functions in string format,
 * which can be matched against function identifiers in an expression.
 */
#define SIN "sin"
#define COS "cos"
#define TAN "tan"
#define ABS "abs"
#define LN "ln"
#define LOG "log"
#define ASIN "asin"
#define ACOS "acos"
#define ATAN "atan"
#define SINH "sinh"
#define COSH "cosh"
#define TANH "tanh"
#define EXP "exp"

/**
 * @brief Defines characters for basic mathematical operators.
 *
 * These constants represent basic operators used in mathematical expressions.
 * They are utilized during tokenization and parsing operations.
 */
#define MINUS_UN '-'
#define PLUS '+'
#define MINUS '-'
#define MULT '*'
#define DIVISION '/'
#define POWER '^'

/**
 * @brief Defines characters for parentheses and dot (decimal point).
 *
 * These constants represent characters used for grouping expressions and representing decimal numbers.
 */
#define LEFT_PAREN '('
#define RIGHT_PAREN ')'
#define DOT '.'

/**
 * @brief Defines the character for the variable 'x' used in mathematical functions.
 *
 * The 'x' character is commonly used as the independent variable in mathematical expressions.
 */
#define X "x"

/**
 * @brief Defines the end-of-file character.
 *
 * This character marks the end of the input string and is used for detecting the end of tokenization.
 */
#define END_OF_FILE '\0'

/**
 * @brief Defines characters for exponent notation in numbers.
 *
 * These constants represent the characters used for specifying the exponent part in scientific notation.
 */
#define EXPONENT_SIGN 'e'
#define EXPONENT_SIGN_CAP 'E'

/**
 * @brief Defines the maximum length for function identifiers.
 *
 * This constant limits the maximum length of a function identifier in the lexer to avoid buffer overflow.
 */
#define MAX_IDENTIFIER_LENGTH 64

/**
 * @brief Represents a lexer for tokenizing mathematical expressions.
 *
 * This structure holds the state of the lexer, including the text to be tokenized,
 * the current position in the text, and the current character being processed.
 */
typedef struct Lexer {
    /**
     * @brief Pointer to the text being tokenized.
     *
     * This points to the input string that is being analyzed by the lexer.
     */
    const char *text;

    /**
     * @brief Current position in the text.
     *
     * This is the index that indicates the current character being processed within the input text.
     */
    size_t pos;

    /**
     * @brief Current character being processed.
     *
     * This is the character currently being examined in the text at the position specified by `pos`.
     */
    char current_char;
} Lexer;


/**
 * @brief Enum representing different types of tokens.
 *
 * This enumeration defines the possible types of tokens that can be encountered
 * while lexing a mathematical expression.
 */
typedef enum TokenType {
    /**
     * @brief Token type for numeric values.
     *
     * Represents a token that corresponds to a number in the expression.
     */
    TOKEN_NUM,

    /**
     * @brief Token type for identifiers (variables or constants).
     *
     * Represents a token that corresponds to an identifier, such as a variable or constant.
     */
    TOKEN_ID,

    /**
     * @brief Token type for function names.
     *
     * Represents a token that corresponds to a function name, such as "sin", "cos", etc.
     */
    TOKEN_FUNC,

    /**
     * @brief Token type for the addition operator.
     *
     * Represents a token that corresponds to the addition operator '+' in the expression.
     */
    TOKEN_PLUS,

    /**
     * @brief Token type for the subtraction operator.
     *
     * Represents a token that corresponds to the subtraction operator '-' in the expression.
     */
    TOKEN_MINUS,

    /**
     * @brief Token type for the multiplication operator.
     *
     * Represents a token that corresponds to the multiplication operator '*' in the expression.
     */
    TOKEN_MUL,

    /**
     * @brief Token type for the division operator.
     *
     * Represents a token that corresponds to the division operator '/' in the expression.
     */
    TOKEN_DIV,

    /**
     * @brief Token type for the left parenthesis.
     *
     * Represents a token that corresponds to the opening parenthesis '(' in the expression.
     */
    TOKEN_LPAREN,

    /**
     * @brief Token type for the right parenthesis.
     *
     * Represents a token that corresponds to the closing parenthesis ')' in the expression.
     */
    TOKEN_RPAREN,

    /**
     * @brief Token type for the exponentiation operator.
     *
     * Represents a token that corresponds to the exponentiation operator '^' in the expression.
     */
    TOKEN_POW,

    /**
     * @brief Token type for errors.
     *
     * Represents a token that indicates an error in the expression, such as an invalid character.
     */
    TOKEN_ERROR
} TokenType;

/**
 * @brief Struct representing a token.
 *
 * This structure is used to represent a single token identified during the lexing process.
 * It contains the token type and, depending on the type, stores the relevant value (number, identifier, or function).
 */
typedef struct Token {
    /**
     * @brief Type of the token.
     *
     * This field stores the type of the token, which indicates whether it is a number, identifier,
     * function, operator, parenthesis, or error.
     */
    TokenType type;

    /**
     * @brief Union for storing token values.
     *
     * This union contains the values associated with the token, depending on its type:
     * - num: The numeric value if the token represents a number.
     * - id: The identifier string if the token represents an identifier (variable or constant).
     * - func: The function name string if the token represents a mathematical function.
     */
    union {
        double num; /**< The numeric value if the token is a number. */
        char id[MAX_IDENTIFIER_LENGTH]; /**< The identifier name if the token is an identifier. */
        char func[MAX_IDENTIFIER_LENGTH]; /**< The function name if the token is a function. */
    };
} Token;


/**
 * @brief Initializes a lexer for tokenizing a mathematical expression.
 *
 * This function checks if the brackets in the expression are balanced. If not,
 * it exits the program with an error message. Then, it initializes a Lexer structure
 * to begin the tokenization process.
 *
 * @param text The mathematical expression to be tokenized.
 * @return A pointer to the initialized Lexer structure.
 */
Lexer *initialize_lexer(const char *text);

/**
 * @brief Advances the lexer to the next character in the text.
 *
 * This function increments the lexer’s position and updates the current character
 * to the next character in the text. If the end of the text is reached, the current
 * character is set to the null character ('\0') to indicate the end of the input.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character.
 */
void advance(Lexer *lexer);

/**
 * @brief Skips over any whitespace characters in the text.
 *
 * This function advances the lexer through the text, skipping all whitespace characters
 * (such as spaces, tabs, and newlines), until a non-whitespace character or the end of
 * the text is reached.
 *
 * @param lexer A pointer to the lexer that tracks the current position and character.
 */
void skip_whitespace(Lexer *lexer);

/**
 * @brief Processes a number (integer or floating-point) from the expression.
 *
 * This function lexes both integer and floating-point numbers (including scientific notation) from the input expression.
 * It handles fractional parts and exponent notation, returning a token with the correctly parsed numeric value.
 *
 * @param lexer A pointer to the lexer containing the current position in the expression.
 * @return A token representing the numeric value (integer or floating-point) parsed from the expression, or an error token if the number is malformed.
 */
Token process_number(Lexer *lexer);

/**
 * @brief Processes an identifier from the expression, such as a variable or a function name.
 *
 * This function lexes an identifier, which can either be a variable (e.g., "x") or a function name (e.g., "cos", "sin").
 * It checks if the identifier corresponds to a predefined variable (like "x") or a known function (like "cos", "sin").
 * If the identifier is valid, it returns the appropriate token.
 *
 * @param lexer A pointer to the lexer containing the current position in the expression.
 * @return A token representing the identifier, either a variable (TOKEN_ID) or a function (TOKEN_FUNC), or an error token if the identifier is not recognized.
 */
Token process_identifier(Lexer *lexer);

/**
 * @brief Processes an operator from the expression.
 *
 * This function checks the current character to identify which mathematical operator is present
 * (e.g., `+`, `-`, `*`, `/`, `^`). It then advances the lexer and returns the corresponding token type
 * based on the operator.
 *
 * @param lexer A pointer to the lexer containing the current position in the expression.
 * @return The token representing the operator (e.g., TOKEN_PLUS, TOKEN_MINUS, etc.), or an error token if the character is not a valid operator.
 */
Token process_operator(Lexer *lexer);

/**
 * @brief Processes a bracket from the expression.
 *
 * This function checks whether the current character is a left parenthesis (`(`) or right parenthesis (`)`),
 * and returns the corresponding token type (LPAREN or RPAREN). It also advances the lexer position after processing the bracket.
 *
 * @param lexer A pointer to the lexer containing the current position in the expression.
 * @return The token representing either a left parenthesis (LPAREN) or a right parenthesis (RPAREN).
 */
Token process_bracket(Lexer *lexer);

/**
 * @brief Checks if a character is a valid operator.
 *
 * This function checks whether the given character is one of the valid mathematical operators:
 * addition ('+'), subtraction ('-'), multiplication ('*'), division ('/'), or exponentiation ('^').
 *
 * @param c The character to be checked.
 * @return 1 if the character is a valid operator, 0 otherwise.
 */
int is_operator(char c);

/**
 * @brief Checks if a character is a bracket.
 *
 * This function checks whether the given character is a left or right parenthesis ('(' or ')').
 *
 * @param c The character to be checked.
 * @return 1 if the character is a bracket, 0 otherwise.
 */
int is_bracket(char c);

/**
 * @brief Checks if the brackets in the given expression are balanced.
 *
 * This function iterates through the characters of the input expression, using a stack
 * to track opening and closing brackets. It returns 1 if the brackets are balanced
 * (i.e., each opening bracket has a corresponding closing bracket in the correct order),
 * and 0 if they are not.
 *
 * @param expression The expression to be checked for balanced brackets.
 * @return 1 if the brackets are balanced, 0 otherwise.
 */
int are_brackets_balanced(const char *expression);

/**
 * @brief Retrieves the next token from the expression.
 *
 * This function scans the expression character by character to identify and return the next token.
 * It handles different types of tokens, including numbers, identifiers, operators, and brackets.
 * It skips over any whitespace characters.
 * If an unrecognized character is encountered, it returns an error token.
 *
 * @param lexer A pointer to the lexer containing the expression text and current position.
 * @return The next token in the expression.
 */
Token get_next_token(Lexer *lexer);


#endif // LEXER_H
