#ifndef LIMITS_H
#define LIMITS_H
#include <stdlib.h>

/**
 * @brief Defines the default limit value for graphing or function evaluation.
 *
 * This constant represents the default value used for the limits in the graphing system or in evaluating the function.
 * When no specific limits are provided, this value will be used as the default.
 */
#define DEFAULT_LIMIT_VALUE 10

/**
 * @brief Structure that defines the limits for the graph's x and y axes.
 *
 * This structure stores the minimum and maximum values for both the x and y axes, which define the range of the graph's coordinate system.
 * These limits are used for scaling and drawing the graph, ensuring that the graph is rendered within the specified bounds.
 * The limits are represented as double precision floating-point numbers to accommodate a wide range of values.
 *
 * @struct Limits
 * @member x_min The minimum value for the x-axis.
 * @member x_max The maximum value for the x-axis.
 * @member y_min The minimum value for the y-axis.
 * @member y_max The maximum value for the y-axis.
 */
typedef struct Limits {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
} Limits;

/**
 * @brief Parses a string containing limits for the graph and stores them in a `Limits` structure.
 *
 * This function takes a string representation of the graph limits, which is expected to be in the format:
 * "x_min:x_max:y_min:y_max". It then parses the values from the string and stores them in the provided `Limits` structure.
 * The function validates the format of the input string and ensures that the limits are logically correct (i.e., `x_min` should not be greater than `x_max` and `y_min` should not be greater than `y_max`).
 * If the input string is invalid or the limits are not valid, the function returns a non-zero value to indicate an error.
 *
 * @param limits_str A string representing the graph limits in the format "x_min:x_max:y_min:y_max".
 * @param limits A pointer to a `Limits` structure where the parsed limits will be stored.
 *
 * @return Returns 0 if the parsing was successful, or 1 if there was an error (invalid format or invalid limits).
 */
int parse_limits(const char *limits_str, Limits *limits);

/**
 * @brief Initializes a Limits structure with default values for x and y limits.
 *
 * This function allocates memory for a Limits structure and assigns default values
 * to its fields. The limits are set using the predefined constant DEFAULT_LIMIT_VALUE.
 *
 * @return Pointer to the newly allocated Limits structure. If memory allocation fails,
 *         the behavior is undefined.
 *
 * @note The caller is responsible for freeing the allocated memory using `free`
 *       to avoid memory leaks.
 */
Limits *initialize_limits();

#endif //LIMITS_H
