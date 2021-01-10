#include "parser.hpp"

#include <iostream>
#include <memory>
#include <string_view>

namespace laserc {

std::unique_ptr<expression_node> parse_expression(std::vector<token>::iterator &token_it) {
  // I hate expression parsing
  // So, we basically just need to churn until we see ";", ",", or "}".

  bool expecting_val = true; // Right now, my goal is to make sure it's valid.
  uint8_t decimal_parsing_state = 0; // 0 = No decimal point, 1 = "x.", 2 = ""

  token cur_token = *token_it;

  while(cur_token.get_text() != ";" && cur_token.get_text() != "," && cur_token.get_text() != "}") {
    switch(cur_token.get_type()) {
    case NUMBER:
      if(!expecting_val) {
        std::cerr << cur_token.get_line() << ":" << cur_token.get_column() << ": Syntax error: expected symbol, got number" << std::endl;
        std::exit(1);
      }
      if(decimal_parsing_state == 1) decimal_parsing_state = 2;
      expecting_val = false;
      break;
    case SYMBOL:
      if(expecting_val) {
        std::cerr << cur_token.get_line() << ":" << cur_token.get_column() << ": Syntax error: expected number or identifier, got symbol" << std::endl;
        std::exit(1);
      }
      if(cur_token.get_text() == "-" || cur_token.get_text() == "/") {
        expecting_val = true;
        decimal_parsing_state = 0;
        break;
      } else if(cur_token.get_text() == ".") {
        if(decimal_parsing_state == 0) decimal_parsing_state = 1;
        else {
          std::cerr << cur_token.get_line() << ":" << cur_token.get_column() << ": Syntax error: single number has two decimal points" << std::endl;
          std::exit(1);
        }
        expecting_val = true;
        break;
      } else {
        std::cerr << "Incomplete expression parser: Symbol \"" << cur_token.get_text() << "\" was allowed through" << std::endl;
        std::exit(1);
      }
    default:
      std::cerr << "Incomplete expression parser: Token \"" << cur_token.get_text() << "\" was allowed through" << std::endl;
      std::exit(1);
    }
    cur_token = *(++token_it);
  }

  std::unique_ptr<expression_node> result(std::move(nullptr));
  return result;
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
