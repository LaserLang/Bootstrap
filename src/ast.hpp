#ifndef LASERC_AST_HPP
#define LASERC_AST_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace laserc {

class ast_node { // Don't know if we need any common properties or not
};

class identifier_node : public ast_node {
  private:
    std::string value;

  public:
    identifier_node(std::string value) noexcept : value(std::move(value)) {}
    [[nodiscard]] const std::string &get_value() const noexcept;
};

class pattern_node : public ast_node {
  private:
    std::unique_ptr<identifier_node> name;

  public:
    pattern_node(std::unique_ptr<identifier_node> name) noexcept
        : name(std::move(name)) {}
    [[nodiscard]] const identifier_node &get_name() const noexcept;
};

class type_node : public ast_node {
  private:
    std::unique_ptr<identifier_node> name;

  public:
    type_node(std::unique_ptr<identifier_node> name) noexcept
        : name(std::move(name)) {}
    [[nodiscard]] const identifier_node &get_name() const noexcept;
};

class parameter_node : public ast_node {
  private:
    std::unique_ptr<pattern_node> pattern;
    std::unique_ptr<type_node> type;

  public:
    parameter_node(std::unique_ptr<pattern_node> pattern,
                   std::unique_ptr<type_node> type) noexcept
        : pattern(std::move(pattern)), type(std::move(type)) {}
    [[nodiscard]] const pattern_node &get_pattern() const noexcept;
    [[nodiscard]] const type_node &get_type() const noexcept;
};

class block_expression_node;

class statement_node : public ast_node { // Base interface
};

class item_node : public statement_node { // Base interface
};

class fn_node : public item_node {
  private:
    std::unique_ptr<identifier_node> name;
    std::vector<std::unique_ptr<parameter_node>> parameters;
    std::unique_ptr<block_expression_node> code;

  public:
    fn_node(std::unique_ptr<identifier_node> name,
            std::vector<std::unique_ptr<parameter_node>> parameters,
            std::unique_ptr<block_expression_node> code) noexcept
        : name(std::move(name)), parameters(std::move(parameters)),
          code(std::move(code)) {}
    [[nodiscard]] const identifier_node &get_name() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<parameter_node>> &
    get_parameters() const noexcept;
    [[nodiscard]] const block_expression_node &get_code() const noexcept;
};

class expression_without_block_node : public statement_node { // Base interface
};

class block_expression_node : public ast_node {
  private:
    std::vector<std::unique_ptr<statement_node>> statements;
    std::unique_ptr<expression_without_block_node> ret_val; // NULLABLE

  public:
    block_expression_node(
        std::vector<std::unique_ptr<statement_node>> statements,
        std::unique_ptr<expression_without_block_node> ret_val) noexcept
        : statements(std::move(statements)), ret_val(std::move(ret_val)) {}
    [[nodiscard]] const std::vector<std::unique_ptr<statement_node>> &
    get_statements() const noexcept;
    [[nodiscard]] expression_without_block_node *get_ret_val() const noexcept;
};

} // namespace laserc

#endif // LASERC_TOKEN_HPP
