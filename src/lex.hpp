#ifndef LASERC_LEX_HPP
#define LASERC_LEX_HPP

#include <iostream>
#include <vector>

#include "token.hpp"

namespace laserc {

std::vector<token>* lex(std::istream &file);

}

#endif // LASERC_LEX_HPP
