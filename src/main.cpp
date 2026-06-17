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

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::vector<uint8_t> tape(30000, 0);
    size_t ptr = 0;

    const auto instructions = format_code(buffer.str());

    for (size_t i = 0; i < instructions.size(); ++i) {
        switch (instructions[i]) {
            case '+':
                tape[ptr]++;
                break;
            case '-':
                tape[ptr]--;
                break;
            case '>':
                ptr = (ptr + 1) % 30000;
                break;
            case '<':
                ptr = (ptr + 29999) % 30000;
                break;
            case '.':
                std::print("{}", static_cast<char>(tape[ptr]));
                break;
            case ',':
                tape[ptr] = static_cast<uint8_t>(std::cin.get());
                break;
            case '[':
                if (tape[ptr] == 0) {
                    for (int depth = 1; depth > 0;) {
                        i++;
                        if (instructions[i] == '[') depth++;
                        else if (instructions[i] == ']') depth--;
                    }
                }
                break;
            case ']':
                if (tape[ptr] != 0) {
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
