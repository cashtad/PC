// plot_utils.h
#ifndef PLOT_UTILS_H
#define PLOT_UTILS_H

#include <stdio.h>
#include "parser.h"

typedef struct Limits {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
} Limits;


void initialize_statics(const Limits *limits_def, FILE *file_def);

void prepare_graph();

void draw_axis();

void draw_limits();

void draw_support_lines();

void draw_function(Node *abstract_syntax_tree);

void finish();


void draw_graph(Limits *limits, FILE *file, Node *abstract_syntax_tree);


#endif // PLOT_UTILS_H
