#include "../matrix/matrix.h"

#include <iostream>

DynamicMatrix::DynamicMatrix()
    : rows(1),
      cols(1),
      min_x(0),
      max_x(0),
      min_y(0),
      max_y(0),
      save_bmp_flag(false) {
    matrix = new uint64_t*[rows];
    matrix[0] = new uint64_t[cols]();
}

DynamicMatrix::DynamicMatrix(int initial_rows, int initial_cols)
    : rows(initial_rows),
      cols(initial_cols),
      min_x(0),
      max_x(initial_cols - 1),
      min_y(0),
      max_y(initial_rows - 1),
      save_bmp_flag(false) {
    matrix = new uint64_t*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new uint64_t[cols]();
    }
}

DynamicMatrix::~DynamicMatrix() { ClearOldMatrix(); }

void DynamicMatrix::AddGrains(int x, int y, uint64_t grains) {
    Expand(x, y);
    int row = y - min_y;
    int col = x - min_x;
    matrix[row][col] += grains;
}

uint64_t DynamicMatrix::Get(int x, int y) const {
    if (IsWithinBounds(x, y)) {
        return matrix[y - min_y][x - min_x];
    }
    return 0;
}

void DynamicMatrix::Set(int x, int y, uint64_t grains) {
    Expand(x, y);
    matrix[y - min_y][x - min_x] = grains;
}

int DynamicMatrix::GetWidth() const { return cols; }

int DynamicMatrix::GetHeight() const { return rows; }

int DynamicMatrix::GetMinX() const { return min_x; }

int DynamicMatrix::GetMinY() const { return min_y; }

void DynamicMatrix::PrintMatrix() const {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void DynamicMatrix::Expand(int x, int y) {
    if (x < min_x) ExpandLeft();
    if (x > max_x) ExpandRight();
    if (y < min_y) ExpandUp();
    if (y > max_y) ExpandDown();
}

void DynamicMatrix::ExpandLeft() {
    min_x -= 1;
    int new_cols = cols + 1;

    uint64_t** new_matrix = new uint64_t*[rows];
    for (int i = 0; i < rows; ++i) {
        new_matrix[i] = new uint64_t[new_cols]();
        for (int j = 0; j < cols; ++j) {
            new_matrix[i][j + 1] = matrix[i][j];
        }
    }

    ClearOldMatrix();
    matrix = new_matrix;
    cols = new_cols;
}

void DynamicMatrix::ExpandRight() {
    max_x += 1;
    int new_cols = cols + 1;

    uint64_t** new_matrix = new uint64_t*[rows];
    for (int i = 0; i < rows; ++i) {
        new_matrix[i] = new uint64_t[new_cols]();
        for (int j = 0; j < cols; ++j) {
            new_matrix[i][j] = matrix[i][j];
        }
    }

    ClearOldMatrix();
    matrix = new_matrix;
    cols = new_cols;
}

void DynamicMatrix::ExpandUp() {
    min_y -= 1;
    int new_rows = rows + 1;

    uint64_t** new_matrix = new uint64_t*[new_rows];
    for (int i = 0; i < new_rows; ++i) {
        new_matrix[i] = new uint64_t[cols]();
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            new_matrix[i + 1][j] = matrix[i][j];
        }
    }

    ClearOldMatrix();
    matrix = new_matrix;
    rows = new_rows;
}

void DynamicMatrix::ExpandDown() {
    max_y += 1;
    int new_rows = rows + 1;

    uint64_t** new_matrix = new uint64_t*[new_rows];
    for (int i = 0; i < new_rows; ++i) {
        new_matrix[i] = new uint64_t[cols]();
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            new_matrix[i][j] = matrix[i][j];
        }
    }

    ClearOldMatrix();
    matrix = new_matrix;
    rows = new_rows;
}

void DynamicMatrix::ClearOldMatrix() {
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

bool DynamicMatrix::IsWithinBounds(int x, int y) const {
    return x >= min_x && x <= max_x && y >= min_y && y <= max_y;
}
