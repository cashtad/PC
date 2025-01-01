#include "draw_utils.h"


/**
 * @brief Main function to generate a graphical plot of a mathematical expression.
 *
 * This program accepts a mathematical expression as a single-variable function and generates a graph in PostScript format. It supports optional user-defined limits for the x and y axes.
 *
 * The function performs the following steps:
 * - Validates the command-line arguments.
 * - If no limits are provided, it uses the default limits of [-10, 10] for both x and y axes.
 * - If limits are provided, they are parsed and validated.
 * - It initializes a lexer to process the mathematical expression.
 * - The expression is parsed into an abstract syntax tree (AST).
 * - The graph is drawn based on the parsed expression and limits.
 * - Outputs to a PostScript (.ps) file.
 * - The memory allocated for the AST is freed before exiting.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * - argv[1]: The mathematical expression to plot.
 * - argv[2]: The output file where the graph will be saved.
 * - argv[3] (optional): A string representing the limits for the graph in the format "xmin:xmax:ymin:ymax".
 *
 * @return 0 if the program executed successfully, or an error code if an issue occurred.
 */
int main(const int argc, char *argv[]) {
    // Necessary arguments check
    if (argc < 3) {
        error_exit(
            "invalid input. Correct usage: <func> <out-file> [<limits>].\nEnsure the function is a single-variable mathematical expression and enclosed in quotes if it contains spaces.\n",
            1);
    }

    const char *expression = argv[1];
    const char *output_file_name = argv[2];

    printf("Expression: %s\n", expression);
    // Default values for limits
    Limits *limits = initialize_limits();


    // If limits were defined in arguments
    if (argc == 4) {
        if (parse_limits(argv[3], limits) == 1) {
            error_exit(
                "while parsing limits string.\nCorrect usage: ⟨xmin⟩:⟨xmax⟩:⟨ymin⟩:⟨ymax⟩\nEnsure that xmin < xmax and ymin < ymax",
                4);
        }
    }


    // Open .ps file for write mode
    FILE *output_file = fopen(output_file_name, "w");
    if (!output_file) {
        error_exit("unable to open output file", 3);
    }


    Lexer *lexer = initialize_lexer(expression);

    Node *abstract_syntax_tree = parse_expr(lexer);

    free(lexer);

    draw_graph(limits, output_file, abstract_syntax_tree);

    free_node(abstract_syntax_tree);

    free(limits);

    fclose(output_file);


    return 0;
}
