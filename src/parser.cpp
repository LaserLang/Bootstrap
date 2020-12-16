#include "parser.hpp"

#include <iostream>
#include <memory>
#include <string_view>

namespace laserc {

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
  std::unique_ptr<expression_without_block_node> ret_val;

  if((*token_it).get_text() != "{") {
    std::cerr << (*token_it).get_line() << ":" << (*token_it).get_column() << ": Syntax error: expected \"{\", got \"" << (*token_it).get_text() << "\"" << std::endl;
    std::exit(1);
  }
  while((*(token_it++)).get_text() != "}") {
    // TODO: parse statements
  }

  block_expression_node result(std::move(statements), std::move(ret_val));
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