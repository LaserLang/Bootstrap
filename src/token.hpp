#ifndef LASERC_TOKEN_HPP
#define LASERC_TOKEN_HPP

#include <cstdint>

#include <string_view>

namespace laserc {

class token {
private:
    uint64_t line;
    uint32_t column;
    std::string_view text;
public:
    Token(uint64_t line, uint32_t column, std::string_view text) noexcept : line(line), column(column), text(text) {}
    [[nodiscard]] uint64_t get_line()const noexcept;
    [[nodiscard]] uint32_t get_column()const noexcept;
    [[nodiscard]] std::string_view get_text()const noexcept;
};

}

#endif // LASERC_TOKEN_HPP
