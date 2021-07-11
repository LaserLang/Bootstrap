#ifndef CANNON_TOKEN_HPP
#define CANNON_TOKEN_HPP

#include <cstdint>

#include <string>

namespace cannon {

enum token_type {
  SYMBOL,
  NUMBER,
  IDENTIFIER,
};

class token {
  private:
    uint64_t line;
    uint32_t column;
    std::string text;
    token_type type;

  public:
    token(uint64_t line, uint32_t column, std::string text, token_type type) noexcept
        : line(line), column(column), text(text), type(type) {}
    [[nodiscard]] uint64_t get_line() const noexcept;
    [[nodiscard]] uint32_t get_column() const noexcept;
    [[nodiscard]] std::string get_text() const noexcept;
    [[nodiscard]] token_type get_type() const noexcept;
};

} // namespace cannon

#endif // CANNON_TOKEN_HPP
