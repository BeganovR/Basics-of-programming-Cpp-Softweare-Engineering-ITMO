#include "../functions/functions.cpp"
#include <cstring>
#include <fstream>
#include <iostream>

/* Структура для хранения каждой строчки TSV-файла */
struct log_s {
    int width;
    int height;
    uint64_t count;
};

/* Парсер TSV-файла */
void ParsTSVFile(char* str, log_s& logs) {
    int i = 0, small_i = 0;

    /* Пропускаем табы, если есть */
    while (str[i] == ' ') {
        i++;
    }

    /* Считываем ширину */
    char buffer_x[256];
    small_i = 0;
    while (str[i] != ' ' && str[i] != '\0') {
        buffer_x[small_i++] = str[i++];
    }
    buffer_x[small_i] = '\0';

    /* Пропускаем табы */
    while (str[i] == ' ') {
        i++;
    }

    /* Считываем высоту */
    char buffer_y[256];
    small_i = 0;
    while (str[i] != ' ' && str[i] != '\0') {
        buffer_y[small_i++] = str[i++];
    }
    buffer_y[small_i] = '\0';

    /* Пропускаем табы */
    while (str[i] == ' ') {
        i++;
    }
    /* Считываем количество песчинок */
    char buffer_count[512];
    small_i = 0;
    while (str[i] != ' ' && str[i] != '\0') {
        buffer_count[small_i++] = str[i++];
    }
    buffer_count[small_i] = '\0';

    /* Заполняем структуру данными */
    logs.width = StrToInt(buffer_x);
    logs.height = StrToInt(buffer_y);
    logs.count = StrToUint64(buffer_count);

    /* Добавьте дополнительные проверки, если необходимо */
    if (logs.width < 0 || logs.height < 0 || logs.count < 0) {
        std::cerr << "Error: Invalid dimensions or grain count." << std::endl;
        return;
    }
}
