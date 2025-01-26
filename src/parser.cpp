#include "assembler/parser.hpp"

#include "assembler-c/instruction.h"
#include "assembler-c/parser.h"
#include "assembler/instruction.hpp"
#include "assembler/token.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef USE_CPP
    #include "assembler/solution.hpp"
#else
    #include "assembler-c/solution.h"
#endif

namespace {
/// Consumes non-newline whitespace characters (such as '\r' and ' ') starting from the position
/// pointed to by `current`, until reaching `end` or encountering a non-whitespace character.
/// Returns the position where it stops.
auto consume_spaces(char const* current, char const* end) -> char const* {
    return std::find_if_not(current, end, [](char ch) { return std::isspace(ch) && ch != '\n'; });
}

/// Parses a decimal integer starting from the position pointed to by `current` until reaching `end`
/// or encountering an invalid character. Returns the position where parsing ends (i.e., the first
/// invalid character).
///
/// We allow an optional leading sign.
auto lex_decimal_number(char const* current, char const* end) -> char const* {
    // This function will be implemented by the student.
    return parse_decimal_number(current, end);
}

/// Parses a string literal starting from the position pointed to by `current`, until reaching
/// `end`, the closing double quote of the string literal, or the end of the line. Returns the
/// position where parsing ends.
///
/// A string literal is always enclosed in double quotes. Note that `current` points to the first
/// character after the opening double quote, not the quote itself:
///
///     "string literal"
///      ^ current
///
/// If there is a closing double quote, this function will also consume it.
auto lex_string_literal(char const* current, char const* end) -> char const* {
    // This function will be implemented by the student.
    return parse_string_literal(current, end);
}

/// Parses an identifier starting from the position pointed to by `current`, until reaching `end`
/// or encountering the first character that cannot be part of an identifier. Returns the position
/// where parsing ends.
///
/// In LC-3, an identifier is defined as a string consisting of 1 to 20 alphanumeric characters,
/// where the first character must be a letter. Note that `current` points to the second character
/// of the identifier, not the first:
///
///     apple23
///      ^ current
auto lex_identifier(char const* current, char const* end) -> char const* {
    return std::find_if_not(current, end, static_cast<int (*)(int)>(std::isalnum));
}

/// Checks if `identifier` is a valid LC-3 opcode, such as `ADD`, `AND`, `BRp`, etc. In our
/// implementation, opcodes are case-sensitive, so `Add` is not a valid opcode.
///
/// You can find all opcodes in the textbook on page 656.
auto is_valid_opcode(std::string const& identifier) -> bool {
    static std::unordered_set<std::string> const opcodes = {
#define OPCODE(name) #name,
#include "assembler/opcode.def"
    };

    return opcodes.find(identifier) != opcodes.end();
}

/// Checks if `identifier` is a valid register name. In LC-3, valid register names are `R0`~`R7`.
auto is_valid_register(std::string const& identifier) -> bool {
    return identifier.size() == 2 && identifier[0] == 'R' && '0' <= identifier[1]
        && identifier[1] < '8';
}

/// Checks if `identifier` *can* be a valid immediate value.
///
/// Note that this function does not actually verify whether the immediate value is fully valid,
/// since it cannot cover all cases. For example, decimal immediate values cannot be checked here
/// because they are not valid identifiers (they start with the `#` character). This function is
/// only used to determine whether an identifier *may* be considered an immediate value so it can be
/// subjected to further checks later.
///
/// Since immediate values always have a prefix, and hexadecimal and binary immediate values can
/// satisfy the identifier format, we try to verify if their format is valid. We do this by checking
/// for invalid characters. For instance, for binary values, only '0' and '1' are allowed.
///
/// Our implementation uses `std::all_of`, meaning that even if the "immediate value" consists of
/// only a prefix, it will still be considered "valid" because `std::all_of` always returns `true`
/// for an empty range. For example, the identifier `x` would be treated as an immediate value. We
/// will further check such cases later.
auto may_be_valid_immediate_number(std::string const& identifier) -> bool {
    // Since we know that `identifier` contains at least one character, it is safe to call
    // `front()` on it.
    switch (identifier.front()) {
    case 'x':
        // When the prefix is 'x', we are trying to interpret `identifier` as a hexadecimal
        // immediate value. We need to ensure that all characters making up the integer are valid
        // hexadecimal digits, i.e., 0-9, a-f, A-F.
        return std::all_of(std::next(identifier.begin()), identifier.end(), [](char ch) {
            return ('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F');
        });

    case 'b':
        // When the prefix is 'b', we are trying to interpret `identifier` as a binary immediate
        // value. We need to ensure that all characters making up the integer are either '0' or '1'.
        return std::all_of(std::next(identifier.begin()), identifier.end(), [](char ch) {
            return ch == '0' || ch == '1';
        });

    default:
        return false;
    }
}

/// Determines whether an identifier is an opcode, register, immediate value, or label based on its
/// content. The identifier's text content is specified by the range [identifier_begin,
/// identifier_end).
auto identifier_kind(char const* identifier_begin, char const* identifier_end) -> Token::TokenKind {
    // Convert the token's content to a `std::string`, which makes it easier to handle the string
    // content (e.g., using it in `std::unordered_set`).
    //
    // A better approach would be to use `std::string_view`, which is a C++17 feature. However,
    // considering that students' environments may not have newer compilers, we didn't do this.
    std::string const identifier(identifier_begin, identifier_end);

    if (is_valid_opcode(identifier)) {
        return Token::Opcode;
    }

    if (is_valid_register(identifier)) {
        return Token::Register;
    }

    if (may_be_valid_immediate_number(identifier)) {
        return Token::Immediate;
    }

    return Token::Label;
}

/// Checks if the content of a token is a valid LC-3 pseudo-instruction opcode, such as `.ORIG`,
/// `.END`, etc. In our implementation, pseudo-instruction opcodes are case-sensitive, so `.End` is
/// not a valid opcode. `content_begin` and `content_end` specify the range of the token's content.
///
/// You can find all pseudo-instruction opcodes in the textbook on page 236.
auto is_valid_pseudo(char const* content_begin, char const* content_end) -> bool {
    static std::unordered_set<std::string> const pseudos = {
#define PSEUDO(name) "." #name,
#include "assembler/opcode.def"
    };

    return pseudos.find(std::string(content_begin, content_end)) != pseudos.end();
}

/// Consumes all characters starting from the position pointed to by `current` until reaching a
/// newline character or reaching `end`. This allows us to skip all content from `current` onward as
/// a comment.
auto consume_comment(char const* current, char const* end) -> char const* {
    return std::find(current, end, '\n');
}
}  // namespace

auto Parser::next_token() -> Token const& {
Restart:
    // Save the current value of `current_`. It will be used as the start of the token.
    char const* const token_begin = current_;
    // The type of the current token. We will modify this based on the current character below.
    Token::TokenKind kind = Token::Unknown;

    if (current_ == source_end_) {
        // If we have reached the end of the source code, generate a `Token::End`.
        kind = Token::End;
    } else {
        // Otherwise, we can safely dereference `current_` and make further judgments based on the
        // current character.
        switch (*current_++) {
        case '\n':
            // We reach the end of the current line, so we consume it and generate a `Token::EOL`.
            kind = Token::EOL;
            break;

            // clang-format off
        case '\r': case ' ': case '\t': case '\f': case '\v':
            // clang-format on
            // Skip all non-newline whitespace characters, such as '\r' and ' '. We need to preserve
            // '\n' to correctly generate `Token::EOL`.
            current_ = consume_spaces(current_, source_end_);
            // Restart the process to parse the token from the updated position.
            goto Restart;

        case ',':
            // If the current character is a comma, return a `Token::Comma` and move `current_` to
            // the next character.
            kind = Token::Comma;
            break;

        case '#':
            // The '#' indicates the start of a decimal immediate value, so we parse the remaining
            // part as a decimal integer.
            //
            // We do not handle `x` or `b` prefixed tokens here because tokens starting with `x` or
            // `b` are not always immediate values. For example, `x3000` is an immediate value,
            // while `xabc` is a valid label. So we choose to parse them as identifiers and further
            // determine whether they are valid immediate values.
            current_ = lex_decimal_number(current_, source_end_);
            kind = Token::Immediate;
            break;

        case '"':
            // Parse string literals.
            current_ = lex_string_literal(current_, source_end_);
            kind = Token::String;
            break;

            // clang-format off
        case '0': case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': case '+': case '-':
            // clang-format on
            // We encountered a number or a sign, parse it as a decimal integer.
            current_ = lex_decimal_number(--current_, source_end_);
            kind = Token::Number;
            break;

            // clang-format off
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I':
        case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
        case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':
        case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
        case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            // clang-format on
            // The current token starts with a letter, parse it as an identifier.
            current_ = lex_identifier(current_, source_end_);
            // We need to determine whether the identifier is an opcode, register, immediate value,
            // or label. Note that it cannot be a pseudo-instruction because it doesn't satisfy the
            // format of a pseudo-instruction.
            kind = identifier_kind(token_begin, current_);
            break;

        case '.':
            // The current token starts with a '.', and it is always parsed as a pseudo-instruction
            // because it cannot be interpreted as any other type of token, even though it might be
            // an invalid pseudo-instruction.
            current_ = lex_identifier(current_, source_end_);
            // Check if the content of the token is a valid pseudo-instruction opcode.
            if (is_valid_pseudo(token_begin, current_)) {
                kind = Token::Pseudo;
            } else {
                kind = Token::Unknown;
            }
            break;

        case ';':
            // A semicolon marks the beginning of a line comment, so skip the remaining characters
            // in the line.
            current_ = consume_comment(current_, source_end_);
            // Restart the process to parse the token from the updated position.
            goto Restart;

        default:
            // For all other characters, keep `kind` as `Unknown`, generating an unknown token.
            break;
        }
    }

    // Construct the token and save it in `cur_token_`.
    cur_token_ = { kind, token_begin, current_ };
    return cur_token_;
}

auto Parser::emit_diagnostic_at_current_token() const -> std::ostream& {
    // TODO: We need to enhance this function to report the line and column position of the token,
    // as this will help students debug more effectively.
    return std::cout << "error: at token `" << current_token().display_content() << "`: ";
}

void Parser::emit_opcode_diag_at_current_token() const {
    emit_diagnostic_at_current_token()
        << "expected token kind `" << Token::Opcode << "` or `" << Token::Pseudo << "`, but got `"
        << current_token().kind() << "`\n";
}

void Parser::emit_operand_diag_at_current_token(OperandConstructionErrorType error) const {
    std::ostream& out = emit_diagnostic_at_current_token()
        << "error when constructing an operand: ";

    switch (error) {
    case OperandConstructionErrorType::InvalidTokenKind:
        out << "cannot construct an operand from token kind `" << current_token().kind() << '`';
        break;
    case OperandConstructionErrorType::InvalidNumber:
        out << "invalid number `" << current_token().display_content() << '`';
        break;
    case OperandConstructionErrorType::IntegerOverflow:
        out << "integer value overflow `" << current_token().display_content()
            << "` for a 16-bit integer";
        break;
    case OperandConstructionErrorType::MissingQuote:
        out << "missing closing quote in string literal `" << current_token().display_content()
            << '`';
        break;
    default:
        out << "ICE: unknown error type `" << error << '`';
        break;
    }

    out << '\n';
}

auto Parser::parse_operand_list(Instruction instr) -> Instruction {
    // The operand list is a sequence of tokens separated by `Token::Comma`.

    // We need to check whether the current token can be the start of the operand list, as LC-3
    // syntax has no special marker to indicate the start of the operand list, and some instructions
    // have no operands.
    OperandConstructionErrorType const first_operand_construction_result =
        instr.add_operand(current_token());

    switch (first_operand_construction_result) {
    case OperandConstructionErrorType::InvalidTokenKind:
        // This means the current token is not suitable to be an operand, so we should not start
        // parsing the operand list, but rather construct an instruction with no operands.
        return instr;

    case OperandConstructionErrorType::NoError:
        // The current token is a valid operand, so we continue parsing the other operands.
        break;

    default:
        // The current token could be an operand but is not valid. Report an error and stop parsing.
        emit_operand_diag_at_current_token(first_operand_construction_result);

        // Return an unknown instruction, indicating an error occurred during parsing.
        return {};
    }

    // Now we parse the remaining part as a comma-separated operand list until encountering a token
    // that cannot form part of the operand list.
    while (next_token().kind() == Token::Comma) {
        // Try to construct an operand from the next token.
        OperandConstructionErrorType const construction_result = instr.add_operand(next_token());

        if (construction_result != OperandConstructionErrorType::NoError) {
            // An error occurred during construction, report a diagnostic message and stop parsing.
            emit_operand_diag_at_current_token(construction_result);

            // Return an unknown instruction, indicating an error occurred during parsing.
            return {};
        }
    }

    // Operands parsing is complete. We do not need to handle the `EOL` token at the end of the
    // line, as it will be processed during the next iteration.
    return instr;
}

auto Parser::parse_instructions() -> std::vector<Instruction> {
    std::vector<Instruction> instructions;

    // Call `next_token()` to generate the first token and save it to `cur_token_`.
    next_token();

    // Parse and construct `Instruction` objects one by one.
    while (true) {
        switch (current_token().kind()) {
        case Token::EOL:
            // Skip the `EOL` token and start the next iteration.
            next_token();
            break;

        case Token::End:
            // We reach the end of the code, so stop parsing.
            return instructions;

        default:
            // Try to parse an instruction starting from the current token.
            Instruction instr = parse_instruction();

            // If an unknown instruction is returned, it indicates an error was encountered during
            // parsing. We return a result containing only one unknown instruction to indicate
            // parsing failure.
            if (instr.is_unknown()) {
                return { {} };
            }

            // Add the current instruction to the result.
            instructions.push_back(std::move(instr));

            // Check if the current instruction is the `.END` pseudo-instruction. If so, stop
            // parsing.
            if (instr.get_opcode() == Instruction::END) {
                return instructions;
            }
        }
    }
}

//==================================================================================================
// C bindings for the `Parser` class.
//==================================================================================================

namespace {
auto unwrap(ParserRef parser) -> Parser& {
    return *reinterpret_cast<Parser*>(parser);
}

auto wrap(Token const& token) -> TokenRef {
    return reinterpret_cast<TokenRef>(&token);
}

auto unwrap(InstructionRef instr) -> Instruction& {
    return *reinterpret_cast<Instruction*>(instr);
}
}  // namespace

auto parser_get_current_token(ParserRef parser) -> TokenRef {
    return wrap(unwrap(parser).current_token());
}

auto parser_move_to_next_token(ParserRef parser) -> TokenRef {
    return wrap(unwrap(parser).next_token());
}

void parser_parse_operand_list(ParserRef parser, InstructionRef instr) {
    Instruction result = unwrap(parser).parse_operand_list(unwrap(instr));
    unwrap(instr) = std::move(result);
}

void parser_emit_opcode_diag_at_current_token(ParserRef parser) {
    unwrap(parser).emit_opcode_diag_at_current_token();
}

#ifndef USE_CPP
namespace {
auto wrap(Parser& parser) -> ParserRef {
    return reinterpret_cast<ParserRef>(&parser);
}

auto wrap(Instruction& instr) -> InstructionRef {
    return reinterpret_cast<InstructionRef>(&instr);
}
}  // namespace

auto Parser::parse_instruction() -> Instruction {
    Instruction result_instr;
    // Parse the instruction. This function will be implemented by the student.
    ::parse_instruction(wrap(*this), wrap(result_instr));
    return result_instr;
}
#endif
