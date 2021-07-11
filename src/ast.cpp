#include "ast.hpp"

#include <iostream>
#include <map>

using namespace cannon;
using namespace std::string_view_literals;

ast_node::~ast_node() {}
[[nodiscard]] uint64_t ast_node::get_line() const noexcept { return line; }
[[nodiscard]] uint32_t ast_node::get_column() const noexcept { return column; }

std::ostream &ast_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name();
    return os;
}

identifier_node::identifier_node(std::string value) noexcept : value(std::move(value)) {}
[[nodiscard]] std::string_view identifier_node::get_node_name() const noexcept {
    return "Identifier"sv;
}
[[nodiscard]] const std::string &identifier_node::get_value() const noexcept {
    return value;
}

type_node::type_node(std::unique_ptr<identifier_node> name) noexcept : name(std::move(name)) {}
[[nodiscard]] std::string_view type_node::get_node_name() const noexcept {
    return "Type"sv;
}

const identifier_node &type_node::get_name() const noexcept {
    return *name;
}

std::ostream &type_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << std::endl;
    os << indent << "Name: ";
    get_name().pretty_print(os, indent + "  ");
    return os;
}

parameter_node::parameter_node(
    std::unique_ptr<pattern_node> pattern,
    std::unique_ptr<type_node> type) noexcept : pattern(std::move(pattern)), type(std::move(type)) {}
[[nodiscard]] std::string_view parameter_node::get_node_name() const noexcept {
    return "Parameter"sv;
}

statement_node::~statement_node() {}

item_node::~item_node() {}

fn_node::fn_node(
    std::unique_ptr<identifier_node> name,
    std::vector<std::unique_ptr<parameter_node>> parameters,
    std::unique_ptr<type_node> return_type,
    std::unique_ptr<block_expression_node> code) noexcept
    : name(std::move(name)), parameters(std::move(parameters)), return_type(std::move(return_type)), code(std::move(code)) {}
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

std::ostream &fn_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << std::endl;
    os << indent << "Name: ";
    get_name().pretty_print(os, indent + "  ");
    os << std::endl;
    os << indent << "Parameters:" << std::endl;
    for (const auto &parameter : get_parameters()) {
        os << indent << "  ";
        parameter->pretty_print(os, indent + "    ");
        os << std::endl;
    }
    auto return_type = get_return_type();
    if (return_type) {
        os << indent << "Return type: ";
        return_type->pretty_print(os, indent + "  ");
        os << std::endl;
    } else {
        os << indent << "Return type: <null>" << std::endl;
    }
    os << indent << "Code: ";
    get_code().pretty_print(os, indent + "  ");
    return os;
}

expression_node::~expression_node() {}

block_expression_node::block_expression_node(std::vector<std::unique_ptr<statement_node>> statements) noexcept
    : statements(std::move(statements)) {}
[[nodiscard]] std::string_view block_expression_node::get_node_name() const noexcept {
    return "Block expression"sv;
}

const std::vector<std::unique_ptr<statement_node>> &block_expression_node::get_statements() const noexcept {
    return statements;
}

std::ostream &block_expression_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << std::endl;
    os << indent << "Statements:" << std::endl;
    std::string separator = "";
    for (const auto &statement : get_statements()) {
        os << separator << indent << "  ";
        statement->pretty_print(os, indent + "    ");
        separator = "\n";
    }
    return os;
  }

function_call_expression_node::function_call_expression_node(
    std::unique_ptr<expression_node> func,
    std::vector<std::unique_ptr<expression_node>> params) noexcept
    : func(std::move(func)), params(std::move(params)) {}

[[nodiscard]] std::string_view function_call_expression_node::get_node_name() const noexcept {
    return "Function call expression"sv;
}

const expression_node &function_call_expression_node::get_func() const noexcept {
    return *func;
}

const std::vector<std::unique_ptr<expression_node>> &function_call_expression_node::get_params() const noexcept {
    return params;
}

std::ostream &function_call_expression_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << std::endl;
    os << indent << "Function:" << std::endl << indent << "  ";
    get_func().pretty_print(os, indent + "  ");
    if (get_params().size() == 0) {
        os << std::endl << indent << "Parameters: (none)";
        return os;
    }
    os << std::endl << indent << "Parameters:" << std::endl;
    std::string separator = "";
    for (const auto &param : get_params()) {
        os << separator << indent << "  ";
        param->pretty_print(os, indent + "    ");
        separator = "\n";
    }
    return os;
}

integer_expression_node::integer_expression_node(int value) noexcept : value(value) {}
[[nodiscard]] std::string_view integer_expression_node::get_node_name() const noexcept {
    return "Integer"sv;
}

const int &integer_expression_node::get_value() const noexcept {
    return value;
}

std::ostream &integer_expression_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << " (" << get_value() << ")";
    return os;
}

identifier_expression_node::identifier_expression_node(std::unique_ptr<identifier_node> value) noexcept
    : value(std::move(value)) {}
[[nodiscard]] std::string_view identifier_expression_node::get_node_name() const noexcept {
    return "Identifier (expression)"sv;
}

const identifier_node &identifier_expression_node::get_value() const noexcept {
    return *value;
}

std::ostream &identifier_expression_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << " (" << get_value().get_value() << ")";
    return os;
}

double_expression_node::double_expression_node(double value) noexcept
    : value(value) {}
[[nodiscard]] std::string_view double_expression_node::get_node_name() const noexcept {
    return "Double"sv;
}

const double &double_expression_node::get_value() const noexcept {
    return value;
}

std::ostream &double_expression_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << " (" << get_value() << ")";
    return os;
}

binary_expression_node::binary_expression_node(
    std::unique_ptr<expression_node> lhs,
    binary_operator op,
    std::unique_ptr<expression_node> rhs) noexcept
    : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}
[[nodiscard]] std::string_view binary_expression_node::get_node_name() const noexcept {
    return "Binary expression"sv;
}

std::map<binary_operator, std::string_view> ops_strings = {
    {ADD, "+"sv},
    {SUB, "-"sv},
    {MUL, "*"sv},
    {DIV, "/"sv},
};

std::ostream &binary_expression_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << std::endl;
    os << indent << "LHS: ";
    get_lhs().pretty_print(os, indent + "  ");
    os << std::endl;
    os << indent << "Operator: \"" << ops_strings[get_op()] << "\"" << std::endl;
    os << indent << "RHS: ";
    get_rhs().pretty_print(os, indent + "  ");
    return os;
}

const expression_node &binary_expression_node::get_lhs() const noexcept {
    return *lhs;
}

const binary_operator &binary_expression_node::get_op() const noexcept {
    return op;
}

const expression_node &binary_expression_node::get_rhs() const noexcept {
    return *rhs;
}

file_node::file_node(std::vector<std::unique_ptr<item_node>> items) noexcept
    : items(std::move(items)) {}
[[nodiscard]] std::string_view file_node::get_node_name() const noexcept {
    return "File"sv;
}

const std::vector<std::unique_ptr<item_node>> &file_node::get_items() const noexcept {
    return items;
}

std::ostream &file_node::pretty_print(std::ostream &os, std::string indent) const noexcept {
    os << get_node_name() << std::endl;
    os << indent << "Items:" << std::endl;
    std::string separator = "";
    for (const auto &item : get_items()) {
        os << separator << indent << "  ";
        item->pretty_print(os, indent + "    ");
        separator = "\n";
    }
    return os;
}
