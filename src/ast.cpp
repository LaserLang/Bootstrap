#include "ast.hpp"

using namespace laserc;
using namespace std::string_view_literals;

ast_node::~ast_node() {}
[[nodiscard]] uint64_t ast_node::get_line() const noexcept {
  return line;
}
[[nodiscard]] uint32_t ast_node::get_column() const noexcept {
  return column;
}

identifier_node::identifier_node(std::string value) noexcept : value(std::move(value)) {}
[[nodiscard]] std::string_view identifier_node::get_node_name() const noexcept {
  return "Identifier"sv;
}
[[nodiscard]] const std::string &identifier_node::get_value() const noexcept {
  return value;
}

pattern_node::pattern_node(std::unique_ptr<identifier_node> name) noexcept
        : name(std::move(name)) {}
[[nodiscard]] std::string_view pattern_node::get_node_name() const noexcept {
  return "Pattern"sv;
}

type_node::type_node(std::unique_ptr<identifier_node> name) noexcept
        : name(std::move(name)) {}
[[nodiscard]] std::string_view type_node::get_node_name() const noexcept {
  return "Type"sv;
}

parameter_node::parameter_node(std::unique_ptr<pattern_node> pattern,
                   std::unique_ptr<type_node> type) noexcept
        : pattern(std::move(pattern)), type(std::move(type)) {}
[[nodiscard]] std::string_view parameter_node::get_node_name() const noexcept {
  return "Parameter"sv;
}

statement_node::~statement_node() {}

item_node::~item_node() {}

fn_node::fn_node(std::unique_ptr<identifier_node> name,
            std::vector<std::unique_ptr<parameter_node>> parameters,
            std::unique_ptr<block_expression_node> code) noexcept
        : name(std::move(name)), parameters(std::move(parameters)),
          code(std::move(code)) {}
[[nodiscard]] std::string_view fn_node::get_node_name() const noexcept {
  return "Function"sv;
}

block_expression_node::block_expression_node(
        std::vector<std::unique_ptr<statement_node>> statements,
        std::unique_ptr<expression_without_block_node> ret_val) noexcept
        : statements(std::move(statements)), ret_val(std::move(ret_val)) {}
[[nodiscard]] std::string_view block_expression_node::get_node_name() const noexcept {
  return "Block expression"sv;
}