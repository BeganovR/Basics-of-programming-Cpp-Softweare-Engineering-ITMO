#include <iostream>

template <typename T>
class Smth {
   public:
    Smth(auto&& smth) : value_(std::forward(smth)) {}

   private:
    T value_;
};