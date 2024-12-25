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


    const double page_width = 595.0; // Ширина страницы A4
    const double page_height = 842.0; // Высота страницы A4

    fprintf(file, "%%PageSetup\n");
    fprintf(file, "/Courier findfont 12 scalefont setfont\n");
    fprintf(file, "<< /PageSize [%f %f] >> setpagedevice\n", page_width, page_height);

    // Масштабирование
    const double scale_x = (page_width - 100) / (limits.x_max - limits.x_min); // Масштаб по оси X
    const double scale_y = (page_height - 100) / (limits.y_max - limits.y_min); // Масштаб по оси Y
    int const steps_x_to_right = (int) (limits.x_max);
    int const steps_x_to_left = abs((int) limits.x_min);
    int const steps_y_up = (int) (limits.y_max);
    int const steps_y_down = abs((int) (limits.y_min));

    // Печать заголовка PostScript
    fprintf(file, "%%!PS\n");
    fprintf(file, "/inch {72 mul} def\n");

    // Сдвиг начала координат в центр страницы
    fprintf(file, "%f %f translate\n", page_width / 2 - scale_x * (limits.x_max + limits.x_min) / 2,
            page_height / 2 - scale_y * (limits.y_max + limits.y_min) / 2);


    fprintf(file, "1 0 0 setrgbcolor\n");


    //X red line
    fprintf(file, "%f %f moveto\n", limits.x_min * scale_x - 25, 0.0);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x + 25, 0.0);
    fprintf(file, "stroke\n");
    //X arrow
    fprintf(file, "%f %f moveto\n", -5.0, limits.y_max * scale_y + 20);
    fprintf(file, "%f %f lineto\n", 0.0, limits.y_max * scale_y + 25);
    fprintf(file, "%f %f lineto\n", 5.0, limits.y_max * scale_y + 20);
    fprintf(file, "stroke\n");
    //X letter
    fprintf(file, "%f %f moveto\n", limits.x_max * scale_x + 20, -13.0);
    fprintf(file, "(x) show\n");


    //Y red line
    fprintf(file, "%f %f moveto\n", 0.0, limits.y_min * scale_y - 25);
    fprintf(file, "%f %f lineto\n", 0.0, limits.y_max * scale_y + 25);
    fprintf(file, "stroke\n");
    //Y arrow
    fprintf(file, "%f %f moveto\n", limits.x_max * scale_x + 20, 5.0);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x + 25, 0.0);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x + 20, -5.0);
    fprintf(file, "stroke\n");
    //Y letter
    fprintf(file, "%f %f moveto\n", 5.0, limits.y_max * scale_y + 20);
    fprintf(file, "(y) show\n");

    //Limits (blue lines)
    fprintf(file, "0 0 0.5 setrgbcolor\n"); // Полутон (50% серого)
    fprintf(file, "[5 15] 0 setdash\n"); // Штрих 5 пунктов, промежуток 3 пункта
    //RIGHT limit
    fprintf(file, "%f %f moveto\n", limits.x_max * scale_x, -page_height*2);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x, page_height*2);
    //LEFT limit
    fprintf(file, "%f %f moveto\n", limits.x_min * scale_x, -page_height*2);
    fprintf(file, "%f %f lineto\n", limits.x_min * scale_x, page_height*2);
    //TOP limit
    fprintf(file, "%f %f moveto\n", -page_width*2, limits.y_max * scale_y);
    fprintf(file, "%f %f lineto\n", page_width*2, limits.y_max * scale_y);
    //BOT limit
    fprintf(file, "%f %f moveto\n", -page_width*2, limits.y_min * scale_y);
    fprintf(file, "%f %f lineto\n", page_width*2, limits.y_min * scale_y);

    fprintf(file, "stroke\n");
    fprintf(file, "[] 0 setdash\n");


    //BLACK lines on x and y lines and grey cells
    for (int i = 0; i <= steps_x_to_right; i++) {
        //Grey lines
        if (i > 0 && i != steps_x_to_right) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", i * scale_x, -page_height * 2);
            fprintf(file, "%f %f lineto\n", i * scale_x, page_height * 2);
            fprintf(file, "stroke\n");
        }
        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", i * scale_x, 5.0);
        fprintf(file, "%f %f lineto\n", i * scale_x, -5.0);
        fprintf(file, "stroke\n");

        //Numbers
        fprintf(file, "%f %f moveto\n", i * scale_x - 3, -15.0);
        if (i > 0) {
            fprintf(file, "(%d) show\n", i);
        }
    }
    for (int i = -1; i >= -steps_x_to_left; i--) {
        //Grey lines
        if (i != -steps_x_to_left) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", i * scale_x, -page_height * 2);
            fprintf(file, "%f %f lineto\n", i * scale_x, page_height * 2);
            fprintf(file, "stroke\n");
        }
        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", i * scale_x, 5.0);
        fprintf(file, "%f %f lineto\n", i * scale_x, -5.0);
        fprintf(file, "stroke\n");

        //Numbers
        fprintf(file, "%f %f moveto\n", i * scale_x - 11, -15.0);
        fprintf(file, "(%d) show\n", i);
    }
    for (int i = 0; i <= steps_y_up; i++) {
        //Grey lines
        if (i > 0 && i != steps_y_up) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", -page_width * 2, i * scale_y);
            fprintf(file, "%f %f lineto\n", page_width * 2, i * scale_y);
            fprintf(file, "stroke\n");
        }

        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", -5.0, i * scale_y);
        fprintf(file, "%f %f lineto\n", 5.0, i * scale_y);
        fprintf(file, "stroke\n");

        //Numbers
        if (i > 0) {
            fprintf(file, "%f %f moveto\n", 6.0, i * scale_y - 3);
            fprintf(file, "(%d) show\n", i);
        }
    }
    for (int i = -1; i >= -steps_y_down; i--) {
        //Grey lines
        if (i != -steps_y_down) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", -page_width * 2, i * scale_y);
            fprintf(file, "%f %f lineto\n", page_width * 2, i * scale_y);
            fprintf(file, "stroke\n");
        }
        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", -5.0, i * scale_y);
        fprintf(file, "%f %f lineto\n", 5.0, i * scale_y);
        fprintf(file, "stroke\n");

        //Numbers
        fprintf(file, "%f %f moveto\n", 6.0, i * scale_y - 3);
        fprintf(file, "(%d) show\n", i);
    }


    Lexer *lexer = create_lexer(expression);
    Node *abstract_syntax_tree = parse_expr(lexer);
    free(lexer);

    int first_point = 1;
    int out_of_range = 0;
    for (double x = limits.x_min; x <= limits.x_max; x += 0.01) {
        const double y = evaluate(abstract_syntax_tree, x);
        if (y > limits.y_max || y < limits.y_min) {
            if (!out_of_range) {
                first_point = 1;
                out_of_range = 1;
                // const double ps_x = x * scale_x; // Масштабированное значение x
                // if (y > limits.y_max) {
                //     const double ps_y = limits.y_max * scale_y; // Масштабированное значение y
                //     fprintf(file, "%f %f lineto\n", ps_x, ps_y);
                //
                // } else {
                //     const double ps_y = limits.y_min * scale_y; // Масштабированное значение y
                //     fprintf(file, "%f %f lineto\n", ps_x, ps_y);
                // }

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
