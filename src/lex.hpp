#ifndef CANNON_LEX_HPP
#define CANNON_LEX_HPP

#include <iostream>
#include <vector>

#include "token.hpp"

namespace cannon {

std::vector<token> lex(std::istream &input);

}

#endif // CANNON_LEX_HPP
