#ifndef MATRIX_H
#define MATRIX_H

#include <cstdint>

class DynamicMatrix {
   public:
    DynamicMatrix();  // Конструктор по умолчанию
    DynamicMatrix(int initial_rows,
                  int initial_cols);  // Конструктор с параметрами
    ~DynamicMatrix();

    void AddGrains(int x, int y, uint64_t grains);
    uint64_t Get(int x, int y) const;
    void Set(int x, int y, uint64_t grains);
    int GetWidth() const;
    int GetHeight() const;
    int GetMinX() const;
    int GetMinY() const;
    void PrintMatrix() const;
    void Expand(int x, int y);
    void ExpandLeft();
    void ExpandRight();
    void ExpandUp();
    void ExpandDown();

   private:
    int rows, cols;
    int min_x, max_x, min_y, max_y;
    uint64_t **matrix;
    bool save_bmp_flag;

    bool IsWithinBounds(int x, int y) const;
    void ClearOldMatrix();
};

#endif