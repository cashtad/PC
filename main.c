#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DEFAULT_X_MIN (-10)
#define DEFAULT_X_MAX 10
#define DEFAULT_Y_MIN (-10)
#define DEFAULT_Y_MAX 10
#define STEP 0.1

// Функции для математических операций
double eval_function(double x) {
    // Пример функции для теста, можно заменить на любую
    // Например, sin(x), cos(x), exp(x), log(x)
    return sin(x) * cos(x); // функция y = sin(x) * cos(x)
}

// Преобразование координат в систему PostScript
void convert_to_postscript(double x, double y, double x_min, double x_max, double y_min, double y_max, int width, int height, double *ps_x, double *ps_y) {
    *ps_x = ((x - x_min) / (x_max - x_min)) * width;       // Преобразование X в координаты PostScript
    *ps_y = ((y - y_min) / (y_max - y_min)) * height;      // Преобразование Y в координаты PostScript
}

// Функция для записи PostScript файла
void generate_postscript(const char *filename, const double x_min, const double x_max, const double y_min, const double y_max) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", filename);
        exit(3);
    }

    const int width = 500;
    const int height = 500;  // Размеры области рисования

    // Заголовок PostScript файла
    fprintf(fp, "%%!PS-Adobe-2.0\n");
    fprintf(fp, "%%%%Creator: C Graph Plotter\n");
    fprintf(fp, "%%%%Title: Function Graph\n");
    fprintf(fp, "%%%%EndComments\n");

    // Настройка ширины линий и рисование осей
    fprintf(fp, "1 setlinewidth\n");
    fprintf(fp, "newpath\n");
    fprintf(fp, "%d %d moveto\n", width / 2, 0);  // Ось Y
    fprintf(fp, "%d %d lineto\n", width / 2, height);
    fprintf(fp, "%d %d moveto\n", 0, height / 2); // Ось X
    fprintf(fp, "%d %d lineto\n", width, height / 2);
    fprintf(fp, "stroke\n");

    // Рисование графика функции
    fprintf(fp, "0.5 setlinewidth\n");
    double x, y, ps_x, ps_y;
    int is_first_point = 1;
    for (x = x_min; x <= x_max; x += STEP) {
        y = eval_function(x);  // Вычисление значения функции y = f(x)

        if (y < y_min || y > y_max) {
            // Пропускаем точки вне диапазона
            continue;
        }

        // Преобразование координат в систему PostScript
        convert_to_postscript(x, y, x_min, x_max, y_min, y_max, width, height, &ps_x, &ps_y);

        if (is_first_point) {
            fprintf(fp, "newpath\n");
            fprintf(fp, "%.2f %.2f moveto\n", ps_x, ps_y);  // Первая точка
            is_first_point = 0;
        } else {
            fprintf(fp, "%.2f %.2f lineto\n", ps_x, ps_y);  // Соединение точек
        }
    }

    // Закрытие пути и рисование графика
    fprintf(fp, "stroke\n");

    // Конец документа
    fprintf(fp, "showpage\n");
    fprintf(fp, "%%%%EOF\n");

    fclose(fp);
}

// Парсинг аргументов командной строки
int parse_args(int argc, char *argv[], double *x_min, double *x_max, double *y_min, double *y_max, char **filename) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <function> <output_file> [x_min:x_max:y_min:y_max]\n", argv[0]);
        return 1;
    }

    *filename = argv[2];

    // Разбор диапазонов
    if (argc == 4) {
        if (sscanf(argv[3], "%lf:%lf:%lf:%lf", x_min, x_max, y_min, y_max) != 4) {
            fprintf(stderr, "Error: Invalid range format. Expected format: x_min:x_max:y_min:y_max\n");
            return 4;
        }
    } else {
        *x_min = DEFAULT_X_MIN;
        *x_max = DEFAULT_X_MAX;
        *y_min = DEFAULT_Y_MIN;
        *y_max = DEFAULT_Y_MAX;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    double x_min, x_max, y_min, y_max;
    char *output_file;

    // Парсинг аргументов командной строки
    int error_code = parse_args(argc, argv, &x_min, &x_max, &y_min, &y_max, &output_file);
    if (error_code != 0) {
        return error_code;
    }

    // Генерация PostScript файла
    generate_postscript(output_file, x_min, x_max, y_min, y_max);

    printf("Graph saved to %s\n", output_file);
    return 0;
}
