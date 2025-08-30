#ifndef BMP_WRITER_H
#define BMP_WRITER_H

#include "../matrix/matrix.h"

#pragma pack(push, 1)

/* Структура заголовка файла BMP */
struct BMP_file_header {
    uint16_t file_type;  // BM тип файла
    uint32_t file_size;  // размер файла в байтах
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset_data;  // смещение до начала данных изображения
};

/* Структура заголовка информации BMP */
struct BMP_info_header {
    uint32_t size;         // размер заголовка
    int32_t width;         // ширина изображения
    int32_t height;        // высота изображения
    uint16_t planes;       // количество плоскостей
    uint16_t bit_count;    // количество бит на пиксель
    uint32_t compression;  // без сжатия
    uint32_t size_image;  // размер изображения, 0 тк несжатое
    int32_t x_pixels_per_meter;  // горизонтальное разрешение пикселей на метр
    int32_t y_pixels_per_meter;  // вертикальное разрешение пикселей на метр
    uint32_t colors_used;  // количество используемых цветов
    uint32_t colors_important;  // все цвета важны
};

#pragma pack(pop)

// Функция для записи BMP-файла на основе матрицы
void WriteBMP(const char *filename, const DynamicMatrix &matrix);

#endif  // BMP_WRITER_H
