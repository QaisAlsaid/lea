#pragma once

#include "ast.hpp"
#include "utils.hpp"
#include <expected>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

namespace my_ns 
{
  enum class object_type 
  {
    null = 0, integer, string, array, map, boolean, ret_value, fun, builtin,
    error, void_obj
  };

  struct hash_t
  {
    object_type type;
    utils::hash_type value;
    
    bool operator == (const hash_t& other) const
    {
      return type == other.type && value == other.value; 
    }
  };
}

namespace std 
{
  template <>
  struct hash<my_ns::hash_t>
  {
    size_t operator()(const my_ns::hash_t& h) const
    {
      return my_ns::utils::hash_combine(static_cast<my_ns::utils::hash_type>(h.type), h.value);
    }
  };
}


namespace my_ns
{ 
  class hashable
  {
  public:
    virtual hash_t hash() = 0;
  };

  class object 
  {
  public:
    virtual object_type get_type() = 0;
    virtual std::string inspect() = 0;
  };
 
  class void_object : public object 
  {
  public:
    object_type get_type() override { return object_type::void_obj; }
    std::string inspect() override { return "void"; }
  };

  class null : public object
  {
  public:
    object_type get_type() override
    {
      return object_type::null;
    }

    std::string inspect() override
    {
      return "null";
    }
  };

  class integer : public object, public hashable
  {
  public:
    integer(int64_t val)
      : m_value(val)
    {
    }

    object_type get_type() override
    {
      return object_type::integer;
    }

    std::string inspect() override
    {
      return std::to_string(m_value);
    }

    inline int64_t get_value() const
    {
      return m_value;
    }

    hash_t hash() override
    {
      return { .type = object_type::integer, .value = static_cast<utils::hash_type>(m_value) };
    }
  private:
    int64_t m_value;
  };

  class string : public object, public hashable
  {
  public:
    string(const std::string& val)
      : m_value(val)
    {
    }

    object_type get_type() override
    {
      return object_type::string;
    }

    std::string inspect() override
    {
      return m_value;
    }

    inline const std::string& get_value() const
    {
      return m_value;
    }

    hash_t hash() override
    {
      return { .type = object_type::integer, .value = utils::fnv1a_hash(m_value) };
    }
  private:
    std::string m_value;
  };

  class array : public object
  {
  public:
    array(const std::vector<std::shared_ptr<object>>& elems)
      : m_elements(elems)
    {
    }

    object_type get_type() override
    {
      return object_type::array;
    }

    std::string inspect() override
    {
      std::stringstream ss;

      ss << "[";

      for(const auto& elem : m_elements)
        ss << elem->inspect() << ", " ;
      ss << "]";
      
      return ss.str();
    }

    inline const std::vector<std::shared_ptr<object>>& get_elements() const
    {
      return m_elements;
    }
  private:
    std::vector<std::shared_ptr<object>> m_elements;
  };

  class map : public object
  {
  public:
    struct hash_pair 
    {
      std::shared_ptr<object> key, value;
    };
  public:
    map(const std::unordered_map<hash_t, hash_pair>& m)
      : m_map(m)
    {
    }

    object_type get_type() override
    {
      return object_type::map;
    }

    std::string inspect() override
    {
      std::stringstream ss;

      ss << "[";
      for(const auto& elem : m_map)
        ss << elem.second.key->inspect() << ": " << elem.second.value->inspect() << ", ";
      ss << "]";
      
      return ss.str();
    }

    inline const std::unordered_map<hash_t, hash_pair>& get_map() const
    {
      return m_map;
    }
  private:
    std::unordered_map<hash_t, hash_pair> m_map;
  };

  class boolean : public object, public hashable
  {
  public:
    boolean(bool val)
      : m_value(val)
    {
    }

    object_type get_type() override
    {
      return object_type::boolean;
    }

    std::string inspect() override
    {
      return m_value ? "true" : "false";
    }

    inline bool get_value() const
    {
      return m_value;
    }

    hash_t hash() override
    {
      return { .type = object_type::integer, .value = static_cast<utils::hash_type>(m_value) };
    }
  private:
    bool m_value;
  };

  class ret_value : public object 
  {
  public:
    ret_value(const std::shared_ptr<object>& val)
      : m_value(val)
    {
    }

    object_type get_type() override
    {
      return object_type::ret_value;
    }

    std::string inspect() override
    {
      return m_value->inspect();
    }

    inline std::shared_ptr<object> get_value() const
    {
      return m_value;
    }
  private:
    std::shared_ptr<object> m_value;
  };
 
  class error : public object 
  {
  public:
    error(const std::string& message)
      : m_message(message)
    {
    }

    object_type get_type() override
    {
      return object_type::error;
    }

    std::string inspect() override
    {
      return "error: " + m_message;;
    }
  private:
    std::string m_message;
  };

  class environment
  {
  public:
    enum class error 
    {
      not_found,
    };
  public:
    environment() = default;
    environment(const std::initializer_list<std::pair<const std::string, std::shared_ptr<object>>>& inl)
      : m_map(inl)
    {
    }

    environment(const std::shared_ptr<environment>& outer)
      : m_outer(outer)
    {
    }

    std::expected<std::shared_ptr<object>, error> get(const std::string& ident) const
    {
      auto it = m_map.find(ident);
      if(it == m_map.end())
      {
        if(m_outer == nullptr)
        {
          return std::unexpected(error::not_found);
        }
        else
        {
          return m_outer->get(ident);
        }
      }
      return it->second;
    }

    //TODO: non replacing set
    void set(const std::string& ident, const std::shared_ptr<object>& obj)
    {
      m_map[ident] = obj;
    }
  private:
    std::unordered_map<std::string, std::shared_ptr<object>> m_map;
    std::shared_ptr<environment> m_outer = nullptr;
  };

  class fun : public object 
  {
  public:
    fun(const std::vector<std::shared_ptr<identifire>>& params, std::shared_ptr<block> body, const std::shared_ptr<environment>& env)
      : parameters(params), body(body), env(env)
    {
    }

    object_type get_type() override
    {
      return object_type::fun;
    }

    std::string inspect() override
    {
      std::stringstream ss;
      
      ss << "fun" << "(";
      for(auto p : parameters)
        ss << p << ", ";

      ss << ")\n{\n";
      ss << body->to_string();
      ss << "\n}";

      return ss.str();
    }
  public:
    std::vector<std::shared_ptr<identifire>> parameters;
    std::shared_ptr<block> body;
    std::shared_ptr<environment> env;
  };

  class builtin : public object
  {
  public:
    using fun_type = std::function<std::shared_ptr<object>(const std::vector<std::shared_ptr<object>>&)>;
  public:
    builtin() = default;
    builtin(const fun_type& fn)
      : _fun(fn)
    {
    }

    object_type get_type() override
    {
      return object_type::builtin;
    }

    std::string inspect() override
    {
      return "builtin function";
    }
  public:
    fun_type _fun;
  };
}

