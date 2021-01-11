#include "ast.hpp"

#include <iostream>

using namespace laserc;
using namespace std::string_view_literals;

ast_node::~ast_node() {}
[[nodiscard]] uint64_t ast_node::get_line() const noexcept { return line; }
[[nodiscard]] uint32_t ast_node::get_column() const noexcept { return column; }

std::ostream &ast_node::do_print(std::ostream &os) const noexcept {
  os << get_node_name();
  return os;
}

identifier_node::identifier_node(std::string value) noexcept
    : value(std::move(value)) {}
[[nodiscard]] std::string_view identifier_node::get_node_name() const noexcept {
    return "Identifier"sv;
}
[[nodiscard]] const std::string &identifier_node::get_value() const noexcept {
    return value;
}

type_node::type_node(std::unique_ptr<identifier_node> name) noexcept
    : name(std::move(name)) {}
[[nodiscard]] std::string_view type_node::get_node_name() const noexcept {
    return "Type"sv;
}

const identifier_node &type_node::get_name() const noexcept {
    return *name;
}

std::ostream &type_node::do_print(std::ostream &os) const noexcept {
  os << get_node_name() << " {" << std::endl;
  os << "Name: " << get_name() << std::endl;
  os << "}";
  return os;
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
                 std::unique_ptr<type_node> return_type,
                 std::unique_ptr<block_expression_node> code) noexcept
    : name(std::move(name)), parameters(std::move(parameters)),
      return_type(std::move(return_type)), code(std::move(code)) {}
[[nodiscard]] std::string_view fn_node::get_node_name() const noexcept {
    return "Function"sv;
}

const identifier_node &fn_node::get_name() const noexcept {
    return *name;
}

const std::vector<std::unique_ptr<parameter_node>> &fn_node::get_parameters() const noexcept {
    return parameters;
}

const type_node *fn_node::get_return_type() const noexcept {
    return return_type.get();
}

const block_expression_node &fn_node::get_code() const noexcept {
    return *code;
}

std::ostream &fn_node::do_print(std::ostream &os) const noexcept {
  os << get_node_name() << " {" << std::endl;
  os << "Name: " << get_name() << std::endl;
  os << "Parameters: [" << std::endl;
  for(const auto &parameter : get_parameters()) {
    os << *parameter << std::endl;
  }
  os << "]" << std::endl;
  auto return_type = get_return_type();
  if(return_type)
    os << "Return type: " << *return_type << std::endl;
  else
    os << "Return type: <null>" << std::endl;
  os << "Code: " << get_code() << std::endl;
  os << "}";
  return os;
}

expression_node::~expression_node() {}

block_expression_node::block_expression_node(
    std::vector<std::unique_ptr<statement_node>> statements) noexcept
    : statements(std::move(statements)) {}
[[nodiscard]] std::string_view
block_expression_node::get_node_name() const noexcept {
    return "Block expression"sv;
}

const std::vector<std::unique_ptr<statement_node>> &block_expression_node::get_statements() const noexcept {
    return statements;
}

std::ostream &block_expression_node::do_print(std::ostream &os) const noexcept {
  os << get_node_name() << " {" << std::endl;
  os << "Statements: [" << std::endl;
  for(const auto &statements : get_statements()) {
    os << *statements << std::endl;
  }
  os << "]" << std::endl;
  os << "}";
  return os;
}

integer_expression_node::integer_expression_node(
    int value) noexcept
    : value(value) {}
[[nodiscard]] std::string_view
integer_expression_node::get_node_name() const noexcept {
    return "Integer"sv;
}

const int &integer_expression_node::get_value() const noexcept {
    return value;
}

double_expression_node::double_expression_node(
    double value) noexcept
    : value(value) {}
[[nodiscard]] std::string_view
double_expression_node::get_node_name() const noexcept {
    return "Double"sv;
}

const double &double_expression_node::get_value() const noexcept {
    return value;
}

binary_expression_node::binary_expression_node(
    std::unique_ptr<expression_node> lhs,
    binary_operator op,
    std::unique_ptr<expression_node> rhs) noexcept
    : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}
[[nodiscard]] std::string_view
binary_expression_node::get_node_name() const noexcept {
    return "Binary expression"sv;
}

file_node::file_node(std::vector<std::unique_ptr<item_node>> items) noexcept
    : items(std::move(items)) {}
[[nodiscard]] std::string_view file_node::get_node_name() const noexcept {
    return "File"sv;
}

const std::vector<std::unique_ptr<item_node>> &file_node::get_items() const noexcept {
    return items;
}

std::ostream &file_node::do_print(std::ostream &os) const noexcept {
  os << get_node_name() << " {" << std::endl;
  os << "Items: [" << std::endl;
  for(const auto &item : get_items()) {
    os << *item << std::endl;
  }
  os << "]" << std::endl;
  os << "}";
  return os;
}
