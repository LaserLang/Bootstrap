#include "program.hpp"

namespace cannon {

program incomplete_program::to_program() const {
    std::vector<std::unique_ptr<function>> functions;
    for(const incomplete_function *incomp_fn : m_functions) {
        functions.push_back(std::make_unique<function>(incomp_fn->to_function()));
    }
    return program(std::move(functions));
}

}
