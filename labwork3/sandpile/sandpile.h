#pragma once
#include "../matrix/matrix.h"

class Sandpile {
   public:
    Sandpile(DynamicMatrix& matrix);
    void Topple();
    bool IsStable() const;

   private:
    DynamicMatrix& matrix;
};

