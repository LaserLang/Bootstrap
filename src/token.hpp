#pragma once

#include <cstdint>

#include <string_view>

namespace laserc {

class Token {
private:
    uint64_t line;
    uint32_t column;
    std::string_view text;
public:
    Token() = delete;
    Token(uint64_t line, uint32_t column, std::string_view text) : line(line), column(column), text(text) {}
    uint64_t get_line();
    uint32_t get_column();
    std::string_view get_text();
};

}
