#include "program.hpp"

#include <ostream>

namespace cannon {

void incomplete_type::set_name(std::string_view name) {
    m_name = name;
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

function incomplete_function::to_function() const {
    return function(m_return_type->to_type(), m_name);
}

void incomplete_program::add_function(const incomplete_function &function) {
    m_functions.push_back(&function);
}

program incomplete_program::to_program() const {
    std::vector<std::unique_ptr<function>> functions;
    for(const incomplete_function *incomp_fn : m_functions) {
        functions.push_back(std::make_unique<function>(incomp_fn->to_function()));
    }
    return program(std::move(functions));
}

type::type(type_id id): m_id(id) {}

std::ostream& operator<<(std::ostream &os, const type &type) {
    return os;
}

function::function(type return_type, std::string_view name): m_return_type(return_type), m_name(name) {}

std::ostream& operator<<(std::ostream &os, const function &function) {
    os << "    Function" << std::endl;
    os << "      Name: " << function.name() << std::endl;
    os << "      Return type: " << function.return_type() << std::endl;
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
    const std::vector<std::unique_ptr<function>> &functions = program.functions();
    for(auto func = functions.begin(); func < functions.end(); func++) {
        os << std::endl << **func;
    }
    return os;
}

const std::vector<std::unique_ptr<function>>& program::functions() const {
    return m_functions;
}

}
