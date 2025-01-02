#ifndef PLOT_UTILS_H
#define PLOT_UTILS_H

#include "evaluator.h"
#include "limits.h"



#define PAGE_WIDTH 595.0
#define PAGE_HEIGHT 842.0
#define PAGE_MARGIN 100.0
#define RED_LINE_MARGIN 25.0
#define MISC_MARGIN 5.0
#define FONT_SIZE 12.0
#define X_EVALUATION_STEP 0.01




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
void prepare_graph(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y);

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
void draw_axes(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y, const double *x_cords_for_y_axis, const double *y_cords_for_x_axis);

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
void draw_limits(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y);

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
void draw_support_lines(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y, const double *x_cords_for_y_axis, const double *y_cords_for_x_axis);

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
void draw_function(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y, const Node *abstract_syntax_tree);

/**
 * @brief Finalizes the PostScript drawing and ends the page.
 *
 * This function completes the drawing process by finalizing the current path with a stroke and signaling the end of the current page in the PostScript output. It is intended to be called after all drawing commands have been issued to ensure the graph is properly rendered and the page is closed.
 *
 * @param file A pointer to the file where the PostScript content is being written.
 *
 * @note This function assumes that all drawing operations (such as axes, lines, and the function plot) have been completed before it is called. It does not modify the drawing content itself but ensures the output is correctly finalized for rendering.
 */
void finish(FILE *file);

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
void draw_graph(const Limits *limits, FILE *file, const Node *abstract_syntax_tree);


#endif // PLOT_UTILS_H
