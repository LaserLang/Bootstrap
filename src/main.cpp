#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <llvm/ADT/Triple.h>
#include <string_view>
#include <vector>

#include <Config.hpp>

#include "ast.hpp"
#include "lex.hpp"
#include "mode.hpp"
#include "parser.hpp"
#include "semantic.hpp"
#include "token.hpp"

using namespace cannon;

int main(int argc, char *argv[]) {
    std::vector<std::string_view> opts(argv, argv + argc);
    std::string_view output{};
    std::vector<std::string_view> input_files{};
    auto mode{cannon::compiler_mode::CompileAndLink};
    auto output_type{cannon::link_type::Exec};
    std::string_view default_linker{CANNON_DEFAULT_LINKER};
    llvm::Triple target{CANNON_DEFAULT_TRIPLE};
    for (auto it = std::next(begin(opts)); it != end(opts);
         it++) { // ADL too OP
    }

    static_cast<void>(output);
    static_cast<void>(input_files);
    static_cast<void>(output_type);
    static_cast<void>(mode);
    static_cast<void>(default_linker);
    // FIXME: TEMPORARY CODE UNTIL OPTION PARSING IS DONE
    std::ifstream file(argv[1], std::ios::in);
    if (!file.is_open())
        return 1;
    std::vector<token> tokens = lex(file);
    file.close();

    std::cout << "Tokens: {" << std::endl;
    for (token t : tokens) {
        std::cout << "\t{" << t.get_line() << " " << t.get_column() << " "
                  << std::quoted(t.get_text()) << "}" << std::endl;
    }
    std::cout << "}" << std::endl;

    file_node parsed = parse_file(tokens);

    std::cout << "AST: " << parsed << std::endl;

    program analysed = analyze(std::move(parsed));

    std::cout << analysed << std::endl;

    return 0;
}
