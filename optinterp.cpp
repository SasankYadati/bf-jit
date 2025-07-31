#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "parser.h"
#include "utils.h"

constexpr int MEMORY_SIZE = 30000;

std::vector<size_t> compute_jumptable(const Program& p) {
    size_t pc = 0;
    size_t program_size = p.instructions.size();
    std::vector<size_t> jumptable(program_size, 0);

    while (pc < program_size) {
        char instruction = p.instructions[pc];
        if (instruction == '[') {
            int bracket_nesting = 1;
            size_t seek = pc;

            while (bracket_nesting && ++seek < program_size) {
                if (p.instructions[seek] == ']') {
                    bracket_nesting--;
                } else if (p.instructions[seek] == '[') {
                    bracket_nesting++;
                }
            }

            if (!bracket_nesting) {
                jumptable[pc] = seek;
                jumptable[seek] = pc;
            } else {
                DIE << "unmatched '[' at pc=" << pc;
            }
        }
        pc++;
    }

    return jumptable;
}

void optinterp(const Program& p, bool verbose) {
    std::vector<uint8_t> memory(MEMORY_SIZE, 0);
    size_t pc = 0;
    size_t dataptr = 0;
    auto jumptable = compute_jumptable(p);
    while (pc < p.instructions.size()) {
        char instruction = p.instructions[pc];
        switch (instruction) {
            case '>':
                dataptr++;
                break;
            case '<':
                dataptr--;
                break;
            case '+':
                memory[dataptr]++;
                break;
            case '-':
                memory[dataptr]--;
                break;
            case '.':
                std::cout.put(memory[dataptr]);
                break;
            case ',':
                memory[dataptr] = std::cin.get();
                break;
            case '[':
                if (memory[dataptr] == 0) {
                    pc = jumptable[pc];
                }
                break;
            case ']':
                if (memory[dataptr] != 0) {
                    pc = jumptable[pc];
                }
                break;
            default:
                DIE << "bad char '" << instruction << "' at pc=" << pc;
        }
        pc++;
    }
}

int main(int argc, const char** argv) {
    bool verbose = false;
    std::string bf_file_path;
    parse_command_line(argc, argv, &bf_file_path, &verbose);

    Timer t1;
    std::ifstream file(bf_file_path);
    if (!file) {
        DIE << "unable to open file " << bf_file_path;
    }

    Program program = parse_from_stream(file);
    if (verbose) {
        std::cout << "Parsing took: " << t1.elapsed() << "s\n";
        std::cout << "Length of program: " << program.instructions.size() << "\n";
        std::cout << "Program:\n" << program.instructions << "\n";
    }

    if (verbose) {
        std::cout << "[>] Running optinterp:\n";
    }
    
    Timer t2;
    optinterp(program, verbose);
    
    if (verbose) {
        std::cout << "[<] Done (elapsed: " << t2.elapsed() << "s)\n";
    }

    return 0;
}