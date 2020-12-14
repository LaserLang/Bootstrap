#ifndef LASERC_AST_HPP
#define LASERC_AST_HPP

#include <optional>
#include <string>
#include <vector>

namespace laserc {

class ast_node { // Don't know if we need any common properties or not
};

class identifier_node : public ast_node {
private:
	std::string *value;
public:
	identifier_node(std::string *value) noexcept: value(value) {}
    [[nodiscard]] std::string* get_value() const noexcept;
};

class pattern_node : public ast_node {
private:
	identifier_node *name;
public:
	pattern_node(identifier_node *name) noexcept: name(name) {}
    [[nodiscard]] identifier_node* get_name() const noexcept;
};

class type_node : public ast_node {
private:
	identifier_node *name;
public:
	type_node(identifier_node *name) noexcept: name(name) {}
    [[nodiscard]] identifier_node* get_name() const noexcept;
};

class parameter_node : public ast_node {
private:
	pattern_node *pattern;
	type_node *type;
public:
	parameter_node(pattern_node *pattern, type_node *type) noexcept: pattern(pattern), type(type) {}
    [[nodiscard]] pattern_node* get_pattern() const noexcept;
    [[nodiscard]] type_node* get_type() const noexcept;
};

class block_expression_node;

class fn_node : public ast_node {
private:
	identifier_node *name;
	std::vector<parameter_node*> parameters;
	block_expression_node *code;
public:
	fn_node(identifier_node *name, std::vector<parameter_node*> parameters, block_expression_node *code) noexcept: name(name), parameters(parameters), code(code) {}
    [[nodiscard]] identifier_node* get_name() const noexcept;
    [[nodiscard]] std::vector<parameter_node*> get_parameters() const noexcept;
    [[nodiscard]] block_expression_node* get_code() const noexcept;
};

class item_node : public ast_node {
private:
	std::optional<fn_node*> fn;
public:
	item_node(std::optional<fn_node*> fn) noexcept: fn(fn) {}
    [[nodiscard]] identifier_node* get_fn() const noexcept;
};

class let_statement_node;
class expression_node;

class statement_node : public ast_node {
private:
	std::optional<item_node*> item;
	std::optional<let_statement_node*> let_statement;
	std::optional<expression_node*> expression; // Includes expression_without_block
public:
	statement_node(std::optional<item_node*> item, std::optional<let_statement_node*> let_statement, std::optional<expression_node*> expression) noexcept: item(item), let_statement(let_statement), expression(expression) {}
    [[nodiscard]] std::optional<item_node*> get_item() const noexcept;
    [[nodiscard]] std::optional<let_statement_node*> get_let_statement() const noexcept;
    [[nodiscard]] std::optional<expression_node*> get_expression() const noexcept;
};

class expression_without_block_node;

class block_expression_node : public ast_node {
private:
	std::vector<statement_node*> statements;
	std::optional<expression_without_block_node*> ret_val;
public:
	block_expression_node(std::vector<statement_node*> statements, std::optional<expression_without_block_node*> ret_val) noexcept: statements(statements), ret_val(ret_val) {}
    [[nodiscard]] std::vector<statement_node*> get_statements() const noexcept;
    [[nodiscard]] std::optional<expression_without_block_node*> get_ret_val() const noexcept;
};

}

#endif // LASERC_TOKEN_HPP
