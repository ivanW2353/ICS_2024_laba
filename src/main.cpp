#include "CLI/CLI.hpp"
#include "assembler/assembler.hpp"
#include "assembler/instruction.hpp"
#include "assembler/parser.hpp"
#include "assembler/token.hpp"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

namespace {
struct ProgramOptions {
    std::string input_file;
    std::string output_file;
    bool print_tokens = false;
    bool print_instructions = false;
};

auto parse_program_options(int argc, char** argv) -> ProgramOptions {
    ProgramOptions options;

    CLI::App app { "LC-3 Assembler" };
    app.add_option("input_file", options.input_file, "Path to the input assembly file")->required();
    app.add_option("-o,--output", options.output_file, "Path to the output file");
    app.add_flag("-t,--tokens", options.print_tokens, "Print all parsed tokens and stop");
    app.add_flag(
        "-I,--instructions",
        options.print_instructions,
        "Print all parsed instructions and stop"
    );
    app.set_help_flag("-h, --help", "Print help information");

    try {
        app.parse(argc, argv);
    } catch (CLI::ParseError const& e) {
        std::exit(app.exit(e));
    }

    return options;
}

/// Reads the entire content of the input stream `input` and returns it as a string.
auto read_all(std::ifstream& input) -> std::string {
    std::string content;
    input.seekg(0, std::ios::end);
    content.reserve(input.tellg());
    input.seekg(0, std::ios::beg);
    content.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    return content;
}
}  // namespace

auto main(int argc, char** argv) -> int {
    ProgramOptions const options = parse_program_options(argc, argv);

    // Open the input file.
    std::ifstream input(options.input_file);
    if (!input) {
        std::cerr << "error: cannot open file '" << options.input_file << "'\n";
        return 1;
    }

    std::ofstream output_file;
    auto& out = [&]() -> std::ostream& {
        if (options.output_file.empty()) {
            return std::cout;
        } else {
            output_file.open(options.output_file);
            return output_file;
        }
    }();

    if (!out) {
        std::cerr << "error: cannot open file '"
                  << (options.output_file.empty() ? "stdout" : options.output_file) << "'\n";
        return 1;
    }

    // Read the source code into `source`.
    std::string const source = read_all(input);

    Parser parser(source);

    if (options.print_tokens) {
        while (parser.next_token().kind() != Token::End) {
            out << parser.current_token() << '\n';
        }

        out << parser.current_token() << '\n';
        return 0;
    }

    // Parse the source code into a sequence of instructions.
    std::vector<Instruction> const instructions = parser.parse_instructions();

    // There was an error during parsing, so we return an error code.
    if (instructions.size() == 1 && instructions.front().is_unknown()) {
        return 1;
    }

    if (options.print_instructions) {
        for (Instruction const& instruction : instructions) {
            out << instruction << '\n';
        }

        return 0;
    }

    Assembler assembler(std::move(instructions));
    // Emit the binary representation of the instructions.
    std::vector<std::uint16_t> const binary = assembler.run();

    // Print the binary representation of the instructions.
    if (binary.empty()) {
        return 1;
    }

    std::uint16_t const start_address = assembler.start_address();
    for (std::size_t i = 0; i != binary.size(); ++i) {
        out << '(' << std::hex << std::uppercase << start_address + i << ") "
            << std::bitset<16>(binary[i]) << '\n';
    }
}
