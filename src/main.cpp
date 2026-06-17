//
// Created by rylin on 2026/6/17.
//

#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <print>
#include <sstream>

std::vector<char> format_code(const std::string_view code_string) {
    std::vector<char> result;
    result.reserve(code_string.size());
    for (auto c: code_string) {
        if (c == '+' || c == '-' || c == '<' || c == '>' || c == '.' || c == ',' || c == '[' || c == ']') {
            result.push_back(c);
        }
    }
    result.shrink_to_fit();

    return result;
}

int main(const int argc, char** argv) {
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


    const auto tape = std::make_unique<uint8_t[]>(30000);
    uint8_t *ptr = tape.get();

    const auto instructions = format_code(buffer.str());

    for (size_t i = 0; i < instructions.size(); ++i) {
        switch (instructions[i]) {
            case '+':
                *(ptr) += 1;
                break;
            case '-':
                *(ptr) -= 1;
                break;
            case '>':
                if (ptr == tape.get() + 29999) {
                    ptr = tape.get();
                } else {
                    ptr += 1;
                }
                break;
            case '<':
                if (ptr == tape.get()) {
                    ptr += 29999;
                } else {
                    ptr -= 1;
                }
                break;
            case '.':
                std::print("{}", static_cast<char>(*(ptr)));
                break;
            case ',':
                *(ptr) = static_cast<uint8_t>(std::cin.get());
                break;
            case '[':
                if (*(ptr) == 0) {
                    auto loop_depth = 1;
                    while (loop_depth > 0) {
                        i++;
                        if (instructions[i] == '[') loop_depth++;
                        else if (instructions[i] == ']') loop_depth--;
                    }
                }
                break;
            case ']':
                if (*(ptr) != 0) {
                    auto loop_depth = 1;
                    while (loop_depth > 0) {
                        i--;
                        if (instructions[i] == '[') loop_depth--;
                        else if (instructions[i] == ']') loop_depth++;
                    }
                }
                break;
            default:
                std::println("");
                std::println("Illegal char detected. i = {} : {}", i, instructions[i]);
                break;
        }
    }
}
