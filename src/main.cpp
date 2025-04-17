#include "repl.hpp"
#include "runner.hpp"

#include <filesystem>
#include <iostream>


int main(int argc, char** argv)
{
  if(argc == 1)
  {
    std::cout << "this is lea language \n";
    my_ns::start_repl();
  }
  else if(argc == 2)
  {
    auto f = std::filesystem::path(argv[1]);
    auto ret = my_ns::start_runner(f);
    if(!ret.has_value())
    {
      const my_ns::runner_error& errs = ret.error();
      if(errs.errors.empty())
        std::cerr << "unknown error\n";
      else 
      {
        for(const auto& err : errs.errors)
        {
          std::string prefix;
          switch(err.first)
          {
            case my_ns::runner_error::type::parse_error:
              prefix = "parse error: ";
              break;
            case my_ns::runner_error::type::cant_open_file:
              prefix = "cant open file error: ";
              break;
          }
          std::cerr << prefix << "message: " << err.second << "\n";
        }
      }
    }
  }
}
