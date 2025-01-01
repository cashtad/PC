#include <stdio.h>
#include <stdlib.h>
#include "evaluator.h"
#include "parser.h"
#include "draw_utils.h"


static Limits limits;
static FILE *file;
static double scale_x;
static double scale_y;


/**
 * @brief Prepares the output file for drawing the graph in PostScript format.
 *
 * This function initializes the PostScript setup for drawing a graph. It sets the font,
 * page size, and translation of the coordinate system to the center of the page. The
 * function also prepares the output file by defining a 1:1 scale for the page and setting
 * up the color for the graphing operations. It assumes that the `initialize_statics` function
 * has been called beforehand to define the scaling factors and the page limits.
 *
 * @note The function writes PostScript commands to the provided output file to set up the
 *       page size and coordinate transformations. The graph's origin is positioned at the
 *       center of the page, and the function's color is set to black (`1 0 0` for RGB).
 */
void prepare_graph() {
    fprintf(file, "%%PageSetup\n");
    fprintf(file, "/Courier findfont %f scalefont setfont\n", FONT_SIZE);
    fprintf(file, "<< /PageSize [%f %f] >> setpagedevice\n", PAGE_WIDTH, PAGE_HEIGHT);

    fprintf(file, "%%!PS\n");
    fprintf(file, "/inch {72 mul} def\n");

    fprintf(file, "%f %f translate\n", PAGE_WIDTH / 2 - scale_x * (limits.x_max + limits.x_min) / 2,
            PAGE_HEIGHT / 2 - scale_y * (limits.y_max + limits.y_min) / 2);


    fprintf(file, "1 0 0 setrgbcolor\n");
}

/**
 * @brief Draws the coordinate axes in the graph.
 *
 * This function draws the X and Y axes for the graph in PostScript format. It includes
 * red lines for the axes, arrows at the end of the axes, and labels ('x' and 'y')
 * at the respective ends of the axes. The function uses predefined limits and scale factors
 * to determine the positions and sizes of the axes and their components.
 *
 * - The X-axis is drawn horizontally, with an arrow pointing to the right and a label 'x'
 *   positioned at the right end of the axis.
 * - The Y-axis is drawn vertically, with an arrow pointing upwards and a label 'y'
 *   positioned at the top end of the axis.
 *
 * The margins and positioning are handled using constants like `RED_LINE_MARGIN` and
 * `MISC_MARGIN`, which control the spacing between the axes and the graph content.
 */
void draw_axis() {
    //X red line
    fprintf(file, "%f %f moveto\n", limits.x_min * scale_x - RED_LINE_MARGIN, 0.0);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x + RED_LINE_MARGIN, 0.0);
    fprintf(file, "stroke\n");
    //X arrow
    fprintf(file, "%f %f moveto\n", -MISC_MARGIN, limits.y_max * scale_y + RED_LINE_MARGIN - MISC_MARGIN);
    fprintf(file, "%f %f lineto\n", 0.0, limits.y_max * scale_y + RED_LINE_MARGIN);
    fprintf(file, "%f %f lineto\n", MISC_MARGIN, limits.y_max * scale_y + RED_LINE_MARGIN - MISC_MARGIN);
    fprintf(file, "stroke\n");
    //X letter
    fprintf(file, "%f %f moveto\n", limits.x_max * scale_x + RED_LINE_MARGIN - MISC_MARGIN, -FONT_SIZE);
    fprintf(file, "(x) show\n");


    //Y red line
    fprintf(file, "%f %f moveto\n", 0.0, limits.y_min * scale_y - RED_LINE_MARGIN);
    fprintf(file, "%f %f lineto\n", 0.0, limits.y_max * scale_y + RED_LINE_MARGIN);
    fprintf(file, "stroke\n");
    //Y arrow
    fprintf(file, "%f %f moveto\n", limits.x_max * scale_x + RED_LINE_MARGIN - MISC_MARGIN, MISC_MARGIN);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x + RED_LINE_MARGIN, 0.0);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x + RED_LINE_MARGIN - MISC_MARGIN, -MISC_MARGIN);
    fprintf(file, "stroke\n");
    //Y letter
    fprintf(file, "%f %f moveto\n", MISC_MARGIN, limits.y_max * scale_y + RED_LINE_MARGIN - MISC_MARGIN);
    fprintf(file, "(y) show\n");
}

/**
 * @brief Draws the boundary limits of the graph.
 *
 * This function draws the boundary limits of the graph in PostScript format. The boundaries
 * are represented by dashed blue lines. It draws lines at the extreme X and Y coordinates
 * defined by the limits (`limits.x_min`, `limits.x_max`, `limits.y_min`, and `limits.y_max`),
 * ensuring that the graph is properly constrained within the defined range.
 *
 * The limits are drawn as dashed lines in blue color. The limits are drawn on all four sides:
 * - Right boundary at `limits.x_max`
 * - Left boundary at `limits.x_min`
 * - Top boundary at `limits.y_max`
 * - Bottom boundary at `limits.y_min`
 *
 * The function uses a dashed line style and resets it back to solid after the limits are drawn.
 */
void draw_limits() {
    //Limits (blue lines)
    fprintf(file, "0 0 0.5 setrgbcolor\n");
    fprintf(file, "[5 15] 0 setdash\n");
    //RIGHT limit
    fprintf(file, "%f %f moveto\n", limits.x_max * scale_x, -PAGE_HEIGHT * 2);
    fprintf(file, "%f %f lineto\n", limits.x_max * scale_x, PAGE_HEIGHT * 2);
    //LEFT limit
    fprintf(file, "%f %f moveto\n", limits.x_min * scale_x, -PAGE_HEIGHT * 2);
    fprintf(file, "%f %f lineto\n", limits.x_min * scale_x, PAGE_HEIGHT * 2);
    //TOP limit
    fprintf(file, "%f %f moveto\n", -PAGE_WIDTH * 2, limits.y_max * scale_y);
    fprintf(file, "%f %f lineto\n", PAGE_WIDTH * 2, limits.y_max * scale_y);
    //BOT limit
    fprintf(file, "%f %f moveto\n", -PAGE_WIDTH * 2, limits.y_min * scale_y);
    fprintf(file, "%f %f lineto\n", PAGE_WIDTH * 2, limits.y_min * scale_y);

    fprintf(file, "stroke\n");
    fprintf(file, "[] 0 setdash\n");
}

/**
 * @brief Draws the support lines and labels on the graph.
 *
 * This function draws both the grid lines and labels along the X and Y axes in PostScript format.
 * It creates a grid with grey lines for support, and black short lines along the axes.
 * The numbers corresponding to the grid lines are also displayed.
 *
 * The function divides the graph area into steps based on the X and Y axis limits:
 * - X-axis lines are drawn from the minimum to maximum values of the X-axis (`limits.x_min` to `limits.x_max`).
 * - Y-axis lines are drawn from the minimum to maximum values of the Y-axis (`limits.y_min` to `limits.y_max`).
 *
 * The grid lines are drawn as grey dashed lines, with the X and Y axes themselves drawn as black short lines.
 * For each grid line, the corresponding number is displayed next to it along the axes.
 *
 * @note The function assumes that the `limits` structure contains the graph's axis limits and that the scaling factors (`scale_x` and `scale_y`) have been properly initialized.
 */
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
            fprintf(file, "%f %f moveto\n", i * scale_x, -PAGE_HEIGHT * 2);
            fprintf(file, "%f %f lineto\n", i * scale_x, PAGE_HEIGHT * 2);
            fprintf(file, "stroke\n");
        }
        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", i * scale_x, MISC_MARGIN);
        fprintf(file, "%f %f lineto\n", i * scale_x, -MISC_MARGIN);
        fprintf(file, "stroke\n");

        //Numbers
        fprintf(file, "%f %f moveto\n", i * scale_x - FONT_SIZE / 4, -FONT_SIZE - MISC_MARGIN);
        if (i > 0) {
            fprintf(file, "(%d) show\n", i);
        }
    }
    for (int i = -1; i >= -steps_x_to_left; i--) {
        //Grey lines
        if (i != -steps_x_to_left) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", i * scale_x, -PAGE_HEIGHT * 2);
            fprintf(file, "%f %f lineto\n", i * scale_x, PAGE_HEIGHT * 2);
            fprintf(file, "stroke\n");
        }
        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", i * scale_x, MISC_MARGIN);
        fprintf(file, "%f %f lineto\n", i * scale_x, -MISC_MARGIN);
        fprintf(file, "stroke\n");

        //Numbers
        fprintf(file, "%f %f moveto\n", i * scale_x - FONT_SIZE + FONT_SIZE / 10, -FONT_SIZE - MISC_MARGIN);
        fprintf(file, "(%d) show\n", i);
    }
    for (int i = 0; i <= steps_y_up; i++) {
        //Grey lines
        if (i > 0 && i != steps_y_up) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", -PAGE_WIDTH * 2, i * scale_y);
            fprintf(file, "%f %f lineto\n", PAGE_WIDTH * 2, i * scale_y);
            fprintf(file, "stroke\n");
        }

        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", -MISC_MARGIN, i * scale_y);
        fprintf(file, "%f %f lineto\n", MISC_MARGIN, i * scale_y);
        fprintf(file, "stroke\n");

        //Numbers
        if (i > 0) {
            fprintf(file, "%f %f moveto\n", MISC_MARGIN + 1, i * scale_y - FONT_SIZE / 4);
            fprintf(file, "(%d) show\n", i);
        }
    }
    for (int i = -1; i >= -steps_y_down; i--) {
        //Grey lines
        if (i != -steps_y_down) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", -PAGE_WIDTH * 2, i * scale_y);
            fprintf(file, "%f %f lineto\n", PAGE_WIDTH * 2, i * scale_y);
            fprintf(file, "stroke\n");
        }
        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", -MISC_MARGIN, i * scale_y);
        fprintf(file, "%f %f lineto\n", MISC_MARGIN, i * scale_y);
        fprintf(file, "stroke\n");

        //Numbers
        fprintf(file, "%f %f moveto\n", MISC_MARGIN + 1.0, i * scale_y - FONT_SIZE / 4);
        fprintf(file, "(%d) show\n", i);
    }
}

/**
 * @brief Draws the graph of a mathematical function based on an abstract syntax tree.
 *
 * This function draws the graph of a mathematical function over the range defined by the `limits.x_min` and `limits.x_max` values along the X-axis, using the corresponding Y values computed by the `evaluate` function. The function is drawn in PostScript format, creating a continuous line for points within the specified range on both the X and Y axes.
 *
 * The function iterates over the X values from `limits.x_min` to `limits.x_max` with a step of 0.01. For each X value, it calculates the corresponding Y value by evaluating the abstract syntax tree (AST). If the Y value is within the specified vertical limits (`limits.y_min` to `limits.y_max`), it is plotted as part of the graph. If the Y value is out of range, the function stops drawing and starts a new segment if the next point is within the range again.
 *
 * The function ensures that the line is properly closed (with `stroke` command) when the graph goes out of the Y-axis range. The graph's points are scaled using the `scale_x` and `scale_y` factors before being written to the PostScript file.
 *
 * @param abstract_syntax_tree A pointer to the root of the abstract syntax tree representing the mathematical function to be plotted.
 *
 * @note The function assumes that the `limits` structure contains the graph's axis limits, and that the scaling factors (`scale_x` and `scale_y`) are initialized. The `evaluate` function is used to compute the Y values based on the abstract syntax tree for each X value.
 */
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

/**
 * @brief Finalizes the PostScript file and completes the drawing process.
 *
 * This function concludes the drawing of the graph by issuing the `stroke` command to ensure that any remaining paths are rendered and then signals the end of the page with the `showpage` command. The `showpage` command tells the PostScript interpreter to complete the current page and prepare for output, typically used to flush the drawing commands to the output device or file.
 *
 * The function does not take any parameters and assumes that all drawing operations have been completed before it is called. It should be the last function invoked in the drawing process to ensure the proper closure of the page.
 */
void finish() {
    fprintf(file, "stroke\n");
    fprintf(file, "showpage\n");
}


/**
 * @brief Initializes static variables for drawing the graph.
 *
 * This function sets up the necessary static variables that are used for drawing the graph
 * of the mathematical function. It takes the provided limits for the x and y axes and the
 * output file, and computes the scaling factors for both axes. These scaling factors are
 * used to map the function's values to the page coordinates. This function must be called
 * before drawing the graph to ensure that the correct scaling and file setup are in place.
 *
 * @param limits_def Pointer to a `Limits` structure containing the minimum and maximum
 *                   values for both the x and y axes.
 * @param file_def Pointer to the file where the graph will be written (usually a PostScript file).
 *
 * @note The function assumes that `page_width` and `page_height` are predefined constants
 *       representing the dimensions of the output page. Additionally, `PAGE_MARGIN`
 *       is assumed to be a constant that defines the margin around the graph on the page.
*/
void initialize_statics(const Limits *limits_def, FILE *file_def) {
    limits = *limits_def;
    file = file_def;
    scale_x = (PAGE_WIDTH - PAGE_MARGIN) / (limits.x_max - limits.x_min); // X-axis scaling
    scale_y = (PAGE_HEIGHT - PAGE_MARGIN) / (limits.y_max - limits.y_min); // Y-axis scaling
}

/**
 * @brief Draws the complete graph including axes, limits, support lines, and the function.
 *
 * This function initializes the necessary static variables, prepares the PostScript file, and draws the graph based on the given input. It sequentially executes the drawing steps:
 * - Initializes the static variables (scaling factors, limits, and output file).
 * - Prepares the graphical settings, such as page size and font.
 * - Draws the X and Y axes with labels and arrows.
 * - Draws the limit lines (blue dashed lines) to indicate the graph's boundaries.
 * - Draws support lines (grey grid lines and axis ticks).
 * - Draws the function curve as per the abstract syntax tree.
 * - Finalizes the drawing and closes the page.
 *
 * @param limits A pointer to a `Limits` structure containing the minimum and maximum values for the x and y axes.
 * @param file A pointer to a `FILE` where the PostScript drawing commands will be written.
 * @param abstract_syntax_tree A pointer to the root node of the abstract syntax tree (AST) that represents the mathematical function to be plotted.
*/
void draw_graph(Limits *limits, FILE *file, Node *abstract_syntax_tree) {
    initialize_statics(limits, file);
    prepare_graph(limits, file);
    draw_axis();
    draw_limits();
    draw_support_lines();
    draw_function(abstract_syntax_tree);
    finish();
}
