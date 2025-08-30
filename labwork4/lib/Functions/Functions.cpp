#include "Functions.h"

#include <cassert>
#include <climits>
#include <iostream>

int StrToInt(const char* str, bool& success) {
    int result = 0;
    int sign = 1;    // Flag for negative numbers
    success = true;  // Convertation success flag
    int i = 0;

    while (str[i] == ' ') {
        i++;
    }

    // Checking + or -
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    for (; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            int digit = str[i] - '0';

            // Chech segmentations
            if (result > (INT_MAX - digit) / 10) {
                std::cerr << "Overflow error!" << std::endl;
                success = false;  // Set error flag
                return 0;         // return 0 when segmentation
            }

            result = result * 10 + digit;
        } else {
            std::cerr << "Invalid character encountered: " << str[i]
                      << std::endl;
            success = false;
            return 0;
        }
    }

    // Positive or negative
    return result * sign;
}