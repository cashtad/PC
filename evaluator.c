#include "evaluator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Recursive evaluation. Returns value of node, if node is 'x' then returns x value from func arg
double evaluate(Node *node, const double x_value) {
    if (node->type == NODE_NUM) {
        return node->num;
    }
    if (node->type == NODE_ID) {
        if (strcmp(node->id, "x") == 0) {
            return x_value;
        }
        return 0.0; // Default value for unknown variable
    }
    if (node->type == NODE_FUNC) {
        const double arg_value = evaluate(node->func.arg, x_value);
        double result;

        // Handle standard mathematical functions
        if (strcmp(node->func.func, "sin") == 0) {
            result = sin(arg_value);
        } else if (strcmp(node->func.func, "cos") == 0) {
            result = cos(arg_value);
        } else if (strcmp(node->func.func, "tan") == 0) {
            result = tan(arg_value);
        } else if (strcmp(node->func.func, "abs") == 0) {
            result = fabs(arg_value);
        } else if (strcmp(node->func.func, "ln") == 0) {
            result = log(arg_value);
        } else if (strcmp(node->func.func, "log") == 0) {
            result = log10(arg_value);
        } else if (strcmp(node->func.func, "asin") == 0) {
            result = asin(arg_value);
        } else if (strcmp(node->func.func, "acos") == 0) {
            result = acos(arg_value);
        } else if (strcmp(node->func.func, "atan") == 0) {
            result = atan(arg_value);
        } else if (strcmp(node->func.func, "sinh") == 0) {
            result = sinh(arg_value);
        } else if (strcmp(node->func.func, "cosh") == 0) {
            result = cosh(arg_value);
        } else if (strcmp(node->func.func, "tanh") == 0) {
            result = tanh(arg_value);
        } else if (strcmp(node->func.func, "exp") == 0) {
            result = exp(arg_value);
        } else {
            fprintf(stderr, "Error: unknown function '%s'\n", node->func.func);
            exit(EXIT_FAILURE);
        }
        return result;
    }
    if (node->type == NODE_OP) {
        if (node->op.left == NULL) {
            // Unarnian operator
            const double right_value = evaluate(node->op.right, x_value);
            switch (node->op.op) {
                case '-':
                    return -right_value;
                default:
                    fprintf(stderr, "Error: unknown unary operator '%c'\n", node->op.op);
                    exit(EXIT_FAILURE);
            }
        }
        // Binary operator
        const double left_value = evaluate(node->op.left, x_value);
        const double right_value = evaluate(node->op.right, x_value);
        double result;

        switch (node->op.op) {
            case '+':
                result = left_value + right_value;
                break;
            case '-':
                result = left_value - right_value;
                break;
            case '*':
                result = left_value * right_value;
                break;
            case '/':
                if (right_value == 0) {
                    fprintf(stderr, "Division by zero\n");
                    exit(EXIT_FAILURE);
                }
                result = left_value / right_value;
                break;
            case '^':
                result = pow(left_value, right_value);
                break;
            default:
                fprintf(stderr, "Error: unknown binary operator '%c'\n", node->op.op);
                exit(EXIT_FAILURE);
        }
        return result;
    }

    fprintf(stderr, "Error: unknown node type\n");
    exit(EXIT_FAILURE);
}
