# Lea

Lea is a small C++23 interpreter for a simple programming language, built following *Writing an Interpreter in Go* by Thorsten Ball. It’s got a few tweaks, named Lea for kicks. Not polished, just a C++ experiment.

## Setup

Clone and build:

```bash
git clone https://github.com/QaisAlsaid/lea.git
cd lea
mkdir build && cd build
cmake ..
make
```

Run:

```bash
./lea
```

## Example

Lea’s syntax is simple. Try this:

```lea
var x = 5;
var y = 10;
puts(to_string(x + y)); // Outputs: 15
```

Save as `test.lea` and run:

```bash
./lea test.lea
```

Or use the REPL:

```bash
./lea
>> puts("Hello, Lea!");
Hello, Lea!
```

## Heads-Up

Uses `shared_ptr` in the evaluator, so deep recursion (\~9,000 calls) can crash the stack. No plans to fix it for now.

## License
Lea's [MIT licensed](LICENSE)
