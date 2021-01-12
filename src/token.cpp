#include "token.hpp"

#include <string>

namespace cannon {

uint64_t token::get_line() const noexcept { return line; }

uint32_t token::get_column() const noexcept { return column; }

std::string token::get_text() const noexcept { return text; }

token_type token::get_type() const noexcept { return type; }

}
