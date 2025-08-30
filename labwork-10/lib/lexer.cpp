#include "lexer.h"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const Token& token) {
    return os << "{Type: " << static_cast<int>(token.type) << ", Value: \""
              << token.value << "\", Pos: (" << token.position.line << ", "
              << token.position.column << ")}";
}