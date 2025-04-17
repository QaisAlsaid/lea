#pragma once

#include <expected>
#include <filesystem>
#include <vector>
namespace my_ns
{
  struct runner_error 
  {
    enum class type
    {
      cant_open_file, parse_error
    };
    std::vector<std::pair<type, std::string>> errors;
  };

  std::expected<void, runner_error> start_runner(const std::filesystem::path& file);
}
