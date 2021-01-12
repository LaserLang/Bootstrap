#ifndef CANNON_PARSER_HPP
#define CANNON_PARSER_HPP

#include <vector>

#include "ast.hpp"
#include "token.hpp"

namespace cannon {

file_node parse_file(std::vector<token> tokens);

}

#endif // CANNON_PARSER_HPP
