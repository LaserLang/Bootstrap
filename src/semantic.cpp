#include "semantic.hpp"

#include <iostream>
#include <unordered_map>

namespace cannon {

std::unique_ptr<incomplete_expression> convert_and_tag_expr(const expression_node &expr) {
    incomplete_binary_expression result;
    return std::make_unique<incomplete_binary_expression>(result);
}

program analyze(file_node file) {
    incomplete_program result;
    std::unordered_map<std::string_view, incomplete_type> incomp_types;
    // FUNCTION LISTING
    for(const auto &item : file.get_items()) {
        const fn_node *func = dynamic_cast<const fn_node*>(&(*item));
        incomplete_function result_fn;
        result_fn.set_name(func->get_name().get_value());
        std::string_view return_type_name = func->get_return_type()->get_name().get_value();
        if(!incomp_types.contains(return_type_name)) {
            incomplete_type t;
            t.set_name(return_type_name);
            incomp_types[return_type_name] = t;
        }
        result_fn.set_return_type(incomp_types[return_type_name]);
        result_fn.set_ast(*func);
        result.add_function(std::make_unique<incomplete_function>(std::move(result_fn)));
    }
    // EXPRESSION TAGGING
    for(auto &fn : result.functions()) {
        const fn_node &func = fn->ast();
        const std::vector<std::unique_ptr<statement_node>> &statements = func.get_code().get_statements();
        for(auto statement = statements.begin(); statement < statements.end(); statement++) {
            const expression_node *expr = dynamic_cast<const expression_node*>(&(**statement));
            std::unique_ptr<incomplete_expression> result_expr = convert_and_tag_expr(*expr);
            fn->add_statement(std::move(result_expr));
        }
    }
    // TYPE RESOLUTION
    for(auto &[name, type] : incomp_types) {
        if(name == "i32") {
            type.set_id(type_id::I32);
        } else {
            std::cerr << "I don't recognize \"" << name << "\" as a type!" << std::endl;
        }
    }
    return result.to_program();
}

} // namespace cannon
