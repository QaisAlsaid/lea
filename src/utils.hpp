#pragma once

#include <cstddef>
#include <string>

namespace my_ns::utils
{
  using hash_type = size_t;
  inline constexpr hash_type fnv1a_hash(const char* str, size_t hash = 14695981039346656037ULL)
  {
    return (*str == 0) ? hash : fnv1a_hash(str + 1, (hash ^ static_cast<size_t>(*str)) * 1099511628211ULL);
  }

  inline constexpr hash_type fnv1a_hash(const std::string_view str, size_t hash = 14695981039346656037ULL)
  {
    return fnv1a_hash(str, hash);
  }

  inline constexpr hash_type fnv1a_hash(const std::string& str, size_t hash = 14695981039346656037ULL)
  {
    return fnv1a_hash(str.c_str(), hash);
  }


  inline constexpr hash_type hash_combine(hash_type lhs, hash_type rhs)
  {
    if constexpr (sizeof(size_t) >= 8) //use this on 64 bit systems
    {
      lhs ^= rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2);
    }
    //else // 32 bit systems
    //{
    //  lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    //}
    return lhs;
  }
}
