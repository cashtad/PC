#include "limits.h"

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
