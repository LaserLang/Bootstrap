#ifndef CANNON_PROGRAM_HPP
#define CANNON_PROGRAM_HPP

#include <memory>
#include <string_view>
#include <vector>

#include "ast.hpp"

namespace cannon {

enum class type_id {
    None, // Not resolved yet
    Void,
    I32
};

class type {
  private:
    const type_id m_id;
  public:
    type(type_id id);
    type_id id() const;
};

class statement {
  public:
    virtual ~statement() = 0;
    virtual type return_type() const;
};

class expression : public statement {
  public:
    virtual ~expression() = 0;
};

class binary_expression : public expression {
  private:
    const expression &m_lhs;
    const binary_operator m_op;
    const expression &m_rhs;
    const type m_return_type;
  public:
    binary_expression(const expression &lhs, binary_operator op, const expression &rhs, type return_type);
    type return_type() const;
};

class function {
  private:
    const type m_return_type;
    const std::string_view m_name;
  public:
    function(type return_type, std::string_view name);
    friend std::ostream &operator<<(std::ostream &os, const function &function);
    type return_type() const;
    std::string_view name() const;
};

class program {
  private:
    const std::vector<std::unique_ptr<function>> m_functions;
  public:
    program(std::vector<std::unique_ptr<function>> functions);
    friend std::ostream &operator<<(std::ostream &os, const program &program);
    const std::vector<std::unique_ptr<function>>& functions() const;
};

class incomplete_type {
  private:
    std::string_view m_name;
    type_id m_id;
  public:
    type to_type() const;
    void set_name(const std::string_view name);
    void set_id(const type_id id);
    std::string_view name() const;
    type_id id() const;
};

class incomplete_statement {
  public:
    virtual ~incomplete_statement() = 0;
};

class incomplete_expression : public incomplete_statement {
  public:
    virtual ~incomplete_expression() = 0;
};

class incomplete_binary_expression : public incomplete_expression {
  private:
    incomplete_expression *lhs;
    binary_operator op;
    incomplete_expression *rhs;
  public:
    binary_expression to_binary_expression() const;
};

class incomplete_function {
  private:
    const incomplete_type *m_return_type;
    std::string_view m_name;
    std::vector<std::unique_ptr<incomplete_statement>> m_statements;
    const fn_node *m_ast;
  public:
    function to_function() const;
    void set_return_type(const incomplete_type &return_type);
    void set_name(const std::string_view name);
    void set_ast(const fn_node &ast);
    void add_statement(std::unique_ptr<incomplete_statement> statement);
    const incomplete_type* return_type() const;
    const std::string_view name() const;
    const fn_node& ast() const;
    const std::vector<std::unique_ptr<incomplete_statement>>& statements() const;
};

class incomplete_program {
  private:
    std::vector<std::unique_ptr<incomplete_function>> m_functions;
  public:
    program to_program() const;
    void add_function(std::unique_ptr<incomplete_function> function);
    const std::vector<std::unique_ptr<incomplete_function>>& functions() const;
};

}

#endif // CANNON_PROGRAM_HPP
