//
// Created by rylin on 2026/6/17.
//

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <print>
#include <utility>
#include <vector>

struct ParsedProgram {
    std::vector<char> instructions;
    std::vector<size_t> jump_table;
};

ParsedProgram format_code(std::ifstream &input_file) {
    std::vector<char> instructions;
    std::vector<size_t> jump_table;
    std::vector<size_t> bracket_stack;

    input_file.seekg(0, std::ios::end);
    const std::streamsize length = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    if (length <= 0) {
        return ParsedProgram{};
    }

    std::string buffer(length, '\0');
    if (!input_file.read(buffer.data(), length)) {
        buffer.resize(input_file.gcount());
    }

    instructions.reserve(buffer.size());
    jump_table.reserve(buffer.size());
    bracket_stack.reserve(buffer.size() / 2);

    for (char c: buffer) {
        if (c == '+' || c == '-' || c == '<' || c == '>' ||
            c == '.' || c == ',' || c == '[' || c == ']') {
            auto current_idx = instructions.size();
            instructions.push_back(c);
            jump_table.push_back(0);

            if (c == '[') {
                bracket_stack.push_back(current_idx);
            } else if (c == ']') {
                if (bracket_stack.empty()) {
                    std::cerr << "Unbalanced brackets" << std::endl;
                    std::exit(1);
                }
                size_t open_idx = bracket_stack.back();
                bracket_stack.pop_back();

                jump_table[open_idx] = current_idx;
                jump_table[current_idx] = open_idx;
            }
        }
    }

    if (!bracket_stack.empty()) {
        std::cerr << "Unbalanced brackets" << std::endl;
        std::exit(1);
    }

    instructions.shrink_to_fit();
    jump_table.shrink_to_fit();

    return ParsedProgram{std::move(instructions), std::move(jump_table)};
}

int main(const int argc, char **argv) {
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
                std::cerr << "Input file already specified: " << input_filepath << std::endl;
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

    const auto [instructions, jump_table] = format_code(input_file);

    auto start_time = std::chrono::steady_clock::now();

    for (size_t i = 0; i < instructions.size(); ++i) {
        switch (instructions[i]) {
            case '+':
                tape[tape_index]++;
                break;
            case '-':
                tape[tape_index]--;
                break;
            case '>':
                tape_index = (tape_index + 1) & 32767;
                break;
            case '<':
                tape_index = (tape_index - 1) & 32767;
                break;
            case '.':
                std::print("{}", static_cast<char>(tape[tape_index]));
                break;
            case ',': {
                std::fflush(stdout);
                int c = std::cin.get();
                tape[tape_index] = (c == EOF) ? 0 : static_cast<uint8_t>(c);
            }
            break;
            case '[':
                if (tape[tape_index] == 0) {
                    i = jump_table[i];
                }
                break;
            case ']':
                if (tape[tape_index] != 0) {
                    i = jump_table[i];
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
