#ifndef CANNON_PROGRAM_HPP
#define CANNON_PROGRAM_HPP

#include <memory>
#include <string>
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
    type_id m_id;
  public:
    type(type_id id);
    friend std::ostream &operator<<(std::ostream &os, const type &type);
    type_id id() const;
};

class statement {
  public:
    virtual ~statement() = 0;
    virtual type return_type() const = 0;
    virtual void do_print(std::ostream &os, std::string leftpad) = 0;
};

class expression : public statement {
  public:
    virtual ~expression() = 0;
};

class binary_expression : public expression {
  private:
    std::unique_ptr<expression> m_lhs;
    binary_operator m_op;
    std::unique_ptr<expression> m_rhs;
    type m_return_type;
  public:
    binary_expression(const std::unique_ptr<expression> lhs, binary_operator op, const std::unique_ptr<expression> rhs, type return_type);
    const expression& lhs() const;
    const expression& rhs() const;
    binary_operator op() const;
    type return_type() const;
    void do_print(std::ostream &os, std::string leftpad);
};

class integer_expression : public expression {
  private:
    int m_value;
  public:
    integer_expression(int value);
    int value() const;
    type return_type() const;
    void do_print(std::ostream &os, std::string leftpad);
};

class function {
  private:
    type m_return_type;
    std::string m_name;
    std::vector<std::unique_ptr<statement>> m_statements;
  public:
    function(type return_type, std::string name, std::vector<std::unique_ptr<statement>> statements);
    friend std::ostream &operator<<(std::ostream &os, const function &function);
    type return_type() const;
    const std::string& name() const;
    const std::vector<std::unique_ptr<statement>>& statements() const;
};

class program {
  private:
    std::vector<std::unique_ptr<function>> m_functions;
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
    virtual std::unique_ptr<statement> to_statement_ptr() const = 0;
};

class incomplete_expression : public incomplete_statement {
  public:
    virtual ~incomplete_expression() = 0;
    std::unique_ptr<statement> to_statement_ptr() const;
    virtual std::unique_ptr<expression> to_expression_ptr() const = 0;
};

class incomplete_integer_expression : public incomplete_expression {
  private:
    int m_value;
  public:
    std::unique_ptr<expression> to_expression_ptr() const;
    integer_expression to_integer_expression() const;
    void set_value(int value);
};

class incomplete_binary_expression : public incomplete_expression {
  private:
    std::unique_ptr<incomplete_expression> m_lhs;
    binary_operator m_op;
    std::unique_ptr<incomplete_expression> m_rhs;
    incomplete_type m_return_type;
  public:
    std::unique_ptr<expression> to_expression_ptr() const;
    binary_expression to_binary_expression() const;
    void set_lhs(std::unique_ptr<incomplete_expression> lhs);
    void set_op(binary_operator op);
    void set_rhs(std::unique_ptr<incomplete_expression> rhs);
    void set_return_type(incomplete_type return_type);
};

class incomplete_function {
  private:
    const incomplete_type *m_return_type;
    std::string_view m_name;
    std::vector<std::unique_ptr<incomplete_statement>> m_statements;
    const fn_node *m_ast;
  public:
    std::unique_ptr<function> to_function_ptr() const;
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
