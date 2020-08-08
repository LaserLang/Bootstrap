#ifndef LASERC_TOKEN_HPP
#define LASERC_TOKEN_HPP

#include <cstdint>

#include <string>

namespace laserc {

class token {
private:
    uint64_t line;
    uint32_t column;
    std::string text;
public:
    token(uint64_t line, uint32_t column, std::string text) noexcept : line(line), column(column), text(text) {}
    [[nodiscard]] uint64_t get_line() const noexcept;
    [[nodiscard]] uint32_t get_column() const noexcept;
    [[nodiscard]] std::string get_text() const noexcept;
};

}

#endif // LASERC_TOKEN_HPP
