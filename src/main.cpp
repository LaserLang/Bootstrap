#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>
#include <llvm/ADT/Triple.h>

#include <Config.hpp>

#include "lex.hpp"
#include "mode.hpp"

using namespace laserc;

int main(int argc, char *argv[])
{
    std::vector<std::string_view> opts(argv,argv+argc);
    std::string_view output{};
    std::vector<std::string_view> input_files{};
    laserc::CompilerMode mode{laserc::CompilerMode::CompileAndLink};
    laserc::LinkType output_type{laserc::LinkType::Exec};
    std::string_view default_linker{LASERC_DEFAULT_LINKER};
    llvm::Triple target{LASERC_DEFAULT_TRIPLE};
    for(auto it = begin(opts); it != end(opts); it++) // ADL too OP
    {

    }

    // FIXME: TEMPORARY CODE UNTIL OPTION PARSING IS DONE
    std::ifstream file(argv[1], std::ios::in);
    if(file.is_open()) return 1;
    std::vector tokens = lex(file);
    file.close();
    return 0;
}
