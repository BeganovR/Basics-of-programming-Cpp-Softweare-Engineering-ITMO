#include "../functions/functions.h"
#include <cstring>
#include <iostream>

/* Парсер аргументов командной строки */
void ParsArgs(int argc, char **argv, const char *&filename,
              const char *&output_file, int &max_iter, int &freq) {
    for (int i = 1; i < argc; ++i) {
        /* Ввод .tsv файла */
        if (std::strncmp(argv[i], "-i", 2) == 0) {
            filename = argv[i] + 3;
        } else if (std::strncmp(argv[i], "--input", 7) == 0) {
            filename = argv[i] + 8;
        }

        /* Путь для сохранения */
        if (std::strncmp(argv[i], "-o", 2) == 0) {
            output_file = argv[i] + 3;
        } else if (std::strncmp(argv[i], "--output", 8) == 0) {
            output_file = argv[i] + 9;
        }

        /* Максимальное количество итераций */
        if (std::strncmp(argv[i], "-m", 2) == 0) {
            // Дополнительные переменные для перевода
            char *max_iter_char;
            max_iter_char = argv[i] + 3;

            max_iter = StrToInt(max_iter_char);
        } else if (std::strncmp(argv[i], "--max-iter", 10) == 0) {
            // Дополнительные переменные для перевода
            char *max_iter_char;
            max_iter_char = argv[i] + 11;

            max_iter = StrToInt(max_iter_char);
        }

        /* Частота, с которой должны сохранятся картинки */
        if (std::strncmp(argv[i], "-f", 2) == 0) {
            // Дополнительные переменные для перевода
            char *freq_char;
            freq_char = argv[i] + 3;

            freq = StrToInt(freq_char);
        } else if (std::strncmp(argv[i], "--freq", 6) == 0) {
            // Дополнительные переменные для перевода
            char *freq_char;
            freq_char = argv[i] + 7;

            freq = StrToInt(freq_char);
        }
    }
}
