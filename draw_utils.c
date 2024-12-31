#include <stdio.h>
#include <stdlib.h>
#include "evaluator.h"
#include "parser.h"
#include "draw_utils.h"


static const double page_width = 595.0; // A4 width
static const double page_height = 842.0; // A4 height
static Limits limits;
static FILE *file;
static double scale_x;
static double scale_y;
static const double PAGE_MARGIN = 100.0;

void initialize_statics(const Limits *limits_def, FILE *file_def) {
    limits = *limits_def;
    file = file_def;
    scale_x = (page_width - PAGE_MARGIN) / (limits.x_max - limits.x_min); // X-axis scaling
    scale_y = (page_height - PAGE_MARGIN) / (limits.y_max - limits.y_min); // Y-axis scaling
}


void prepare_graph() {
    fprintf(file, "%%PageSetup\n");
    fprintf(file, "/Courier findfont 12 scalefont setfont\n");
    fprintf(file, "<< /PageSize [%f %f] >> setpagedevice\n", page_width, page_height);

    fprintf(file, "%%!PS\n");
    fprintf(file, "/inch {72 mul} def\n");

    fprintf(file, "%f %f translate\n", page_width / 2 - scale_x * (limits.x_max + limits.x_min) / 2,
            page_height / 2 - scale_y * (limits.y_max + limits.y_min) / 2);


    fprintf(file, "1 0 0 setrgbcolor\n");
}

void draw_axis() {
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
}

void draw_limits() {
    //Limits (blue lines)
    fprintf(file, "0 0 0.5 setrgbcolor\n");
    fprintf(file, "[5 15] 0 setdash\n");
    //RIGHT limit
    fprintf(file, "%f %f moveto\n", limits.x_max * scale_x, -page_height * 2);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x, page_height * 2);
    //LEFT limit
    fprintf(file, "%f %f moveto\n", limits.x_min * scale_x, -page_height * 2);
    fprintf(file, "%f %f lineto\n", limits.x_min * scale_x, page_height * 2);
    //TOP limit
    fprintf(file, "%f %f moveto\n", -page_width * 2, limits.y_max * scale_y);
    fprintf(file, "%f %f lineto\n", page_width * 2, limits.y_max * scale_y);
    //BOT limit
    fprintf(file, "%f %f moveto\n", -page_width * 2, limits.y_min * scale_y);
    fprintf(file, "%f %f lineto\n", page_width * 2, limits.y_min * scale_y);

    fprintf(file, "stroke\n");
    fprintf(file, "[] 0 setdash\n");
}

void draw_support_lines() {
    int const steps_x_to_right = (int) (limits.x_max);
    int const steps_x_to_left = abs((int) limits.x_min);
    int const steps_y_up = (int) (limits.y_max);
    int const steps_y_down = abs((int) (limits.y_min));

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
}

void draw_function(Node *abstract_syntax_tree) {
    int first_point = 1;
    int out_of_range = 0;
    for (double x = limits.x_min; x <= limits.x_max; x += 0.01) {
        const double y = evaluate(abstract_syntax_tree, x);
        if (y > limits.y_max || y < limits.y_min) {
            if (!out_of_range) {
                first_point = 1;
                out_of_range = 1;
                fprintf(file, "stroke\n");
            }
        } else {
            const double ps_x = x * scale_x;
            const double ps_y = y * scale_y;
            out_of_range = 0;
            if (first_point) {
                fprintf(file, "%f %f moveto\n", ps_x, ps_y);
                first_point = 0;
            } else {
                fprintf(file, "%f %f lineto\n", ps_x, ps_y);
            }
        }
    }
}

void finish() {
    fprintf(file, "stroke\n");
    fprintf(file, "showpage\n");
}


void draw_graph(Limits *limits_def, FILE *file_def, Node *abstract_syntax_tree) {
    initialize_statics(limits_def, file_def);
    prepare_graph(limits_def, file_def);
    draw_axis();
    draw_limits();
    draw_support_lines();
    draw_function(abstract_syntax_tree);
    finish();
}
