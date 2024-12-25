#include <stdio.h>
#include <stdlib.h>

#include "evaluator.h"
#include "lexer.h"
#include "parser.h"


typedef struct Limits {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
} Limits;


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
        fprintf(stderr, "Wrong parameters. Use as: %s <expression> <output_file> [limits]\n", argv[0]);
        return 1;
    }

    const char *expression = argv[1];
    const char *output_file = argv[2];
    Limits limits = {-10, 10, -10, 10}; // Default values

    // Проверка на наличие третьего параметра limits
    if (argc == 4) {
        if (parse_limits(argv[3], &limits) == 0) {
            printf("Limits: x_min=%.2f, x_max=%.2f, y_min=%.2f, y_max=%.2f\n",
                   limits.x_min, limits.x_max, limits.y_min, limits.y_max);
        } else {
            printf("Error parsing limits string.\n");
            return 1;
        }
    }

    printf("Expression: %s\n", expression);
    printf("Output file: %s\n", output_file);
    printf("Limits: x_min=%.2f, x_max=%.2f, y_min=%.2f, y_max=%.2f\n",
           limits.x_min, limits.x_max, limits.y_min, limits.y_max);


    // Открываем .ps файл для записи
    FILE *file = fopen(output_file, "w");
    if (!file) {
        fprintf(stderr, "Unable to open output file: %s\n", output_file);
        return 1;
    }


    const double page_width = 595.0; // Ширина страницы A4
    const double page_height = 842.0; // Высота страницы A4

    fprintf(file, "%%PageSetup\n");
    fprintf(file, "<< /PageSize [%f %f] >> setpagedevice\n", page_width, page_height);

    // Масштабирование
    const double scale_x = (page_width - 100) / (limits.x_max - limits.x_min); // Масштаб по оси X
    const double scale_y = (page_height - 100) / (limits.y_max - limits.y_min); // Масштаб по оси Y

    // Печать заголовка PostScript
    fprintf(file, "%%!PS\n");
    fprintf(file, "/inch {72 mul} def\n");

    // Сдвиг начала координат в центр страницы
    fprintf(file, "%f %f translate\n", page_width / 2, page_height / 2);

    // Инвертирование оси Y (для правильной ориентации графика)
    // fprintf(file, "1 -1 scale\n");

    fprintf(file, "1 0 0 setrgbcolor\n");


    fprintf(file, "%f %f moveto\n", limits.x_min * scale_x, 0.0);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x, 0.0);
    fprintf(file, "stroke\n");

    fprintf(file, "%f %f moveto\n", 0.0, limits.y_min * scale_y);
    fprintf(file, "%f %f lineto\n", 0.0, limits.y_max * scale_y);
    fprintf(file, "stroke\n");


    fprintf(file, "0 0 0 setrgbcolor\n");


    Lexer *lexer = create_lexer(expression);
    Node *abstract_syntax_tree = parse_expr(lexer);
    free(lexer);

    int first_point = 1;
    int out_of_range = 0;
    for (double x = limits.x_min; x <= limits.x_max; x += 0.01) {
        const double y = evaluate(abstract_syntax_tree, x);
        if (y > limits.y_max) {
            if (!out_of_range) {
                first_point = 1;
                out_of_range = 1;
                fprintf(file, "stroke\n");
            }
        } else {
            const double ps_x = x * scale_x; // Масштабированное значение x
            const double ps_y = y * scale_y; // Масштабированное значение y
            out_of_range = 0;
            if (first_point) {
                fprintf(file, "%f %f moveto\n", ps_x, ps_y);
                first_point = 0;
            } else {
                fprintf(file, "%f %f lineto\n", ps_x, ps_y);
            }
        }
    }

    free_node(abstract_syntax_tree);

    fprintf(file, "stroke\n");


    fprintf(file, "showpage\n");

    fclose(file);


    return 0;
}
