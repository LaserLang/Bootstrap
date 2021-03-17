#ifndef CANNON_PROGRAM_HPP
#define CANNON_PROGRAM_HPP

#include <memory>
#include <string_view>
#include <vector>

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

class incomplete_function {
  private:
    const incomplete_type *m_return_type;
    std::string_view m_name;
  public:
    function to_function() const;
    void set_return_type(const incomplete_type &return_type);
    void set_name(const std::string_view name);
    const incomplete_type* return_type() const;
    const std::string_view name() const;
};

class incomplete_program {
  private:
    std::vector<const incomplete_function*> m_functions;
  public:
    program to_program() const;
    void add_function(const incomplete_function &function);
    std::vector<const incomplete_function*> functions() const;
};

}

#endif // CANNON_PROGRAM_HPP
