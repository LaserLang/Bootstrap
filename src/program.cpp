#include "program.hpp"

#include <ostream>

namespace cannon {

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

function incomplete_function::to_function() const {
    return function(m_return_type->to_type(), m_name);
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
        functions.push_back(std::make_unique<function>(f->to_function()));
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

function::function(type return_type, std::string_view name): m_return_type(return_type), m_name(name) {}

std::ostream& operator<<(std::ostream &os, const function &function) {
    os << "    Function" << std::endl;
    os << "      Name: " << function.name() << std::endl;
    os << "      Return type: " << function.return_type();
    return os;
}

type function::return_type() const {
    return m_return_type;
}

std::string_view function::name() const {
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
