#include "program.hpp"

namespace cannon {

type::type(type_id id): m_id(id) {}

type incomplete_type::to_type() const {
    return type(m_id);
}

function::function(type return_type, std::string_view name): m_return_type(return_type), m_name(name) {}

function incomplete_function::to_function() const {
    return function(m_return_type->to_type(), m_name);
}

program::program(std::vector<std::unique_ptr<function>> functions): m_functions(std::move(functions)) {}

program incomplete_program::to_program() const {
    std::vector<std::unique_ptr<function>> functions;
    for(const incomplete_function *incomp_fn : m_functions) {
        functions.push_back(std::make_unique<function>(incomp_fn->to_function()));
    }
    return program(std::move(functions));
}

}
