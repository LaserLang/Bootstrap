#include "semantic.hpp"

namespace cannon {

program analyze(file_node file) {
    incomplete_program result;
    const std::vector<std::unique_ptr<item_node>> &items = file.get_items();
    for(auto item = items.begin(); item < items.end(); item++) {
        fn_node *func = dynamic_cast<fn_node*>(&(**item));
        incomplete_function result_fn;
        result_fn.set_name(func->get_name().get_value());
        incomplete_type t;
        t.set_name(func->get_return_type()->get_name().get_value());
        result_fn.set_return_type(t);
        result.add_function(result_fn);
    }
    return result.to_program();
}

} // namespace cannon
