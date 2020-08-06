#include "token.hpp"

#include <string_view>

using namespace laserc;

uint64_t token::get_line() const noexcept {
    return line;
}

uint32_t token::get_column() const noexcept {
    return column;
}

std::string_view token::get_text() const noexcept {
    return text;
}
