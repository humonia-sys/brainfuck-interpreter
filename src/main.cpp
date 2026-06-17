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
    auto is_instruction = [](char c) {
        return c == '+' || c == '-' || c == '<' || c == '>' ||
               c == '.' || c == ',' || c == '[' || c == ']';
    };
    auto filtered = code_string | std::views::filter(is_instruction);
    return {filtered.begin(), filtered.end()};
}

int main(const int argc, char** argv) {
    auto start_time = std::chrono::steady_clock::now();
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        std::cerr << "Unable to open file: " << argv[1] << std::endl;
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
    std::println("use time: {:.3f} ms", elapsed.count());
}
