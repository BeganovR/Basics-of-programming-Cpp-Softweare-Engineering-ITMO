/*
Struct of this Sandpile project:

    /bmp
        - bmp_writer.cpp - - - - - - - - -|
        - bmp_writer.h   <- - - - |       |
                                  |       |
    /matrix                       |       |
        - matrix.cpp              |       |
        - matrix.h       - - - - -| - - ->|
                                          |
    /pars-args                            |
        - pars-args.cpp  - - - - - - - - >|
                                          |
    /parser-tsv                           |
        - read-tsv.cpp  - - - - - - - - ->|
                                          |
    Root:                                 |
        main.cpp  <- - - - - - - - - - - -|


    Instruments:
    /functions
        - functions.cpp
        - functions.h

P.S special for Fedor Konstantinevich <3
*/

#include "../bmp/bmp_writer.cpp"
#include "../matrix/matrix.cpp"
#include "../pars-args/pars-args.cpp"
#include "../parser-tsv/read-tsv.cpp"
#include "../sandpile/sandpile.cpp"
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
    /* Место под переменные */
    const char *filename = nullptr;
    const char *output_file = nullptr;
    int max_iter = 0;
    int freq = 1;  // Значение по умолчанию для частоты
    int size_of_buffer = 512;
    char buffer[size_of_buffer];
    log_s logs;
    bool flag_for_topple;
    /* Конец переменных */

    /* Парсинг аргументов командной строки */
    ParsArgs(argc, argv, filename, output_file, max_iter, freq);

    /* Открытие файла */
    std::ifstream input_file(filename);

    /* Проверяем на открытие */
    if (!input_file.is_open()) {
        std::cerr << "File could not be opened" << '\n';
        return 1;
    }

    /* Инициализация матрицы песчаной кучи */
    DynamicMatrix sandpile_matrix; 
    Sandpile sandpile(sandpile_matrix); 

    while (input_file.getline(buffer, size_of_buffer)) {
        ParsTSVFile(buffer, logs); 
        sandpile_matrix.AddGrains(logs.width, logs.height,
                                  logs.count); 
    }

    /* Закрываем файл */
    input_file.close();

    /* Итеративная симуляция песчаной кучи */
    for (int current_iter = 0; current_iter < max_iter; ++current_iter) {
        if (current_iter % freq == 0) {
            flag_for_topple = true;
        } else {
            flag_for_topple = false;
        }
        /* Рассыпание песчаной кучи */
        sandpile.Topple();
        // Сохранение состояния в BMP, если установлен флаг
        if (flag_for_topple) {
            char bmp_filename[256];
            snprintf(bmp_filename, sizeof(bmp_filename),
                     "BMP_pictures_iteration_%d.bmp", current_iter);
            WriteBMP(bmp_filename, sandpile_matrix);
            std::cout << "BMP successfully written: " << bmp_filename
                      << std::endl;
        }
        /* Проверка на стабильность */
        if (sandpile.IsStable()) {
            std::cout << "Sandpile stabilized at iteration " << current_iter
                      << std::endl;
            sandpile_matrix.PrintMatrix();  // Печать стабильной матрицы
            break;
        }
    }
    return 0;
}