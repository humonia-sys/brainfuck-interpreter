//
// Created by rylin on 2026/6/17.
//

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <utility>
#include <vector>

struct Instruction {
  char type;
  int repeat_count;
  size_t jump_target;
};

std::vector<Instruction> ParseProgram(std::ifstream& input_file) {
  std::vector<size_t> bracket_stack;
  std::vector<Instruction> program;

  input_file.seekg(0, std::ios::end);
  const std::streamsize length = input_file.tellg();
  input_file.seekg(0, std::ios::beg);

  if (length <= 0) {
    return program;
  }

  std::string buffer(length, '\0');
  if (!input_file.read(buffer.data(), length)) {
    buffer.resize(input_file.gcount());
  }

  program.reserve(buffer.size());
  bracket_stack.reserve(buffer.size() / 2);
  for (size_t i = 0; i < buffer.size(); ++i) {
    char c = buffer[i];
    if (c == '+' || c == '-' || c == '<' || c == '>' || c == '.' || c == ',' ||
        c == '[' || c == ']') {
      Instruction inst;
      inst.type = c;
      inst.repeat_count = 1;
      if (c == '+' || c == '-' || c == '<' || c == '>') {
        while (i + inst.repeat_count < buffer.size() &&
               buffer[i + inst.repeat_count] == c) {
          inst.repeat_count++;
        }
        i += inst.repeat_count - 1;
      } else if (c == '[') {
        bracket_stack.push_back(program.size());
      } else if (c == ']') {
        if (bracket_stack.empty()) {
          std::cerr << "Unbalanced brackets" << std::endl;
          std::exit(1);
        }
        std::size_t idx = bracket_stack.back();
        inst.jump_target = idx;
        program[idx].jump_target = program.size();
        bracket_stack.pop_back();
      }
      program.push_back(inst);
    }
  }

  if (!bracket_stack.empty()) {
    std::cerr << "Unbalanced brackets" << std::endl;
    std::exit(1);
  }

  program.shrink_to_fit();

  return program;
}

int main(const int argc, char** argv) {
  bool time_profiling = false;
  bool stop_parsing_options = false;
  std::string input_filepath;

  for (int i = 1; i < argc; ++i) {
    auto arg = std::string_view(argv[i]);
    if (arg == "--") {
      stop_parsing_options = true;
    } else if (arg.starts_with('-') && !stop_parsing_options) {
      if (arg == "-t" || arg == "--time") {
        time_profiling = true;
      } else if (arg == "-h" || arg == "--help") {
        std::println("Usage: {} [options] <input_file>", argv[0]);
        std::println("Options:");
        std::println("  -t, --time    Enable execution time profiling");
        std::println("  -h, --help    Display this help message");
        return 0;
      } else {
        std::cerr << "Unknown option: " << arg << std::endl;
        return 1;
      }
    } else {
      if (!input_filepath.empty()) {
        std::cerr << "Input file already specified: " << input_filepath
                  << std::endl;
        return 1;
      }
      input_filepath = arg;
    }
  }

  if (input_filepath.empty()) {
    std::cerr << "Usage: " << argv[0] << " [options] <input_file>" << std::endl;
    std::cerr << "Input file not provided" << std::endl;
    return 1;
  }

  std::ifstream input_file(input_filepath);
  if (!input_file.is_open()) {
    std::cerr << "Unable to open file: " << input_filepath << std::endl;
    return 1;
  }

  std::vector<uint8_t> tape(32768, 0);
  size_t tape_index = 0;

  const auto program = ParseProgram(input_file);

  auto start_time = std::chrono::steady_clock::now();

  for (size_t i = 0; i < program.size(); ++i) {
    const auto& inst = program[i];
    switch (inst.type) {
      case '+':
        tape[tape_index] += inst.repeat_count;
        break;
      case '-':
        tape[tape_index] -= inst.repeat_count;
        break;
      case '>':
        tape_index = (tape_index + inst.repeat_count) & 32767;
        break;
      case '<':
        tape_index = (tape_index - inst.repeat_count) & 32767;
        break;
      case '.':
        std::print("{}", static_cast<char>(tape[tape_index]));
        break;
      case ',': {
        std::fflush(stdout);
        int c = std::cin.get();
        tape[tape_index] = (c == EOF) ? 0 : static_cast<uint8_t>(c);
      } break;
      case '[':
        if (tape[tape_index] == 0) {
          i = inst.jump_target;
        }
        break;
      case ']':
        if (tape[tape_index] != 0) {
          i = inst.jump_target;
        }
        break;
      default:
        std::unreachable();
    }
  }

  auto end_time = std::chrono::steady_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
  if (time_profiling) {
    std::println("elapsed time: {:.3f} ms", elapsed.count());
  }
}
