#ifndef LASERC_PARSER_HPP
#define LASERC_PARSER_HPP

#include <vector>

#include "ast.hpp"
#include "token.hpp"

namespace laserc {

file_node parse_file(std::vector<token> tokens);

}

#endif // LASERC_PARSER_HPP