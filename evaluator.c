#include "evaluator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


double evaluate(Node *node, const double x_value) {
    switch (node->type) {
        case NODE_NUM:
            return node->num;

        case NODE_ID:
            return x_value;

        case NODE_FUNC: {
            const double arg_value = evaluate(node->func.arg, x_value);

            if (strcmp(node->func.func, "sin") == 0) return sin(arg_value);
            if (strcmp(node->func.func, "cos") == 0) return cos(arg_value);
            if (strcmp(node->func.func, "tan") == 0) return tan(arg_value);
            if (strcmp(node->func.func, "abs") == 0) return fabs(arg_value);
            if (strcmp(node->func.func, "ln") == 0) {
                if (arg_value <= 0) {
                    fprintf(stderr, "Error: logarithm of non-positive number\n");
                    exit(2);
                }
                return log(arg_value);
            }
            if (strcmp(node->func.func, "log") == 0) {
                if (arg_value <= 0) {
                    fprintf(stderr, "Error: logarithm of non-positive number\n");
                    exit(2);
                }
                return log10(arg_value);
            }
            if (strcmp(node->func.func, "asin") == 0) return asin(arg_value);
            if (strcmp(node->func.func, "acos") == 0) return acos(arg_value);
            if (strcmp(node->func.func, "atan") == 0) return atan(arg_value);
            if (strcmp(node->func.func, "sinh") == 0) return sinh(arg_value);
            if (strcmp(node->func.func, "cosh") == 0) return cosh(arg_value);
            if (strcmp(node->func.func, "tanh") == 0) return tanh(arg_value);
            if (strcmp(node->func.func, "exp") == 0) return exp(arg_value);

            fprintf(stderr, "Error: unknown function '%s'\n", node->func.func);
            exit(2);
        }

        case NODE_OP: {
            if (node->op.left == NULL) {
                double right_value = evaluate(node->op.right, x_value);
                if (node->op.op == '-') {
                    return -right_value;
                }
            }
            const double left_value = evaluate(node->op.left, x_value);
            const double right_value = evaluate(node->op.right, x_value);

            switch (node->op.op) {
                case '+':
                    return left_value + right_value;
                case '-':
                    return left_value - right_value;
                case '*':
                    return left_value * right_value;
                case '/':
                    if (right_value == 0) {
                        fprintf(stderr, "Division by zero\n");
                        exit(2);
                    }
                    return left_value / right_value;
                case '^':
                    return pow(left_value, right_value);
                default:
                    fprintf(stderr, "Error: unknown binary operator '%c'\n", node->op.op);
                    exit(2);
            }
        }

        default:
            fprintf(stderr, "Error: unknown node type\n");
            exit(2);
    }
}
