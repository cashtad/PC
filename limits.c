#include "limits.h"

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
Limits *initialize_limits() {
    Limits *limits = malloc(sizeof(Limits));
    limits->x_min = -DEFAULT_LIMIT_VALUE;
    limits->x_max = DEFAULT_LIMIT_VALUE;
    limits->y_min = -DEFAULT_LIMIT_VALUE;
    limits->y_max = DEFAULT_LIMIT_VALUE;

    return limits;
}