#ifndef PLOT_UTILS_H
#define PLOT_UTILS_H

#include "evaluator.h"
#include "limits.h"

/**
 * @brief Defines constants used for graph rendering and page layout.
 */

/**
 * @brief Width of the page in points (for printing).
 *
 * This value defines the width of the page. In a typical print environment, 1 point equals 1/72 inch.
 */
#define PAGE_WIDTH 595.0

/**
 * @brief Height of the page in points (for printing).
 *
 * This value defines the height of the page. In a typical print environment, 1 point equals 1/72 inch.
 */
#define PAGE_HEIGHT 842.0

/**
 * @brief Margin around the page.
 *
 * This value specifies the margin distance from the edge of the page to the content, ensuring proper spacing for printed content.
 */
#define PAGE_MARGIN 100.0

/**
 * @brief Margin for red lines (possibly for visual separation or emphasis).
 *
 * Defines the specific margin size for red lines in the graph or print layout.
 */
#define RED_LINE_MARGIN 25.0

/**
 * @brief General margin used for minor spacing adjustments.
 *
 * This margin can be used for fine-tuning the layout of elements such as ticks, numbers, or other graphical items.
 */
#define MISC_MARGIN 5.0

/**
 * @brief Font size for text elements.
 *
 * Defines the font size used for displaying numbers or labels in the graph, ensuring legibility.
 */
#define FONT_SIZE 12.0

/**
 * @brief Step size for evaluating function values.
 *
 * This defines the granularity of the evaluation of the function for plotting. A smaller step size gives a more detailed graph.
 */
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
 * @brief Draws the coordinate axes on a PostScript file.
 *
 * This function generates PostScript commands to draw the X and Y axes on a graph.
 * The X-axis is drawn as a horizontal red line at y = 0, and the Y-axis is drawn
 * as a vertical red line at x = 0. Each axis has an arrow at its positive end,
 * and labels 'x' and 'y' are added near the arrows to indicate the axes.
 *
 * @param limits Pointer to a Limits structure defining the minimum and maximum
 *               values for the axes.
 * @param file   Pointer to the output file where PostScript commands are written.
 * @param scale_x Pointer to the scaling factor for the X-axis.
 * @param scale_y Pointer to the scaling factor for the Y-axis.
 * @param x_cords_for_y_axis Pointer to the X-coordinate for the Y-axis position.
 * @param y_cords_for_x_axis Pointer to the Y-coordinate for the X-axis position.
 *
 * @note This function assumes that the file is already open for writing and that
 *       all pointers passed to it are valid.
 *
 * @note The function uses the following constants:
 *       - `RED_LINE_MARGIN`: Extra margin added to the axis lines.
 *       - `MISC_MARGIN`: Margin used for arrow and label positioning.
 *       - `FONT_SIZE`: Font size for the axis labels.
 */
void draw_axes(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y,
               const double *x_cords_for_y_axis, const double *y_cords_for_x_axis);

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
 * @brief Draws supporting grid lines and tick marks for the graph axes.
 *
 * This function generates PostScript commands to draw grid lines and tick marks
 * for both the X and Y axes. It also labels the tick marks with their respective
 * coordinate values. The grid lines are drawn in grey, while the tick marks and
 * labels are black.
 *
 * @param limits Pointer to a Limits structure defining the minimum and maximum
 *               values for the axes.
 * @param file   Pointer to the output file where PostScript commands are written.
 * @param scale_x Pointer to the scaling factor for the X-axis.
 * @param scale_y Pointer to the scaling factor for the Y-axis.
 * @param x_cords_for_y_axis Pointer to the X-coordinate for the Y-axis position.
 * @param y_cords_for_x_axis Pointer to the Y-coordinate for the X-axis position.
 *
 * @note The function draws grid lines and ticks for both positive and negative
 *       directions along the X and Y axes. Tick marks are labeled with integers.
 * @note The function uses constants like `PAGE_HEIGHT`, `PAGE_WIDTH`, `MISC_MARGIN`,
 *       and `FONT_SIZE` to position lines and labels.
 *
 * @note This function assumes that the file is already open for writing and that
 *       all pointers passed to it are valid.
 */
void draw_support_lines(const Limits *limits, FILE *file, const double *scale_x, const double *scale_y,
                        const double *x_cords_for_y_axis, const double *y_cords_for_x_axis);

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
                   const Node *abstract_syntax_tree);

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
 * @brief Draws the graph for the given function based on the defined limits.
 *
 * This function generates a graph in the PostScript format, including grid lines,
 * axes, limits, and the function itself. It scales the graph based on the provided
 * limits and positions the axes accordingly. The graph is drawn using PostScript
 * commands, which can be rendered by compatible software (e.g., a PostScript viewer).
 *
 * @param limits Pointer to a Limits structure that defines the minimum and maximum
 *               values for the graph's X and Y axes.
 * @param file   Pointer to the output file where PostScript commands will be written.
 * @param abstract_syntax_tree Pointer to a Node structure representing the abstract
 *                             syntax tree of the mathematical function to be graphed.
 *
 * @note The function calls various helper functions (`prepare_graph`, `draw_axes`,
 *       `draw_limits`, `draw_support_lines`, and `draw_function`) to construct the graph.
 * @note This function calculates the appropriate scaling factors for both axes
 *       based on the provided limits and adjusts the graph's positioning accordingly.
 *
 * @note Ensure the file is already opened in write mode before passing it to this function.
 */
void draw_graph(const Limits *limits, FILE *file, const Node *abstract_syntax_tree);


#endif // PLOT_UTILS_H
