#include "token.hpp"

#include <string_view>

using namespace laserc;

uint64_t Token::get_line() const noexcept {
    return line;
}

uint32_t Token::get_column() const noexcept {
    return column;
}

std::string_view Token::get_text() const noexcept {
    return text;
}
