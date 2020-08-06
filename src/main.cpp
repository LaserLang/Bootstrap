#include <iostream>
#include <string_view>
#include <vector>
#include <llvm/ADT/Triple.h>

#include <Config.hpp>

#include "mode.hpp"


int main(int argc, char *argv[])
{
    std::vector<std::string_view> opts(argv,argv+argc);
    std::string_view output{};
    std::vector<std::string_view> input_files{};
    laserc::CompilerMode mode{laserc::CompilerMode::CompileAndLink};
    laserc::LinkType output_type{laserc::LinkType::Exec};
    std::string_view default_linker{LASERC_DEFAULT_LINKER};
    llvm::Triple target{LASERC_DEFAULT_TRIPLE};
    for(auto it = begin(opts); it!=end(opts); it++) // ADL too OP
    {

    }
}
