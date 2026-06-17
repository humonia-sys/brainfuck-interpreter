//
// Created by rylin on 2026/6/17.
//

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <utility>
#include <vector>

std::vector<char> format_code(std::string_view code_string) {
    auto is_instruction = [](const char c) {
        return c == '+' || c == '-' || c == '<' || c == '>' ||
               c == '.' || c == ',' || c == '[' || c == ']';
    };
    auto filtered = code_string | std::views::filter(is_instruction);
    return {filtered.begin(), filtered.end()};
}

int main(const int argc, char** argv) {
    bool enable_time_profiling = false;
    std::string input_file_name;

    auto start_time = std::chrono::steady_clock::now();

    for (int i = 1; i < argc; ++i) {
        if (auto arg = std::string_view(argv[i]); arg.starts_with('-')) {
            if (arg == "-t" || arg == "--time") {
                enable_time_profiling = true;
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
            if (!input_file_name.empty()) {
                std::cerr << "Input file already exists: " << input_file_name << std::endl;
                return 1;
            }
            input_file_name = arg;
        }
    }

    if (input_file_name.empty()) {
        std::cerr << "Usage: " << argv[0] << " [options] <input_file>" << std::endl;
        std::cerr << "Input file not provided" << std::endl;
        return 1;
    }

    std::ifstream input_file(input_file_name);
    if (!input_file.is_open()) {
        std::cerr << "Unable to open file: " << input_file_name << std::endl;
        return 1;
    }

    std::stringstream code_buffer;
    code_buffer << input_file.rdbuf();

    std::vector<uint8_t> tape(30000, 0);
    size_t tape_index = 0;

    const auto instructions = format_code(code_buffer.str());

    for (size_t i = 0; i < instructions.size(); ++i) {
        switch (instructions[i]) {
            case '+':
                tape[tape_index]++;
                break;
            case '-':
                tape[tape_index]--;
                break;
            case '>':
                tape_index = (tape_index + 1) % 30000;
                break;
            case '<':
                tape_index = (tape_index + 29999) % 30000;
                break;
            case '.':
                std::print("{}", static_cast<char>(tape[tape_index]));
                break;
            case ',':
                tape[tape_index] = static_cast<uint8_t>(std::cin.get());
                break;
            case '[':
                if (tape[tape_index] == 0) {
                    for (int depth = 1; depth > 0;) {
                        i++;
                        if (instructions[i] == '[') depth++;
                        else if (instructions[i] == ']') depth--;
                    }
                }
                break;
            case ']':
                if (tape[tape_index] != 0) {
                    for (int depth = 1; depth > 0;) {
                        i--;
                        if (instructions[i] == '[') depth--;
                        else if (instructions[i] == ']') depth++;
                    }
                }
                break;
            default:
                std::unreachable();
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
    if (enable_time_profiling) {
        std::println("use time: {:.3f} ms", elapsed.count());
    }
}
