#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "err.h"

/**
 * @brief Represents a lexer that tokenizes the input text.
 *
 * The `Lexer` structure holds the state of the lexer, which is responsible for breaking down
 * the input text into tokens. These tokens are then processed by the parser to build an abstract
 * syntax tree (AST). The lexer reads characters from the input string and identifies the
 * appropriate tokens (numbers, operators, functions, parentheses, etc.).
 *
 * The structure contains the following fields:
 * - `text`: A pointer to the input text that the lexer is parsing.
 * - `pos`: The current position (index) in the text being processed.
 * - `current_char`: The current character being analyzed in the input text.
 * - `amount_of_left_brackets`: A counter for the number of left parentheses encountered in the text.
 * - `amount_of_right_brackets`: A counter for the number of right parentheses encountered in the text.
 *
 * The lexer maintains these fields to efficiently traverse and analyze the input text character by character.
 * It uses this state information to detect tokens such as numbers, operators, functions, and parentheses.
 * It also ensures that parentheses are balanced and manages the position in the input text as tokens are processed.
 */
typedef struct Lexer {
    const char *text;
    size_t pos;
    char current_char;
} Lexer;

/**
 * @brief Enum representing different types of tokens that can be identified in the input text.
 *
 * The `TokenType` enumeration defines the various types of tokens that the lexer can recognize
 * when parsing the input expression. Each token corresponds to a specific type of character or
 * symbol that plays a role in the expression being parsed.
 *
 * The enumeration includes the following values:
 * - `TOKEN_NUM`: Represents a numeric token (e.g., numbers like 42, 3.14, 3E+5).
 * - `TOKEN_ID`: Represents an identifier token (e.g., variable names like 'x' or 'y').
 * - `TOKEN_FUNC`: Represents a function name token (e.g., mathematical functions like 'sin', 'cos').
 * - `TOKEN_PLUS`: Represents the '+' operator token, used for addition.
 * - `TOKEN_MINUS`: Represents the '-' operator token, used for subtraction.
 * - `TOKEN_MUL`: Represents the '*' operator token, used for multiplication.
 * - `TOKEN_DIV`: Represents the '/' operator token, used for division.
 * - `TOKEN_LPAREN`: Represents the left parenthesis token '('.
 * - `TOKEN_RPAREN`: Represents the right parenthesis token ')'.
 * - `TOKEN_POW`: Represents the '^' operator token, used for exponentiation.
 * - `TOKEN_EOF`: Represents the end of input token, indicating the lexer has finished parsing.
 *
 * These token types are used by the lexer to categorize the characters in the input string and
 * pass them to the parser for further processing, such as building an abstract syntax tree (AST).
 */
typedef enum TokenType {
    TOKEN_NUM,
    TOKEN_ID,
    TOKEN_FUNC,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_POW,
    TOKEN_EOF
} TokenType;

/**
 * @brief Struct representing a token in the input text.
 *
 * The `Token` structure is used to represent a single token, which is a categorized unit of
 * the input string processed by the lexer. Each token has a type, which is defined by the
 * `TokenType` enum, and depending on its type, it may contain additional data such as a number,
 * a variable identifier, or a function name.
 *
 * The structure includes the following fields:
 * - `type`: Specifies the type of token, which determines how the token should be interpreted.
 *          The type is one of the values from the `TokenType` enumeration (e.g., `TOKEN_NUM`,
 *          `TOKEN_ID`, `TOKEN_FUNC`).
 * - `num`: Holds the numeric value of the token when the type is `TOKEN_NUM`.
 * - `id`: Holds the string for the identifier (e.g., variable names) when the type is `TOKEN_ID`.
 * - `func`: Holds the string for the function name when the type is `TOKEN_FUNC`. It can store
 *           function names up to 9 characters (plus a null terminator).
 *
 * This structure is used by the lexer to pass information about the tokens to the parser.
 * Each token corresponds to a specific part of the input expression, such as a number,
 * an operator, or a function.
 */
typedef struct Token {
    TokenType type;

    union {
        double num;
        char id[64];
        char func[10];
    };
} Token;


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
Token get_next_token(Lexer *lexer);

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
Token process_number(Lexer *lexer);

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
Token process_identifier(Lexer *lexer);

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
Token process_operator(Lexer *lexer);

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
 * @brief Terminates the program with an error message.
 *
 * This function frees the memory occupied by the lexer, prints an error message
 * to the standard error stream, and terminates the program with an error code of 2.
 *
 * @param message The error message to be displayed.
 * @param lexer A pointer to the lexer whose memory will be freed before program termination.
 */
void error(const char *message, Lexer *lexer);

#endif // LEXER_H
