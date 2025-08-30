#include <cstdint>
#include <iostream>

/*Перевод из строки в int*/
int StrToInt(const char *str) {
    int result = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if ((str[i] >= '0') && (str[i] <= '9')) {
            result = result * 10 + (str[i] - '0');
        } else {
            break;
        }
    }
    return result;
}

/* Перевод из строки в uint64_t */
uint64_t StrToUint64(const char *str) {
    uint64_t result = 0;
    size_t length = std::strlen(str);

    if (str[0] == '-') {
        std::cerr << "Error: Invalid value, count could not be negative"
                  << std::endl;
        return -1;
    }

    for (size_t i = 0; i < length; ++i) {
        if (str[i] < '0' || str[i] > '9') {
            std::cerr << "Error: Invalid value, numbers must be from 0 to 9"
                      << std::endl;
            return -1;
        }
        result = result * 10 + (str[i] - '0');
    }

    return result;
}
