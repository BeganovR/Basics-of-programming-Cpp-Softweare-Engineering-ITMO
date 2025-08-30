#include "../sandpile/sandpile.h"
#include "../bmp/bmp_writer.h"
#include <iostream>


Sandpile::Sandpile(DynamicMatrix& matrix) : matrix(matrix) {}

void Sandpile::Topple() {
    for (int i = matrix.GetMinY(); i <= matrix.GetHeight(); ++i) {
        for (int j = matrix.GetMinX(); j <= matrix.GetWidth(); ++j) {
            if (matrix.Get(j, i) >= 4) {
                uint64_t p_grains = matrix.Get(j, i) / 4;
                matrix.Set(j, i, matrix.Get(j, i) % 4);

                // Проверяем необходимость расширения для каждой стороны
                if (j + 1 > matrix.GetWidth())
                    matrix.Expand(j + 1, i);  // Право
                if (j - 1 < matrix.GetMinX()) matrix.Expand(j - 1, i);  // Лево
                if (i + 1 > matrix.GetHeight())
                    matrix.Expand(j, i + 1);  // Вниз
                if (i - 1 < matrix.GetMinY()) matrix.Expand(j, i - 1);  // Вверх

                // Распределяем песчинки по соседним ячейкам
                matrix.AddGrains(j + 1, i, p_grains);  // Право
                matrix.AddGrains(j - 1, i, p_grains);  // Лево
                matrix.AddGrains(j, i + 1, p_grains);  // Вниз
                matrix.AddGrains(j, i - 1, p_grains);  // Вверх
            }
        }
    }
}

bool Sandpile::IsStable() const {
    for (int y = matrix.GetMinY(); y <= matrix.GetHeight(); ++y) {
        for (int x = matrix.GetMinX(); x <= matrix.GetWidth(); ++x) {
            if (matrix.Get(x, y) > 3) {
                return false;
            }
        }
    }
    return true;
}
