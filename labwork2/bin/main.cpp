#include "number.h"
#include <iostream>
// Преобразование uint239_t обратно в int для удобного отображения значений
uint32_t ToInt(const uint239_t& value) {
    uint32_t result = 0;
    for (int i = 3; i >= 0; --i) {
        result <<= 7;
        result |= (value.data[i] & 0x7F); // Извлекаем младшие 7 бит из каждого байта
    }
    return result;
}
int main() {
    /*
    uint239_t value;
    // Пример числа для тестирования проверка чисел в двоичном виде
    const char* testStr = "127";
    uint32_t shift = 238;

    // Тестируем FromString
    uint239_t number = FromString(testStr, shift);
    std::cout << "FromString result: " << number << std::endl;

    // Тестируем FromInt
    uint32_t intValue = 127;
    uint239_t numberFromInt = FromInt(intValue, shift);
    std::cout << "FromInt result: " << numberFromInt << std::endl;
    */
   /*
    uint239_t num_1 = FromInt(5, 2);
    uint239_t num_2 = FromInt(2, 2);
    uint239_t subResult = num_1 + num_2;
    std::cout << "5 (сдвиг 2) + 2 (сдвиг 2) = " << subResult << std::endl;
    std::cout << "Значение после сложения: " << ToInt(subResult) << std::endl;
    std::cout << "Сдвиг после сложения: " << GetShift(subResult) << std::endl;
    */
   /*
    uint239_t num_1 = FromInt(239, 3);
    uint239_t num_2 = FromInt(30, 5);
    uint239_t subResult = num_1 - num_2;

    std::cout << "239 (сдвиг 3) - 30 (сдвиг 5) = " << subResult << std::endl;
    std::cout << "Значение после вычитания: " << ToInt(subResult) << std::endl;
    std::cout << "Сдвиг после вычитания: " << GetShift(subResult) << std::endl;
*/
    /*uint239_t num1 = FromInt(123, 32);
    uint239_t num2 = FromInt(10, 4);

    uint239_t mulResult = num1 * num2;

    std::cout << "123 (сдвиг 6) * 10 (сдвиг 4) = " << mulResult << std::endl;
    std::cout << "Значение после умножения: " << ToInt(mulResult) << std::endl;
    std::cout << "Сдвиг после умножения: " << GetShift(mulResult) << std::endl;
    */
    uint239_t a = FromInt(42, 7); // Число 42 со сдвигом 7
    uint239_t b = FromInt(5, 2);  // Число 5 со сдвигом 2

    uint239_t result = a / b; // Результат деления
    std::cout << result << std::endl; // Ожидаемый результат: 8 со сдвигом 5

    return 0;

}
