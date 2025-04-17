#include "runner.hpp"
#include "evaluator.hpp"
#include "lexer.hpp"
#include "object.hpp"
#include "parser.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace my_ns 
{
  std::expected<void, runner_error> start_runner(const std::filesystem::path& file)
  {
    runner_error err;
    if(!std::filesystem::exists(file))
    {
      auto fstr = file.string();
      err.errors.emplace_back(runner_error::type::cant_open_file, "can't open file: " + fstr);
      return std::unexpected(err);
    }

    std::ifstream fstream(file);
    std::stringstream ss;
    ss << fstream.rdbuf();

    auto env = std::make_shared<environment>();
    lexer lx(ss.str());
    parser ps(&lx);
    auto prog = ps.parse_program();

    const auto& errors = ps.get_errors();
    if(!errors.empty())
    {
      for(const auto& msg : errors)
        err.errors.emplace_back(runner_error::type::parse_error, msg);
      return std::unexpected(err);
    }

    auto evaluated = eval(prog, env);
    return {};
  }
}
