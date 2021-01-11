#ifndef LASERC_AST_HPP
#define LASERC_AST_HPP

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace laserc {

class ast_node {
  private:
    uint64_t line;
    uint32_t column;

  public:
    [[nodiscard]] uint64_t get_line() const noexcept;
    [[nodiscard]] uint32_t get_column() const noexcept;
    [[nodiscard]] virtual std::string_view get_node_name() const noexcept = 0;
    virtual std::ostream &do_print(std::ostream &os, std::string indent) const noexcept;
    friend std::ostream &operator<<(std::ostream &os, const ast_node &node) {
      return node.do_print(os, "  ");
    }
    virtual ~ast_node() = 0;
};

template <typename T> class value_node : public ast_node {
  public:
    [[nodiscard]] virtual const T &get_value() const noexcept = 0;
    virtual ~value_node() = 0;
    std::ostream &do_print(std::ostream &os, std::string indent) const noexcept override;
};

template <typename T> std::ostream &value_node<T>::do_print(std::ostream &os, std::string indent) const noexcept {
  os << get_node_name() << " (" << get_value() << ")";
  return os;
}

template <typename T> value_node<T>::~value_node() {}

class pattern_node : public ast_node { // Base interface
  public:
    virtual ~pattern_node() = 0;
};

class identifier_node : public value_node<std::string> {
  private:
    std::string value;

  public:
    identifier_node(std::string value) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    [[nodiscard]] const std::string &get_value() const noexcept override;
};

class identifier_pattern_node : public pattern_node {
  private:
    identifier_node pattern;

  public:
    identifier_pattern_node(std::string value) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
};

class type_node : public ast_node {
  private:
    std::unique_ptr<identifier_node> name;

  public:
    type_node(std::unique_ptr<identifier_node> name) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    [[nodiscard]] const identifier_node &get_name() const noexcept;
    std::ostream &do_print(std::ostream &os, std::string indent) const noexcept override;
};

class parameter_node : public ast_node {
  private:
    std::unique_ptr<pattern_node> pattern;
    std::unique_ptr<type_node> type;

  public:
    parameter_node(std::unique_ptr<pattern_node> pattern,
                   std::unique_ptr<type_node> type) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    [[nodiscard]] const pattern_node &get_pattern() const noexcept;
    [[nodiscard]] const type_node &get_type() const noexcept;
};

class statement_node : public ast_node { // Base interface
  public:
    virtual ~statement_node() = 0;
};

class expression_node : public statement_node { // Base interface
  public:
    virtual ~expression_node() = 0;
};

class expression_without_block_node : public expression_node { // Base interface
  public:
    virtual ~expression_without_block_node() = 0;
};

class item_node : public statement_node { // Base interface
  public:
    virtual ~item_node() = 0;
};

class block_expression_node : public expression_node {
  private:
    std::vector<std::unique_ptr<statement_node>> statements;

  public:
    block_expression_node(
        std::vector<std::unique_ptr<statement_node>> statements) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    [[nodiscard]] const std::vector<std::unique_ptr<statement_node>> &
    get_statements() const noexcept;
    std::ostream &do_print(std::ostream &os, std::string indent) const noexcept override;
};

enum binary_operator {
  ADD,
  SUB,
  MUL,
  DIV
};

class binary_expression_node : public expression_node {
  private:
    std::unique_ptr<expression_node> lhs;
    binary_operator op;
    std::unique_ptr<expression_node> rhs;

  public:
    binary_expression_node(std::unique_ptr<expression_node> lhs,
        binary_operator op,
        std::unique_ptr<expression_node> rhs) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    [[nodiscard]] const expression_node &get_lhs() const noexcept;
    [[nodiscard]] const binary_operator &get_op() const noexcept;
    [[nodiscard]] const expression_node &get_rhs() const noexcept;
    std::ostream &do_print(std::ostream &os, std::string indent) const noexcept override;
};

class integer_expression_node : public value_node<int>, public expression_node {
  private:
    int value; // I'll fix this later

  public:
    integer_expression_node(int value) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    [[nodiscard]] const int &get_value() const noexcept override;
    std::ostream &do_print(std::ostream &os, std::string indent) const noexcept override;
};

class double_expression_node : public expression_node, public value_node<double> {
  private:
    double value; // I'll fix this later

  public:
    double_expression_node(double value) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    [[nodiscard]] const double &get_value() const noexcept override;
    std::ostream &do_print(std::ostream &os, std::string indent) const noexcept override;
};

class fn_node : public item_node {
  private:
    std::unique_ptr<identifier_node> name;
    std::vector<std::unique_ptr<parameter_node>> parameters;
    std::unique_ptr<type_node> return_type; // NULLABLE
    std::unique_ptr<block_expression_node> code;

  public:
    fn_node(std::unique_ptr<identifier_node> name,
            std::vector<std::unique_ptr<parameter_node>> parameters,
            std::unique_ptr<type_node> return_type,
            std::unique_ptr<block_expression_node> code) noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    [[nodiscard]] const identifier_node &get_name() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<parameter_node>> &
    get_parameters() const noexcept;
    [[nodiscard]] const type_node *get_return_type() const noexcept;
    [[nodiscard]] const block_expression_node &get_code() const noexcept;
    std::ostream &do_print(std::ostream &os, std::string indent) const noexcept override;
};

class file_node : public ast_node {
  private:
    std::vector<std::unique_ptr<item_node>> items;

  public:
    file_node(std::vector<std::unique_ptr<item_node>> items) noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<item_node>> &get_items() const noexcept;
    [[nodiscard]] std::string_view get_node_name() const noexcept override;
    std::ostream &do_print(std::ostream &os, std::string indent) const noexcept override;
};

} // namespace laserc

#endif // LASERC_TOKEN_HPP
