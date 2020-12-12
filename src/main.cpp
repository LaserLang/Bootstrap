#include <fstream>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>
#include <llvm/ADT/Triple.h>

#include <Config.hpp>

#include "lex.hpp"
#include "mode.hpp"

using namespace laserc;

int main(int argc, char *argv[]) {
    std::vector<std::string_view> opts(argv,argv+argc);
    std::string_view output{};
    std::vector<std::string_view> input_files{};
    auto mode{laserc::compiler_mode::CompileAndLink};
    auto output_type{laserc::link_type::Exec};
    std::string_view default_linker{LASERC_DEFAULT_LINKER};
    llvm::Triple target{LASERC_DEFAULT_TRIPLE};
    for (auto it = begin(opts); it != end(opts); it++) { // ADL too OP

    }

    // FIXME: TEMPORARY CODE UNTIL OPTION PARSING IS DONE
    std::ifstream file(argv[1], std::ios::in);
    if (!file.is_open()) return 1;
    std::vector<token> *tokens = lex(file);
    file.close();

    std::cout << "Tokens: {" << std::endl;
    for (token t : *tokens) {
        std::cout << "\t{" << t.get_line() << " " << t.get_column() << " " << std::quoted(t.get_text()) << "}" << std::endl;
    }
    std::cout << "}" << std::endl;

    delete tokens;
    return 0;
}
