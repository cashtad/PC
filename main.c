#include "draw_utils.h"

/**
 * @brief Static variables used for storing global states in the program.
 */

/**
 * @brief Pointer to the limits of the graph.
 *
 * This structure holds the limits for the graph axes (x and y), which define the range of values
 * to be displayed on the graph. It ensures that the graph is rendered within the appropriate
 * coordinate space.
 */
static Limits *limits;

/**
 * @brief Pointer to the output file.
 *
 * This variable holds a pointer to the file where the graph or evaluation results will be written.
 * The file will be used for output in formats like PostScript (.ps).
 */
static FILE *output_file;

/**
 * @brief Pointer to the lexer.
 *
 * The lexer is responsible for breaking the input expression into tokens for parsing. This pointer
 * is used globally to maintain the state of the lexer throughout the program.
 */
static Lexer *lexer;

/**
 * @brief Pointer to the abstract syntax tree (AST).
 *
 * The AST is used to represent the structure of the mathematical expression, where each node
 * corresponds to an operator, operand, or function. It is used by the evaluator to compute function values.
 */
static Node *abstract_syntax_tree;

/**
 * @brief Cleans up the allocated memory and resources.
 *
 * This function ensures proper deallocation of memory and closure of file resources.
 * It frees the lexer, abstract syntax tree, limits, and closes the output file if they were
 * previously allocated or opened. This helps prevent memory leaks and file handle issues.
 */
void cleanup() {
    if (lexer) {
        free(lexer);
    }
    if (abstract_syntax_tree) {
        free_node(abstract_syntax_tree);
    }

    if (limits) {
        free(limits);
    }
    if (output_file) {
        fclose(output_file);
    }
}

/**
 * @brief Main function for parsing an expression, evaluating it, and generating a graphical representation.
 *
 * The program expects the following command-line arguments:
 * - The mathematical expression to be parsed and evaluated.
 * - The output file name where the graphical representation will be saved.
 * - Optional argument: A string defining the limits for the graph (in the form of x_min,x_max,y_min,y_max).
 *
 * The program performs the following tasks:
 * 1. Initializes limits for the graph.
 * 2. Parses the expression into an abstract syntax tree.
 * 3. Generates a graphical representation of the function in a .ps (PostScript) file.
 * 4. Handles resource cleanup at program termination.
 *
 * @param argc The number of arguments passed to the program.
 * @param argv An array of strings representing the arguments passed to the program.
 * @return Returns 0 if the program executes successfully.
 */
int main(const int argc, char *argv[]) {
    atexit(cleanup);
    // Necessary arguments check
    if (argc < 3) {
        error_exit(ERROR_ARGS_TEXT,ERROR_ARGS);
    }

    const char *expression = argv[1];
    const char *output_file_name = argv[2];

    // Default values for limits
    limits = initialize_limits();

    // If limits were defined in arguments
    if (argc == 4) {
        if (parse_limits(argv[3], limits) == 1) {
            error_exit(ERROR_LIMITS_TEXT,ERROR_LIMITS);
        }
    }

    // Open .ps file for write mode
    output_file = fopen(output_file_name, "w");
    if (!output_file) {
        error_exit(ERROR_FILE_TEXT, ERROR_FILE);
    }

    lexer = initialize_lexer(expression);

    abstract_syntax_tree = parse(lexer);

    draw_graph(limits, output_file, abstract_syntax_tree);

    return 0;
}
