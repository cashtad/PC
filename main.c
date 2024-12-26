#include <stdio.h>
#include <stdlib.h>

#include "draw_utils.h"
#include "lexer.h"
#include "parser.h"


int parse_limits(const char *limits_str, Limits *limits) {
    char *endptr;

    limits->x_min = strtod(limits_str, &endptr);
    if (*endptr != ':') return 1;

    limits->x_max = strtod(endptr + 1, &endptr);
    if (*endptr != ':') return 1;

    limits->y_min = strtod(endptr + 1, &endptr);
    if (*endptr != ':') return 1;

    limits->y_max = strtod(endptr + 1, &endptr);
    if (*endptr != '\0') return 1;

    if (limits->x_min > limits->x_max) return 1;
    if (limits->y_min > limits->y_max) return 1;

    return 0; // Success
}


// Main function
int main(const int argc, char *argv[]) {

    // Necessary arguments check
    if (argc < 3) {
        fprintf(stderr, "Invalid input. Correct usage: %s <func> <out-file> [<limits>].\nEnsure the function is a single-variable mathematical expression and enclosed in quotes if it contains spaces.\n", argv[0]);
        return 1;
    }

    const char *expression = argv[1];
    const char *output_file = argv[2];
    // Default values for limits
    Limits limits = {-10, 10, -10, 10};

    // If limits were defined in arguments
    if (argc == 4) {
        if (parse_limits(argv[3], &limits) == 1) {
            printf("Error parsing limits string.\nCorrect usage: ⟨xmin⟩:⟨xmax⟩:⟨ymin⟩:⟨ymax⟩\nEnsure that xmin < xmax and ymin < ymax");
            return 4;
        }
    }

    printf("Expression: %s\n", expression);
    printf("Output file: %s\n", output_file);


    // Открываем .ps файл для записи
    FILE *file = fopen(output_file, "w");
    if (!file) {
        fprintf(stderr, "Unable to open output file: %s\n", output_file);
        return 3;
    }


    Lexer *lexer = create_lexer(expression);

    Node *abstract_syntax_tree = parse_expr(lexer);

    free(lexer);

    draw_graph(&limits, file, abstract_syntax_tree);

    free_node(abstract_syntax_tree);


    fclose(file);


    return 0;
}
