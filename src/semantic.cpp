#include "semantic.hpp"

#include <iostream>
#include <unordered_map>

namespace cannon {

program analyze(file_node file) {
    incomplete_program result;
    const std::vector<std::unique_ptr<item_node>> &items = file.get_items();
    std::unordered_map<std::string_view, incomplete_type> incomp_types;
    for(auto item = items.begin(); item < items.end(); item++) {
        fn_node *func = dynamic_cast<fn_node*>(&(**item));
        incomplete_function result_fn;
        result_fn.set_name(func->get_name().get_value());
        std::string_view return_type_name = func->get_return_type()->get_name().get_value();
        if(!incomp_types.contains(return_type_name)) {
            incomplete_type t;
            t.set_name(return_type_name);
            incomp_types[return_type_name] = t;
        }
        result_fn.set_return_type(incomp_types[return_type_name]);
        result.add_function(result_fn);
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
