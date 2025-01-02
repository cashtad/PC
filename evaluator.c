#include "evaluator.h"

double evaluate(const Node *node, const double x_value) {
    switch (node->type) {
        case NODE_NUM:
            return node->num;

        case NODE_ID:
            return x_value;

        case NODE_FUNC: {
            const double arg_value = evaluate(node->func.arg, x_value);

            if (strcmp(node->func.func, SIN) == 0) return sin(arg_value);
            if (strcmp(node->func.func, COS) == 0) return cos(arg_value);
            if (strcmp(node->func.func, TAN) == 0) return tan(arg_value);
            if (strcmp(node->func.func, ABS) == 0) return fabs(arg_value);
            if (strcmp(node->func.func, LN) == 0) return log(arg_value);
            if (strcmp(node->func.func, LOG) == 0) return log10(arg_value);
            if (strcmp(node->func.func, ASIN) == 0) return asin(arg_value);
            if (strcmp(node->func.func, ACOS) == 0) return acos(arg_value);
            if (strcmp(node->func.func, ATAN) == 0) return atan(arg_value);
            if (strcmp(node->func.func, SINH) == 0) return sinh(arg_value);
            if (strcmp(node->func.func, COSH) == 0) return cosh(arg_value);
            if (strcmp(node->func.func, TANH) == 0) return tanh(arg_value);
            if (strcmp(node->func.func, EXP) == 0) return exp(arg_value);

            error_exit(ERROR_UNKNOWN_FUNCTION_TEXT, ERROR_FUNCTION);
        }

        case NODE_OP: {
            if (node->op.left == NULL) {
                const double right_value = evaluate(node->op.right, x_value);
                if (node->op.op == MINUS_UN) {
                    return -right_value;
                }
            }
            const double left_value = evaluate(node->op.left, x_value);
            const double right_value = evaluate(node->op.right, x_value);

            switch (node->op.op) {
                case PLUS:
                    return left_value + right_value;
                case MINUS:
                    return left_value - right_value;
                case MULT:
                    return left_value * right_value;
                case DIVISION:
                    return left_value / right_value;
                case POWER:
                    return pow(left_value, right_value);
                default:
                    error_exit(ERROR_UNKNOWN_OPERATOR_TEXT, ERROR_FUNCTION);
            }
        }

        default:
            error_exit(ERROR_UNKNOWN_NODE_TEXT, ERROR_FUNCTION);
    }
    return 0; // never reached, but required for compiler to know that the function returns a value
}
