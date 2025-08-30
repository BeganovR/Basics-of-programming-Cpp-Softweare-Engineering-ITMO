#include "../bmp/bmp_writer.h"
#include "../matrix/matrix.h"
#include <fstream>
#include <iostream>

void WriteBMP(const char *filename, const DynamicMatrix &matrix) {
    int width = matrix.GetWidth();
    int height = matrix.GetHeight();
    int min_x = matrix.GetMinX();
    int min_y = matrix.GetMinY();
    int row_size = ((width + 1) / 2 + 3) & ~3;  // Размер строки, кратный 4
    int image_size = row_size * height;

    BMP_file_header file_header;
    BMP_info_header info_header;

    file_header.file_type = 0x4D42;  // "BM"
    file_header.file_size =
        sizeof(BMP_file_header) + sizeof(BMP_info_header) + 16 * 4 + image_size;
    file_header.reserved1 = 0;
    file_header.reserved2 = 0;
    file_header.offset_data =
        sizeof(BMP_file_header) + sizeof(BMP_info_header) + 16 * 4;

    info_header.size = sizeof(BMP_info_header);
    info_header.width = width;
    info_header.height = height;
    info_header.planes = 1;
    info_header.bit_count = 4;
    info_header.compression = 0;
    info_header.size_image = image_size;
    info_header.x_pixels_per_meter = 2835;
    info_header.y_pixels_per_meter = 2835;
    info_header.colors_used = 5;
    info_header.colors_important = 0;

    // Открытие файла для записи
    std::ofstream output_file(filename, std::ios::out | std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "File could not be opened" << std::endl;
        return;
    }

    output_file.write(reinterpret_cast<char *>(&file_header),
                      sizeof(file_header));
    output_file.write(reinterpret_cast<char *>(&info_header),
                      sizeof(info_header));

    uint8_t colors[5][4] = {
        {255, 255, 255, 0},  // Белый
        {0, 255, 0, 0},      // Зеленый
        {128, 0, 128, 0},    // Фиолетовый
        {255, 255, 0, 0},    // Желтый
        {0, 0, 0, 0}         // Черный
    };

    for (int i = 0; i < 5; ++i) {
        output_file.put(colors[i][2]);  // B
        output_file.put(colors[i][1]);  // G
        output_file.put(colors[i][0]);  // R
        output_file.put(colors[i][3]);  // A
    }

    // Дополнение оставшихся цветов черным
    for (int i = 5; i < 16; ++i) {
        output_file.put(0);  // Черный
        output_file.put(0);
        output_file.put(0);
        output_file.put(0);
    }

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; x += 2) {
            uint8_t pixel1 = matrix.Get(x + min_x, y + min_y);
            uint8_t pixel2 =
                (x + 1 < width) ? matrix.Get(x + 1 + min_x, y + min_y) : 0;

            pixel1 = (pixel1 > 3) ? 4 : pixel1;  // Ограничение
            pixel2 = (pixel2 > 3) ? 4 : pixel2;

            uint8_t byte = (pixel1 << 4) | (pixel2 & 0xF);
            output_file.put(byte);
        }

        // Дополнение до кратного 4 для размера строки
        for (int p = 0; p < row_size - (width + 1) / 2; ++p) {
            output_file.put(0);
        }
    }

    output_file.close();
    std::cout << "BMP successfully written: " << filename << std::endl;
}
