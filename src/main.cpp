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
#include "codegen.hpp"
#include "lex.hpp"
#include "mode.hpp"
#include "parser.hpp"
#include "semantic.hpp"
#include "token.hpp"

using namespace cannon;
using namespace std::string_view_literals;

int main(int argc, char *argv[]) {
    std::vector<std::string_view> opts(argv, argv + argc);
    std::string_view output{"a.o"};
    std::vector<std::string_view> input_files{};
    std::vector<std::string_view> libs{};
    std::vector<std::string_view> libdirs{};
    auto mode{cannon::compiler_mode::CompileAndLink};
    auto output_type{cannon::link_type::Exec};
    optimization_level opt_level{0};
    std::string_view linker{CANNON_DEFAULT_LINKER};
    std::optional<std::string_view> sysroot{};
    std::string_view target{CANNON_DEFAULT_TRIPLE};
    for (auto it = std::next(begin(opts)); it != end(opts);
        it++) { // ADL too OP

        std::string_view opt{*it};
        if(opt=="--target"sv){
            it++;
            if(it == end(opts))
                std::exit(1);
            target = *it;
        }
        else if(opt.starts_with("--target="sv))
            target = opt.substr(9);
        else if(opt=="--sysroot"){
            it++;
            if(it == end(opts))
                std::exit(1);
            sysroot = *it;
        }
        else if(opt.starts_with("--sysroot="sv))
            sysroot = opt.substr(10);
        else if(opt.starts_with("-fuse-ld="sv))
            linker = opt.substr(10);
        else if(opt == "--compile-only"sv)
            mode = compiler_mode::CompileOnly;
        else if(opt == "-ftype-check"sv || opt == "--check"sv)
            mode = compiler_mode::TypeCheck;
        else if(opt == "-O"sv || opt == "-O2"sv)
            opt_level = optimization_level{2};
        else if(opt == "-O0"sv)
            opt_level = optimization_level{};
        else if(opt == "-O1"sv)
            opt_level = optimization_level{1};
        else if(opt == "-O3"sv)
            opt_level = optimization_level{3};
        else if(opt == "-Og"sv)
            opt_level = optimization_level::Debug;
        else if(opt == "-Os"sv)
            opt_level = optimization_level::Size;
        else if(opt == "-Oz"sv)
            opt_level = optimization_level::Zize;
        else if(opt == "-Ofast"sv)
            opt_level = optimization_level::Fast;
        else if(opt == "-Oextra"sv)
            opt_level = optimization_level::Extra;
        else if(opt == "-c"sv)
            mode = compiler_mode::CompileOnly;
        else if(opt == "-co"sv){
            mode = compiler_mode::CompileOnly;
            it++;
            if(it == end(opts))
                std::exit(1);
            output = *it;
        }else if(opt.starts_with("-co"sv)){
            mode = compiler_mode::CompileOnly;
            output = opt.substr(3);
        }else if(opt=="-o"sv){
            it++;
            if(it == end(opts))
                std::exit(1);
            output = *it;
        }else if(opt.starts_with("-o"sv))
            output = opt.substr(2);  
        else if(opt == "-L"sv){
            it++;
            if(it == end(opts))
                std::exit(1);
            libdirs.push_back(*it);
        }else if(opt.starts_with("-L"sv))
            libdirs.push_back(opt.substr(2));
        else if(opt == "-L"sv){
            it++;
            if(it == end(opts))
                std::exit(1);
            libdirs.push_back(*it);
        }else if(opt.starts_with("-L"sv))
            libdirs.push_back(opt.substr(2));
        else
            input_files.push_back(opt);
    }

    static_cast<void>(input_files);
    static_cast<void>(output_type);
    static_cast<void>(mode);
    static_cast<void>(linker);
    static_cast<void>(opt_level);
    for(auto&& a : input_files){
        std::ifstream file{std::string{a}};
        if(!file)
            std::exit(1);
        std::vector<token> tokens = lex(file);

        std::cout << "Tokens: {" << std::endl;
        for (token t : tokens) {
            std::cout << "\t{" << t.get_line() << " " << t.get_column() << " "
                    << std::quoted(t.get_text()) << "}" << std::endl;
        }
        std::cout << "}" << std::endl;

        file_node parsed = parse_file(tokens);

        std::cout << "AST: " << parsed << std::endl;

        program analysed = analyze(std::move(parsed));

        std::cout << "Analysed: " << analysed << std::endl;
        if(mode<compiler_mode::TypeCheck)
            codegen(std::move(analysed), std::string(output));
    }

    return 0;
}
