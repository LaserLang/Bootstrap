#include "program.hpp"

#include <ostream>

namespace cannon {

statement::~statement() {}

expression::~expression() {}

identifier_expression::identifier_expression(std::string value): m_value(value) {}

void identifier_expression::do_print(std::ostream &os, std::string leftpad) {
    os << "Identifier (" << m_value << ")";
}

std::string identifier_expression::value() const {
    return m_value;
}

type identifier_expression::return_type() const {
    return type(type_id::I32);
}

integer_expression::integer_expression(int value): m_value(value) {}

void integer_expression::do_print(std::ostream &os, std::string leftpad) {
    os << "Integer (" << m_value << ")";
}

int integer_expression::value() const {
    return m_value;
}

type integer_expression::return_type() const {
    return type(type_id::I32);
}

binary_expression::binary_expression(std::unique_ptr<expression> lhs, binary_operator op, std::unique_ptr<expression> rhs, type return_type):
    m_lhs(std::move(lhs)), m_op(op), m_rhs(std::move(rhs)), m_return_type(return_type) {}

void binary_expression::do_print(std::ostream &os, std::string leftpad) {
    os << "Binary expression" << std::endl << leftpad;

    os << "LHS: ";
    m_lhs->do_print(os, leftpad + "  ");
    os << std::endl << leftpad;

    os << "Operator: ";
    switch(m_op) {
      case ADD:
        os << "+";
        break;
      case SUB:
        os << "-";
        break;
      case MUL:
        os << "*";
        break;
      case DIV:
        os << "/";
        break;
      default:
        os << "Unknown :sus:";
        break;
    }
    os << std::endl << leftpad;

    os << "RHS: ";
    m_rhs->do_print(os, leftpad + "  ");
    os << std::endl << leftpad;

    os << "Result type: " << m_return_type;
}

const expression& binary_expression::lhs() const {
    return *m_lhs;
}

const expression& binary_expression::rhs() const {
    return *m_rhs;
}

binary_operator binary_expression::op() const {
    return m_op;
}

type binary_expression::return_type() const {
    return m_return_type;
}

function_call_expression::function_call_expression(std::unique_ptr<expression> func, std::vector<std::unique_ptr<expression>> params):
    m_func(std::move(func)), m_params(std::move(params)) {}

const expression& function_call_expression::func() const {
    return *m_func;
}

const std::vector<std::unique_ptr<expression>>& function_call_expression::params() const {
    return m_params;
}

void function_call_expression::do_print(std::ostream &os, std::string leftpad) {
    os << "Function call expression" << std::endl << leftpad;

    os << "Function: ";
    m_func->do_print(os, leftpad + "  ");
    os << std::endl << leftpad;

    os << "Parameters:";
    if(m_params.size() == 0)
        os << " (none)";
    else for(auto &param : m_params) {
        os << std::endl << leftpad;
        param->do_print(os, leftpad + "  ");
    }
}

type function_call_expression::return_type() const {
    return type(type_id::I32);
}

void incomplete_identifier_expression::set_value(std::string value) {
    m_value = value;
}

identifier_expression incomplete_identifier_expression::to_identifier_expression() const {
    return identifier_expression(m_value);
}

std::unique_ptr<expression> incomplete_identifier_expression::to_expression_ptr() const {
    return std::make_unique<identifier_expression>(to_identifier_expression());
}

void incomplete_integer_expression::set_value(int value) {
    m_value = value;
}

integer_expression incomplete_integer_expression::to_integer_expression() const {
    return integer_expression(m_value);
}

std::unique_ptr<expression> incomplete_integer_expression::to_expression_ptr() const {
    return std::make_unique<integer_expression>(to_integer_expression());
}

void incomplete_function_call_expression::set_func(std::unique_ptr<incomplete_expression> func) {
    m_func = std::move(func);
}

void incomplete_function_call_expression::add_param(std::unique_ptr<incomplete_expression> param) {
    m_params.push_back(std::move(param));
}

function_call_expression incomplete_function_call_expression::to_function_call_expression() const {
    std::vector<std::unique_ptr<expression>> params;
    for(auto &param : m_params) {
        params.push_back(param->to_expression_ptr());
    }
    return function_call_expression(m_func->to_expression_ptr(), std::move(params));
}

std::unique_ptr<expression> incomplete_function_call_expression::to_expression_ptr() const {
    return std::make_unique<function_call_expression>(to_function_call_expression());
}

std::unique_ptr<statement> incomplete_expression::to_statement_ptr() const {
    return to_expression_ptr();
}

void incomplete_binary_expression::set_lhs(std::unique_ptr<incomplete_expression> lhs) {
    m_lhs = std::move(lhs);
}

void incomplete_binary_expression::set_op(binary_operator op) {
    m_op = op;
}

void incomplete_binary_expression::set_rhs(std::unique_ptr<incomplete_expression> rhs) {
    m_rhs = std::move(rhs);
}

void incomplete_binary_expression::set_return_type(incomplete_type return_type) {
    m_return_type = return_type;
}

binary_expression incomplete_binary_expression::to_binary_expression() const {
    return binary_expression(m_lhs->to_expression_ptr(), m_op, m_rhs->to_expression_ptr(), m_return_type.to_type());
}

std::unique_ptr<expression> incomplete_binary_expression::to_expression_ptr() const {
    return std::make_unique<binary_expression>(to_binary_expression());
}

incomplete_expression::~incomplete_expression() {}

incomplete_statement::~incomplete_statement() {}

void incomplete_type::set_name(std::string_view name) {
    m_name = name;
}

void incomplete_type::set_id(type_id id) {
    m_id = id;
}

type incomplete_type::to_type() const {
    return type(m_id);
}

void incomplete_function::set_name(std::string_view name) {
    m_name = name;
}

void incomplete_function::set_return_type(const incomplete_type &return_type) {
    m_return_type = &return_type;
}

void incomplete_function::set_ast(const fn_node &ast) {
    m_ast = &ast;
}

void incomplete_function::add_statement(std::unique_ptr<incomplete_statement> statement) {
    m_statements.push_back(std::move(statement));
}

const fn_node& incomplete_function::ast() const {
    return *m_ast;
}

const std::vector<std::unique_ptr<incomplete_statement>>& incomplete_function::statements() const {
    return m_statements;
}

std::unique_ptr<function> incomplete_function::to_function_ptr() const {
    std::vector<std::unique_ptr<statement>> statements;
    for(auto &s : m_statements) {
        statements.push_back(s->to_statement_ptr());
    }
    return std::make_unique<function>(m_return_type->to_type(), std::string(m_name), std::move(statements));
}

void incomplete_program::add_function(std::unique_ptr<incomplete_function> function) {
    m_functions.push_back(std::move(function));
}

const std::vector<std::unique_ptr<incomplete_function>>& incomplete_program::functions() const {
    return m_functions;
}

program incomplete_program::to_program() const {
    std::vector<std::unique_ptr<function>> functions;
    for(auto &f : m_functions) {
        functions.push_back(f->to_function_ptr());
    }
    return program(std::move(functions));
}

type::type(type_id id): m_id(id) {}

std::ostream& operator<<(std::ostream &os, const type &type) {
    switch(type.id()) {
      case type_id::Void:
        os << "()";
        break;
      case type_id::I32:
        os << "i32";
        break;
      case type_id::None:
      default:
        os << "Unknown/unresolved type";
        break;
    }
    return os;
}

type_id type::id() const {
    return m_id;
}

function::function(type return_type, std::string name, std::vector<std::unique_ptr<statement>> statements):
    m_return_type(return_type), m_name(name), m_statements(std::move(statements)) {}

const std::vector<std::unique_ptr<statement>>& function::statements() const {
    return m_statements;
}

std::ostream& operator<<(std::ostream &os, const function &function) {
    os << "    Function" << std::endl;
    os << "      Name: " << function.name() << std::endl;
    os << "      Return type: " << function.return_type() << std::endl;
    os << "      Statements:";
    for(const auto &statement : function.statements()) {
        os << std::endl << "        ";
        statement->do_print(os, "          ");
    }
    return os;
}

type function::return_type() const {
    return m_return_type;
}

const std::string& function::name() const {
    return m_name;
}

program::program(std::vector<std::unique_ptr<function>> functions): m_functions(std::move(functions)) {}

std::ostream& operator<<(std::ostream &os, const program &program) {
    os << "Program" << std::endl << "  Functions:";
    for(const auto &func : program.functions()) {
        os << std::endl << *func;
    }
    return os;
}

const std::vector<std::unique_ptr<function>>& program::functions() const {
    return m_functions;
}

}
