#include "token.hpp"

#include <string_view>

using namespace laserc;

uint64_t Token::get_line() {
    return line;
}

uint32_t Token::get_column() {
    return column;
}

std::string_view Token::get_text() {
    return text;
}
