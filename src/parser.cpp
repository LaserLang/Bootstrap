#include "parser.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <string_view>

using namespace std::string_view_literals;

namespace cannon {

std::unique_ptr<expression_node> parse_expression(std::vector<token>::iterator &token_it); // Why are we using C++ again?

[[noreturn]] void syntax_error(token cur_token, std::string expected) {
    std::cerr << cur_token.get_line() << ":" << cur_token.get_column() << ": Syntax error: expected " << expected << ", got \"" << cur_token.get_text() << "\"" << std::endl;
    std::exit(1);
}

void expect(std::vector<token>::iterator &token_it, std::string expected, std::string description) {
    token cur_token = *token_it;
    if (cur_token.get_text() == expected) {
        token_it++;
    } else {
        syntax_error(cur_token, description);
    }
}

std::unique_ptr<expression_node> parse_primary_expression(std::vector<token>::iterator &token_it) {
    token cur_token = *(token_it++);
    switch (cur_token.get_type()) {
      case NUMBER: {
        int value = 0;
        for (char c : cur_token.get_text()) {
            // TODO: check for overflow
            value *= 10;
            value += (c - '0');
        }
        cur_token = *token_it;
        if (cur_token.get_text() == ".") {
            token_it++;
            token cur_token = *(token_it++);
            switch (cur_token.get_type()) {
              case NUMBER: {
                double value2 = 0;
                for (char c : cur_token.get_text()) {
                    value2 += (c - '0');
                    value2 /= 10;
                }
                cur_token = *token_it;
                if (cur_token.get_text() == ".") {
                    token_it++;
                }
                return std::make_unique<double_expression_node>(value+value2);
              }
              case IDENTIFIER:
                std::cerr << "Calling functions on numbers is not implemented!" << std::endl;
                std::exit(1);
              default:
                syntax_error(*token_it, "number or identifier");
            }
        } else {
            return std::make_unique<integer_expression_node>(value);
        }
      }
      case IDENTIFIER:
        return std::make_unique<identifier_expression_node>(std::make_unique<identifier_node>(cur_token.get_text()));
      default:
        if (cur_token.get_text() == "(") { // parenthesised expression
            auto result = parse_expression(token_it);
            expect(token_it, ")", "closing parenthesis");
            return result;
        }
        syntax_error(cur_token, "primary expression");
    }
}

std::map<std::string_view, std::pair<binary_operator, std::pair<uint8_t, uint8_t>>> operators {
    {"+"sv, {ADD, {1, 2}}},
    {"-"sv, {SUB, {1, 2}}},
    {"*"sv, {MUL, {3, 4}}},
    {"/"sv, {DIV, {3, 4}}},
};

std::unique_ptr<expression_node> parse_expression_bp(uint8_t min_bp, std::vector<token>::iterator &token_it) {
    // pratt parser
    // see https://matklad.github.io/2020/04/13/simple-but-powerful-pratt-parsing.html

    auto lhs = parse_primary_expression(token_it);

    while (true) {
        token lookahead = (*token_it);
        if (lookahead.get_text() == "(") { // function call
            token_it++; // skip "("
            std::vector<std::unique_ptr<expression_node>> params;
            lookahead = *token_it;
            while (lookahead.get_text() != ")") { // good ol' for abuse
                params.push_back(parse_expression(token_it));
                lookahead = *token_it;
                std::cout << "parsed expression, now at " << lookahead.get_line() << ":" << lookahead.get_column() << " '" << lookahead.get_text() << "'" << std::endl;
                if (lookahead.get_text() == ")") break;
                expect(token_it, ",", "comma");
                lookahead = *token_it;
                std::cout << "parsed comma, now at " << lookahead.get_line() << ":" << lookahead.get_column() << " '" << lookahead.get_text() << "'" << std::endl;
            }
            token_it++; // skip ")"
            lhs = std::make_unique<function_call_expression_node>(std::move(lhs), std::move(params));
        } else {
            if (!operators.contains(lookahead.get_text())) break;
            auto op = operators[lookahead.get_text()];
            uint8_t l_bp = op.second.first;
            uint8_t r_bp = op.second.second;
            if (l_bp < min_bp) break;
            token_it++; // skip operator
            auto rhs = parse_expression_bp(r_bp, token_it);
            lhs = std::make_unique<binary_expression_node>(std::move(lhs), op.first, std::move(rhs));
        }
    }

    return lhs;
}

std::unique_ptr<expression_node> parse_expression(std::vector<token>::iterator &token_it) {
    return parse_expression_bp(0, token_it);
}

identifier_node parse_identifier(std::vector<token>::iterator &token_it) {
    token cur_token = *token_it;
    if (cur_token.get_type() != IDENTIFIER)
        syntax_error(cur_token, "identifier");
    identifier_node result(cur_token.get_text());
    token_it++;
    return result;
}

type_node parse_type(std::vector<token>::iterator &token_it) {
    return type_node(std::make_unique<identifier_node>(parse_identifier(token_it)));
}

block_expression_node parse_block_expression(std::vector<token>::iterator &token_it) {
    std::vector<std::unique_ptr<statement_node>> statements;

    expect(token_it, "{", "\"{\"");
    while ((*token_it).get_text() != "}") {
        // TODO: parse non-expression statements
        statements.push_back(parse_expression(token_it));
    }
    token_it++; // skip "}"

    return block_expression_node(std::move(statements));
}

fn_node parse_fn(std::vector<token>::iterator &token_it) {
    expect(token_it, "fn", "function");

    std::unique_ptr<identifier_node> name = std::make_unique<identifier_node>(parse_identifier(token_it));

    expect(token_it, "(", "\"(\"");
    std::vector<std::unique_ptr<parameter_node>> parameters;
    while ((*token_it).get_text() != ")") {
        // TODO: parse parameters
        token_it++;
    }
    token_it++; // skip ")"

    std::unique_ptr<type_node> return_type;

    if ((*token_it).get_text() == "->") {
        token_it++;
        return_type = std::make_unique<type_node>(parse_type(token_it));
    }

    auto code = std::make_unique<block_expression_node>(parse_block_expression(token_it));

    return fn_node(std::move(name), std::move(parameters), std::move(return_type), std::move(code));
}

file_node parse_file(std::vector<token> tokens) {
    std::vector<std::unique_ptr<item_node>> items;

    auto token_it = tokens.begin();
    while (token_it != tokens.end()) {
        if ((*token_it).get_text() == "fn") {
            std::unique_ptr<item_node> item = std::make_unique<fn_node>(parse_fn(token_it));
            items.push_back(std::move(item));
        } else {
            syntax_error(*token_it, "function");
        }
    }

    return file_node(std::move(items));
}

}
