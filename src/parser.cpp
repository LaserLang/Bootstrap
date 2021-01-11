#include "parser.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <string_view>

using namespace std::string_view_literals;

namespace laserc {

std::unique_ptr<expression_node> parse_primary_expression(std::vector<token>::iterator &token_it) {
  token cur_token = *(token_it++);
  switch(cur_token.get_type()) {
  case NUMBER:
  {
    int value = 0;
    for(char c : cur_token.get_text()) {
      value *= 10;
      value += (c - '0');
    }
    cur_token = *token_it;
    if(cur_token.get_text() == ".") {
      token_it++;
      token cur_token = *(token_it++);
      switch(cur_token.get_type()) {
      case NUMBER:
      {
        double value2 = 0;
        for(char c : cur_token.get_text()) {
          value2 += (c - '0');
          value2 /= 10;
        }
        cur_token = *token_it;
        if(cur_token.get_text() == ".") {
          token_it++;
        }
        return std::make_unique<double_expression_node>(double_expression_node(value+value2));
      }
      case IDENTIFIER:
        std::cerr << "Calling functions on numbers is unimplemented!" << std::endl;
        std::exit(1);
      default:
        std::cerr << (*token_it).get_line() << ":" << (*token_it).get_column() << ": Syntax error: expected number or identifier, got \"" << cur_token.get_text() << "\"" << std::endl;
        std::exit(1);
      }
    } else {
      return std::make_unique<integer_expression_node>(integer_expression_node(value));
    }
  }
  case IDENTIFIER:
    std::cerr << "Variables in expressions are unimplemented!" << std::endl;
    std::exit(1);
  default:
    std::cerr << cur_token.get_line() << ":" << cur_token.get_column() << ": Syntax error: expected number or identifier, got \"" << cur_token.get_text() << "\"" << std::endl;
    std::exit(1);
  }
}

std::map<std::string_view, std::pair<uint8_t, binary_operator>> operators {
  {"+"sv, {1, ADD}},
  {"-"sv, {1, SUB}},
  {"*"sv, {2, MUL}},
  {"/"sv, {2, DIV}}
};

std::unique_ptr<expression_node> parse_expression0(std::unique_ptr<expression_node> lhs, uint8_t min_precedence, std::vector<token>::iterator &token_it) {
  // I hate expression parsing
  // So, we basically just need to churn until we see ";", ",", ")", or "}".

  std::unique_ptr<expression_node> result(std::move(lhs));

  token lookaheadt = *token_it;
  while(1) {
    if(!operators.contains(lookaheadt.get_text())) {
      if(lookaheadt.get_text() == ";" || lookaheadt.get_text() == "," || lookaheadt.get_text() == ")" || lookaheadt.get_text() == "}")
        return result;
      std::cerr << lookaheadt.get_line() << ":" << lookaheadt.get_column() << ": Syntax error: expected operator, comma, end of block, end of group, or end of statement, got \"" << lookaheadt.get_text() << "\"" << std::endl;
      std::exit(1);
    }
    std::pair<int, binary_operator> lookahead = operators[lookaheadt.get_text()];
    if(lookahead.first < min_precedence) return result;
    std::pair<int, binary_operator> op = lookahead;
    token_it++;
    std::unique_ptr<expression_node> rhs = parse_primary_expression(token_it);
    while(1) {
      lookaheadt = *token_it;
      if(!operators.contains(lookaheadt.get_text())) {
        if(lookaheadt.get_text() == ";" || lookaheadt.get_text() == "," || lookaheadt.get_text() == ")" || lookaheadt.get_text() == "}")
          break;
        std::cerr << lookaheadt.get_line() << ":" << lookaheadt.get_column() << ": Syntax error: expected operator, comma, end of block, end of group, or end of statement, got \"" << lookaheadt.get_text() << "\"" << std::endl;
        std::exit(1);
      }
      lookahead = operators[lookaheadt.get_text()];
      if(lookahead.first <= op.first) break;
      rhs = parse_expression0(std::move(rhs), lookahead.first, token_it);
    }
    result = std::make_unique<binary_expression_node>(binary_expression_node(std::move(result), op.second, std::move(rhs)));
  }
}

std::unique_ptr<expression_node> parse_expression(std::vector<token>::iterator &token_it) {
  return parse_expression0(parse_primary_expression(token_it), 0, token_it);
}

identifier_node parse_identifier(std::vector<token>::iterator &token_it) {
  identifier_node result((*token_it).get_text());
  token_it++;
  return result;
}

type_node parse_type(std::vector<token>::iterator &token_it) {
  type_node result(std::make_unique<identifier_node>(parse_identifier(token_it)));
  return result;
}

block_expression_node parse_block_expression(std::vector<token>::iterator &token_it) {
  std::vector<std::unique_ptr<statement_node>> statements;

  if((*token_it).get_text() != "{") {
    std::cerr << (*token_it).get_line() << ":" << (*token_it).get_column() << ": Syntax error: expected \"{\", got \"" << (*token_it).get_text() << "\"" << std::endl;
    std::exit(1);
  }
  while((*(token_it++)).get_text() != "}") {
    // TODO: parse non-expression statements
    statements.push_back(parse_expression(token_it));
  }

  block_expression_node result(std::move(statements));
  return result;
}

fn_node parse_fn(std::vector<token>::iterator &token_it) {
  if((*token_it).get_text() != "fn") {
    std::cerr << (*token_it).get_line() << ":" << (*token_it).get_column() << ": Syntax error: expected function, got \"" << (*token_it).get_text() << "\"" << std::endl;
    std::exit(1);
  }
  token_it++;

  std::unique_ptr<identifier_node> name = std::make_unique<identifier_node>(parse_identifier(token_it));
  
  if((*token_it).get_text() != "(") {
    std::cerr << (*token_it).get_line() << ":" << (*token_it).get_column() << ": Syntax error: expected \"(\", got \"" << (*token_it).get_text() << "\"" << std::endl;
    std::exit(1);
  }
  std::vector<std::unique_ptr<parameter_node>> parameters;
  while((*(token_it++)).get_text() != ")") {
    // TODO: parse parameters
  }

  std::unique_ptr<type_node> return_type;

  if((*token_it).get_text() == "->") {
    token_it++;
    return_type = std::make_unique<type_node>(parse_type(token_it));
  }

  std::unique_ptr<block_expression_node> code = std::make_unique<block_expression_node>(parse_block_expression(token_it));

  fn_node result(std::move(name), std::move(parameters), std::move(return_type), std::move(code));

  return result;
}

file_node parse_file(std::vector<token> tokens) {
  std::vector<std::unique_ptr<item_node>> items;

  auto token_it = tokens.begin();
  while(token_it != tokens.end()) {
    if((*token_it).get_text() == "//") continue;
    if((*token_it).get_text() == "fn") {
      std::unique_ptr<item_node> item = std::make_unique<fn_node>(parse_fn(token_it));
      items.push_back(std::move(item));
    } else {
      std::cerr << (*token_it).get_line() << ":" << (*token_it).get_column() << ": Syntax error: expected function, got \"" << (*token_it).get_text() << "\"" << std::endl;
      std::exit(1);
    }
  }

  file_node result(std::move(items));

  return result;
}

}
