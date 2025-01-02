#include "draw_utils.h"


/**
 * @brief Initializes the PostScript file for graph generation, including setting up page size, font, and coordinate system.
 *
 * This function writes the necessary PostScript commands to the specified file to prepare for drawing the graph. It sets up the font, page size, coordinate transformation, and default colors. The graph is centered based on the provided scale values and limits for the x and y axes.
 *
 * @param limits A pointer to a Limits structure containing the minimum and maximum values for the x and y axes.
 * @param file A pointer to the file where the PostScript content will be written.
 * @param scale_x A pointer to a double representing the scaling factor for the x-axis.
 * @param scale_y A pointer to a double representing the scaling factor for the y-axis.
 *
 * @note The function assumes that the constants `FONT_SIZE`, `PAGE_WIDTH`, and `PAGE_HEIGHT` are defined in 'draw_utils.h'.
 */
void prepare_graph(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y) {
    // Default setup of PostScript document
    fprintf(file, "%%!PS\n");
    fprintf(file, "%%PageSetup\n");

    // Font setup
    // Uses the Courier font at a specified size for any text on the graph (e.g., axis labels)
    fprintf(file, "/Courier findfont %f scalefont setfont\n", FONT_SIZE);

    // Set page size to A4 dimensions
    // The page size is fixed, so changing it is not recommended unless necessary
    fprintf(file, "<< /PageSize [%f %f] >> setpagedevice\n", PAGE_WIDTH, PAGE_HEIGHT);

    // Define a helper function for converting inches to PostScript units (1 inch = 72 units)
    fprintf(file, "/inch {72 mul} def\n");

    // Translate the coordinate system to center the graph on the page
    // The translation adjusts the origin to the center of the graph area by applying the scaling factors
    fprintf(file, "%f %f translate\n", PAGE_WIDTH / 2 - *scale_x * (limits->x_max + limits->x_min) / 2,
            PAGE_HEIGHT / 2 - *scale_y * (limits->y_max + limits->y_min) / 2);

    //Black color
    fprintf(file, "1 0 0 setrgbcolor\n");
}

/**
 * @brief Draws the x and y axes with labels and arrows on the PostScript graph.
 *
 * This function generates the PostScript code to draw both the x and y axes for the graph, including axis lines, arrows at the ends, and labels ('x' for the x-axis and 'y' for the y-axis). The positions and scaling of the axes are determined by the provided limits and scaling factors for the x and y axes.
 *
 * @param limits A pointer to a Limits structure containing the minimum and maximum values for the x and y axes.
 * @param file A pointer to the file where the PostScript content will be written.
 * @param scale_x A pointer to a double representing the scaling factor for the x-axis.
 * @param scale_y A pointer to a double representing the scaling factor for the y-axis.
 *
 * @note The function assumes that the constants `RED_LINE_MARGIN`, `MISC_MARGIN`, and `FONT_SIZE` are defined in 'draw_utils.h'. The arrowheads are drawn at the positive ends of the axes.
 */
void draw_axes(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y,  const double *x_cords_for_y_axis, const double *y_cords_for_x_axis) {

    // Draw the X-axis as a red line at y = 0
    fprintf(file, "%f %f moveto\n", limits->x_min * *scale_x - RED_LINE_MARGIN, *y_cords_for_x_axis);
    fprintf(file, "%f %f lineto\n", limits->x_max * *scale_x + RED_LINE_MARGIN, *y_cords_for_x_axis);
    fprintf(file, "stroke\n");

    // Draw the arrow at the end of the X-axis
    fprintf(file, "%f %f moveto\n", limits->x_max * *scale_x + RED_LINE_MARGIN - MISC_MARGIN, MISC_MARGIN + *y_cords_for_x_axis);
    fprintf(file, "%f %f lineto\n", limits->x_max * *scale_x + RED_LINE_MARGIN, *y_cords_for_x_axis);
    fprintf(file, "%f %f lineto\n", limits->x_max * *scale_x + RED_LINE_MARGIN - MISC_MARGIN, -MISC_MARGIN + *y_cords_for_x_axis);
    fprintf(file, "stroke\n");

    // Draw the 'x' label for the X-axis
    fprintf(file, "%f %f moveto\n", limits->x_max * *scale_x + RED_LINE_MARGIN - MISC_MARGIN, -FONT_SIZE + *y_cords_for_x_axis);
    fprintf(file, "(x) show\n");


    // Draw the Y-axis as a red line at x = 0
    fprintf(file, "%f %f moveto\n", *x_cords_for_y_axis, limits->y_min * *scale_y - RED_LINE_MARGIN);
    fprintf(file, "%f %f lineto\n", *x_cords_for_y_axis, limits->y_max * *scale_y + RED_LINE_MARGIN);
    fprintf(file, "stroke\n");

    // Draw the arrow at the top of the Y-axis
    fprintf(file, "%f %f moveto\n", -MISC_MARGIN + *x_cords_for_y_axis, limits->y_max * *scale_y + RED_LINE_MARGIN - MISC_MARGIN);
    fprintf(file, "%f %f lineto\n", *x_cords_for_y_axis, limits->y_max * *scale_y + RED_LINE_MARGIN);
    fprintf(file, "%f %f lineto\n", MISC_MARGIN + *x_cords_for_y_axis, limits->y_max * *scale_y + RED_LINE_MARGIN - MISC_MARGIN);
    fprintf(file, "stroke\n");

    // Draw the 'y' label for the Y-axis
    fprintf(file, "%f %f moveto\n", MISC_MARGIN + *x_cords_for_y_axis, limits->y_max * *scale_y + RED_LINE_MARGIN - MISC_MARGIN);
    fprintf(file, "(y) show\n");
}

/**
 * @brief Draws the boundary limits of the graph as dashed blue lines on the PostScript file.
 *
 * This function generates the PostScript code to draw the four boundary lines of the graph (left, right, top, and bottom) based on the given limits for the x and y axes. The lines are drawn in blue with a dashed pattern. The function uses the scaling factors for the axes to correctly position the lines according to the limits.
 *
 * @param limits A pointer to a Limits structure containing the minimum and maximum values for the x and y axes.
 * @param file A pointer to the file where the PostScript content will be written.
 * @param scale_x A pointer to a double representing the scaling factor for the x-axis.
 * @param scale_y A pointer to a double representing the scaling factor for the y-axis.
 *
 * @note The function assumes that the constants `PAGE_HEIGHT` and `PAGE_WIDTH` are defined in 'draw_utils.h'. The dashed lines are drawn using a dash pattern `[5 15]`, which is a common format for dashed lines in PostScript.
 */
void draw_limits(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y) {
    // Set up blue dashed line style (color and pattern)
    fprintf(file, "0 0 0.5 setrgbcolor\n"); // Blue color (RGB: 0, 0, 0.5)
    fprintf(file, "[5 15] 0 setdash\n"); // Dash pattern: 5 units long dashes, 15 units between dashes

    // Draw the right boundary line (x_max)
    fprintf(file, "%f %f moveto\n", limits->x_max * *scale_x, -PAGE_HEIGHT * 2);
    fprintf(file, "%f %f lineto\n", limits->x_max * *scale_x, PAGE_HEIGHT * 2);

    // Draw the left boundary line (x_min)
    fprintf(file, "%f %f moveto\n", limits->x_min * *scale_x, -PAGE_HEIGHT * 2);
    fprintf(file, "%f %f lineto\n", limits->x_min * *scale_x, PAGE_HEIGHT * 2);

    // Draw the top boundary line (y_max)
    fprintf(file, "%f %f moveto\n", -PAGE_WIDTH * 2, limits->y_max * *scale_y);
    fprintf(file, "%f %f lineto\n", PAGE_WIDTH * 2, limits->y_max * *scale_y);

    // Draw the bottom boundary line (y_min)
    fprintf(file, "%f %f moveto\n", -PAGE_WIDTH * 2, limits->y_min * *scale_y);
    fprintf(file, "%f %f lineto\n", PAGE_WIDTH * 2, limits->y_min * *scale_y);

    // Finish drawing the boundary limits and reset the dash pattern to solid lines
    fprintf(file, "stroke\n");
    fprintf(file, "[] 0 setdash\n");
}

/**
 * @brief Draws the support lines (grid lines and number labels) for the graph in both x and y directions.
 *
 * This function generates the PostScript code to draw grid lines and number labels for both the x and y axes. It draws grey dashed lines to represent the grid and short black lines at each grid intersection. The grid lines are drawn for both positive and negative values along the axes, based on the specified limits. It also labels the grid lines with numbers.
 *
 * @param limits A pointer to a Limits structure containing the minimum and maximum values for the x and y axes.
 * @param file A pointer to the file where the PostScript content will be written.
 * @param scale_x A pointer to a double representing the scaling factor for the x-axis.
 * @param scale_y A pointer to a double representing the scaling factor for the y-axis.
 *
 * @note The function assumes that the constants `PAGE_WIDTH`, `PAGE_HEIGHT`, `MISC_MARGIN`, `FONT_SIZE` are defined in 'draw_utils.h'. The grid lines are drawn in grey with a dashed pattern, and the number labels are positioned near the grid lines.
 */
void draw_support_lines(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y, const double *x_cords_for_y_axis, const double *y_cords_for_x_axis) {
    double const steps_x_to_right = limits->x_max;
    double const steps_x_to_left = fabs(limits->x_min);
    double const steps_y_up = limits->y_max;
    double const steps_y_down = fabs(limits->y_min);



    //TODO: МБ СДЕЛАТЬ ВСЕ В 1-2 ЦИКЛАХ??


    // Draw grid lines and ticks for the x-axis (Positive direction)
    for (int i = 0; i <= steps_x_to_right; i++) {
        // Draw grey lines (grid)
        if (i > 0 && i != steps_x_to_right) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n");
            fprintf(file, "%f %f moveto\n", i * *scale_x, -PAGE_HEIGHT * 2 + *y_cords_for_x_axis);
            fprintf(file, "%f %f lineto\n", i * *scale_x, PAGE_HEIGHT * 2 + *y_cords_for_x_axis);
            fprintf(file, "stroke\n");
        }
        // Draw black ticks on the x-axis
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", i * *scale_x, MISC_MARGIN + *y_cords_for_x_axis);
        fprintf(file, "%f %f lineto\n", i * *scale_x, -MISC_MARGIN + *y_cords_for_x_axis);
        fprintf(file, "stroke\n");

        // Draw numbers for x-axis ticks
        fprintf(file, "%f %f moveto\n", i * *scale_x - FONT_SIZE / 4, -FONT_SIZE - MISC_MARGIN + *y_cords_for_x_axis);
        if (i > 0) {
            fprintf(file, "(%d) show\n", i);
        }
    }

    // Repeat the same process for negative x-axis values
    for (int i = -1; i >= -steps_x_to_left; i--) {
        // Draw grey dashed lines (grid)
        if (i != -steps_x_to_left) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", i * *scale_x, -PAGE_HEIGHT * 2 + *y_cords_for_x_axis);
            fprintf(file, "%f %f lineto\n", i * *scale_x, PAGE_HEIGHT * 2 + *y_cords_for_x_axis);
            fprintf(file, "stroke\n");
        }
        // Draw black ticks on the x-axis
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", i * *scale_x, MISC_MARGIN + *y_cords_for_x_axis);
        fprintf(file, "%f %f lineto\n", i * *scale_x, -MISC_MARGIN + *y_cords_for_x_axis);
        fprintf(file, "stroke\n");

        // Draw numbers for negative x-axis ticks
        fprintf(file, "%f %f moveto\n", i * *scale_x - FONT_SIZE + FONT_SIZE / 10, -FONT_SIZE - MISC_MARGIN + *y_cords_for_x_axis);
        fprintf(file, "(%d) show\n", i);
    }
    for (int i = 0; i <= steps_y_up; i++) {
        //Grey lines
        if (i > 0 && i != steps_y_up) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", -PAGE_WIDTH * 2 + *x_cords_for_y_axis, i * *scale_y);
            fprintf(file, "%f %f lineto\n", PAGE_WIDTH * 2 + *x_cords_for_y_axis, i * *scale_y);
            fprintf(file, "stroke\n");
        }

        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", -MISC_MARGIN + *x_cords_for_y_axis, i * *scale_y);
        fprintf(file, "%f %f lineto\n", MISC_MARGIN + *x_cords_for_y_axis, i * *scale_y);
        fprintf(file, "stroke\n");

        //Numbers
        if (i > 0) {
            fprintf(file, "%f %f moveto\n", MISC_MARGIN + 1 + *x_cords_for_y_axis, i * *scale_y - FONT_SIZE / 4);
            fprintf(file, "(%d) show\n", i);
        }
    }
    for (int i = -1; i >= -steps_y_down; i--) {
        //Grey lines
        if (i != -steps_y_down) {
            fprintf(file, "0.8 0.8 0.8 setrgbcolor\n"); // Полутон (50% серого)
            fprintf(file, "%f %f moveto\n", -PAGE_WIDTH * 2 + *x_cords_for_y_axis, i * *scale_y);
            fprintf(file, "%f %f lineto\n", PAGE_WIDTH * 2 + *x_cords_for_y_axis, i * *scale_y);
            fprintf(file, "stroke\n");
        }
        //Short black lines
        fprintf(file, "0 0 0 setrgbcolor\n");
        fprintf(file, "%f %f moveto\n", -MISC_MARGIN + *x_cords_for_y_axis, i * *scale_y);
        fprintf(file, "%f %f lineto\n", MISC_MARGIN + *x_cords_for_y_axis, i * *scale_y);
        fprintf(file, "stroke\n");

        //Numbers
        fprintf(file, "%f %f moveto\n", MISC_MARGIN + 1.0 + *x_cords_for_y_axis, i * *scale_y - FONT_SIZE / 4);
        fprintf(file, "(%d) show\n", i);
    }
}

/**
 * @brief Draws a mathematical function based on an abstract syntax tree in the PostScript format.
 *
 * This function evaluates the mathematical function represented by the given abstract syntax tree for a range of x-values specified by the limits. It generates the corresponding PostScript code to draw the function on a graph, considering scaling factors for both the x and y axes. The function ensures that points falling outside the y-axis limits are skipped, and it only connects points within the valid range.
 *
 * @param limits A pointer to a Limits structure containing the minimum and maximum values for the x and y axes.
 * @param file A pointer to the file where the PostScript content will be written.
 * @param scale_x A pointer to a double representing the scaling factor for the x-axis.
 * @param scale_y A pointer to a double representing the scaling factor for the y-axis.
 * @param abstract_syntax_tree A pointer to a Node representing the abstract syntax tree of the function to be plotted.
 *
 * @note The function assumes the presence of the constant `X_EVALUATION_STEP`, which defines the step size for the x-values. The function also assumes the availability of an `evaluate` function that computes the y-value for a given x based on the abstract syntax tree.
 */
void draw_function(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y,
                   const Node *abstract_syntax_tree) {
    int first_point = 1;
    int out_of_range = 0;
    for (double x = limits->x_min; x <= limits->x_max; x += X_EVALUATION_STEP) {
        const double y = evaluate(abstract_syntax_tree, x);
        // For invalid evaluate case, for example if 2/x and x == 0
        if (isnan(y)) {
            if (!first_point) {
                fprintf(file, "stroke\n"); // Close the current path if the function can not be evaluated in this point
            }
            first_point = 1;
            out_of_range = 1;
            continue;

        }
        if (y > limits->y_max || y < limits->y_min) {
            if (!out_of_range) {
                first_point = 1;
                out_of_range = 1;
                fprintf(file, "stroke\n"); // Close the current path if the function goes out of range
            }
        } else {
            const double ps_x = x * *scale_x;
            const double ps_y = y * *scale_y;
            out_of_range = 0;
            if (first_point) {
                fprintf(file, "%f %f moveto\n", ps_x, ps_y); // Start a new path at the first point
                first_point = 0;
            } else {
                fprintf(file, "%f %f lineto\n", ps_x, ps_y); // Connect points with lines
            }
        }
    }
}

/**
 * @brief Finalizes the PostScript drawing and ends the page.
 *
 * This function completes the drawing process by finalizing the current path with a stroke and signaling the end of the current page in the PostScript output. It is intended to be called after all drawing commands have been issued to ensure the graph is properly rendered and the page is closed.
 *
 * @param file A pointer to the file where the PostScript content is being written.
 *
 * @note This function assumes that all drawing operations (such as axes, lines, and the function plot) have been completed before it is called. It does not modify the drawing content itself but ensures the output is correctly finalized for rendering.
 */
void finish(FILE *file) {
    fprintf(file, "stroke\n"); // Ensure that all paths are rendered
    fprintf(file, "showpage\n"); // Output the current page and finalize the drawing
}

/**
 * @brief Draws the entire graph, including axes, limits, support lines, and the mathematical function.
 *
 * This function prepares the PostScript environment and then sequentially draws various elements of the graph. It first calculates the scaling factors for the x and y axes based on the page size and the specified limits. Then, it draws the graph components, including:
 * - Axes with labels
 * - The limits of the plot area
 * - Support lines (grid and tick marks)
 * - The function curve based on the provided abstract syntax tree
 * Finally, it finalizes the PostScript output to ensure the graph is properly rendered.
 *
 * @param limits A pointer to a Limits structure containing the minimum and maximum values for the x and y axes.
 * @param file A pointer to the file where the PostScript content will be written.
 * @param abstract_syntax_tree A pointer to a Node representing the abstract syntax tree of the function to be plotted.
 *
 * @note This function calls several other functions to draw individual components of the graph, including `prepare_graph`, `draw_axes`, `draw_limits`, `draw_support_lines`, and `draw_function`, and finishes with the `finish` function to complete the PostScript output.
 */
void draw_graph(const Limits *limits, FILE *file, const Node *abstract_syntax_tree) {
    const double scale_x = (PAGE_WIDTH - PAGE_MARGIN) / (limits->x_max - limits->x_min); // X-axis scaling
    const double scale_y = (PAGE_HEIGHT - PAGE_MARGIN) / (limits->y_max - limits->y_min); // Y-axis scaling
    double x_cords_for_y_axis;
    double y_cords_for_x_axis;

    if (limits->x_min > 0) {
        x_cords_for_y_axis = limits->x_min * scale_x;
    } else if (limits->x_max < 0) {
        x_cords_for_y_axis = limits->x_max * scale_x;

    } else {
        x_cords_for_y_axis = 0.0;
    }
    if (limits->y_min > 0) {
        y_cords_for_x_axis = limits->y_min * scale_y;
    } else if (limits->y_max < 0) {
        y_cords_for_x_axis = limits->y_max * scale_y;

    } else {
        y_cords_for_x_axis = 0.0;
    }

    prepare_graph(limits, file, &scale_x, &scale_y);
    draw_axes(limits, file, &scale_x, &scale_y, &x_cords_for_y_axis, &y_cords_for_x_axis);
    draw_limits(limits, file, &scale_x, &scale_y);
    draw_support_lines(limits, file, &scale_x, &scale_y, &x_cords_for_y_axis, &y_cords_for_x_axis);
    draw_function(limits, file, &scale_x, &scale_y, abstract_syntax_tree);
    finish(file);
}
