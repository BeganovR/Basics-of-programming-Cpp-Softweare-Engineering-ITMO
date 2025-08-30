#pragma once
#include <sstream>

int StrToInt(const char *str, bool &success);

template <typename Arg>
std::string ToStringMy(const Arg &value) {
    std::ostringstream os;
    os << value;
    return os.str();
}