#include "assembler/instruction.hpp"

#include "assembler-c/instruction.h"
#include "assembler-c/operand.h"
#include "assembler-c/token.h"
#include "assembler/operand.hpp"
#include "assembler/token.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef USE_CPP
    #include "assembler/solution.hpp"
#else
    #include "assembler-c/solution.h"
#endif

namespace {
/// Checks whether `token_content` is a valid immediate number or integer.
///
/// The `token_content` is guaranteed to meet the following conditions:
///
///     + If `token_content` represents an immediate value, it has a prefix of `#`, `x`, or `b`. For
///       regular integers, it does not have a prefix.
///
///     + The length of `token_content` is at least 1.
///
///     + All characters in `token_content` are valid. For example, a decimal integer will only
///       contain numeric characters.
///
///     + `token_content` may contain at most one sign (+/-), and this sign must be at the beginning
///       of `token_content` (for a regular number) or immediately following the prefix `#` (for an
///       immediate value). For example, `token_content` could be `+` or `#-`, but not `++` or
///       `#2+`.
///
/// This function further checks if `token_content` is a valid immediate number or integer. For
/// instance, `#` by itself satisfies the above four conditions, but it is not a valid immediate
/// number.
///
/// Note that binary and hexadecimal immediate numbers cannot have a sign.
auto is_valid_number(std::string const& token_content) -> bool {
    switch (token_content.front()) {
    case '#':
    case 'x':
    case 'b':
        if (token_content.size() == 1) {
            // The immediate consists of only a prefix, so it is invalid.
            return false;
        }

        if (token_content.size() == 2) {
            // If the length of the immediate is 2 (i.e., there is only one character after the
            // prefix), that character cannot be a sign.
            return token_content[1] != '+' && token_content[1] != '-';
        }

        // In all other cases, `token_content` is a valid immediate.
        return true;

    case '+':
    case '-':
        // For regular integers, if it starts with a sign, it must contain at least one digit.
        return token_content.size() > 1;

    default:
        // In all other cases, `token_content` is a valid integer, as it definitely contains at
        // least one digit.
        return true;
    }
}

/// Converts the immediate or regular decimal integer represented by `content` into an integer of
/// type `std::int16_t`. If the conversion result exceeds the range of `std::int16_t`, sets `ok` to
/// `false`.
auto to_integer(std::string const& content, bool* ok) -> std::int16_t {
    // This function will be implemented by the student.
#ifdef USE_CPP
    return string_to_integer(content, ok);
#else
    int _ok = 0;
    std::int16_t const result = string_to_integer(content.c_str(), &_ok);
    *ok = static_cast<bool>(_ok);
    return result;
#endif
}
}  // namespace

auto operator<<(std::ostream& out, OperandConstructionErrorType error) -> std::ostream& {
    out << "OperandConstructionErrorType::";

    switch (error) {
    case OperandConstructionErrorType::NoError:
        return out << "NoError";
    case OperandConstructionErrorType::InvalidTokenKind:
        return out << "InvalidTokenKind";
    case OperandConstructionErrorType::InvalidNumber:
        return out << "InvalidNumber";
    case OperandConstructionErrorType::IntegerOverflow:
        return out << "IntegerOverflow";
    case OperandConstructionErrorType::MissingQuote:
        return out << "MissingQuote";
    default:
        return out << "Unknown";
    }
}

auto Instruction::add_operand(Token const& token) -> OperandConstructionErrorType {
    switch (token.kind()) {
    case Token::Register:
        // Tokens of type `Token::Register` are always valid.
        //
        // For `Register` type tokens, the content will only be register names ranging from `R0` to
        // `R7`. Therefore, we can easily retrieve the register number using the character at
        // index 1.
        operands_.push_back(Operand::from_register(token[1] - '0'));
        return OperandConstructionErrorType::NoError;

    case Token::Label:
        // Tokens of type `Token::Label` are always valid.
        operands_.push_back(Operand::from_label(token.begin(), token.end()));
        return OperandConstructionErrorType::NoError;

    case Token::Immediate:
    case Token::Number: {
        std::string const token_content = token.content();

        // Check whether it is a valid number.
        if (!is_valid_number(token_content)) {
            return OperandConstructionErrorType::InvalidNumber;
        }

        // Check if the integer value exceeds the range of a 16-bit signed integer.
        bool ok = true;
        std::int16_t const value = to_integer(token_content, &ok);

        if (ok) {
            operands_.push_back(
                Operand::from_integer(/*is_immediate=*/token.kind() == Token::Immediate, value)
            );
            return OperandConstructionErrorType::NoError;
        } else {
            return OperandConstructionErrorType::IntegerOverflow;
        }
    }

    case Token::String:
        // Check whether the content of the `String` token contains a closing quotation mark. Note
        // that we cannot simply check whether the last character of the string is `"`.
        if (token.size() > 1 && token.back() == '"') {
            operands_.push_back(Operand::from_string_literal(token.begin(), token.end()));
            return OperandConstructionErrorType::NoError;
        } else {
            return OperandConstructionErrorType::MissingQuote;
        }

    default:
        // All other token types cannot be used as operands.
        return OperandConstructionErrorType::InvalidTokenKind;
    }
}

void Instruction::set_opcode(Token const& token) {
    // clang-format off
    static std::unordered_map<std::string, Opcode> const opcode_map = {
#define OPCODE(name) { #name, Opcode::name },
#define PSEUDO(name) { "." #name, Opcode::name },
#include "assembler/opcode.def"
    };
    // clang-format on

    assert(token.kind() == Token::Opcode || token.kind() == Token::Pseudo);

    auto const iter = opcode_map.find(token.content());
    assert(iter != opcode_map.end());

    set_opcode(iter->second);
}

auto Instruction::get_opcode_spelling() const -> char const* {
    switch (opcode_) {
#define OPCODE(name)                                                                               \
    case name:                                                                                     \
        return #name;
#define PSEUDO(name)                                                                               \
    case name:                                                                                     \
        return "." #name;
#include "assembler/opcode.def"

    default:
        return "UnknownOp";
    }
}

auto Instruction::validate_and_emit_diagnostics() const -> bool {
    // Check if a label is attached to an instruction that should not have a label.
    if (!allows_label() && has_label()) {
        std::cout << "error: instruction `" << *this << "` does not allow a label\n";
        return false;
    }

    // Get the expected operand types for the instruction.
    std::vector<std::vector<Operand::OperandType>> const expected_operands =
        expected_operand_types();

    // Check if the number of operands provided matches the expected number of operands for the
    // instruction. In the current implementation, we assume that for every expected operand type
    // list, the number of operands is consistent.
    std::size_t const expected_operand_size = expected_operands.front().size();
    if (operands_.size() != expected_operand_size) {
        std::cout << "error: instruction `" << *this << "` expects " << expected_operand_size
                  << " operand(s), but got " << operands_.size() << " operand(s)\n";
        return false;
    }

    // Check if the types of the provided operands match the expected types.

    // The following two variables are used to record the information about the first mismatched
    // operand. Since some instructions may expect multiple operand type lists, we also need to
    // record the expected operand type in case of a mismatch.

    // The index of the first mismatched operand.
    std::size_t mismatched_operand_index = 0;
    // The expected type for the mismatched operand.
    Operand::OperandType expected_operand_type {};

    for (std::vector<Operand::OperandType> const& expected : expected_operands) {
        // For the expected operand type list `expected`, find the first operand where it mismatches
        // with the actual operand types.
        auto const mismatched_iters = std::mismatch(
            operands_.begin(),
            operands_.end(),
            expected.begin(),
            [](Operand const& lhs, Operand::OperandType rhs) { return lhs.type() == rhs; }
        );

        // Record the index of the mismatched operand. If there is no mismatch, the value of
        // `mismatched_operand_index` will be the number of operands.
        mismatched_operand_index = std::distance(operands_.begin(), mismatched_iters.first);

        if (mismatched_iters.first == operands_.end()) {
            // All operand types match. As soon as we find one matching operand type list, we stop
            // checking.
            break;
        } else {
            // Record the information of the mismatched operand.
            //
            // In the current implementation, we record the information about the mismatched operand
            // from the last operand type list that encountered a mismatch. A better approach would
            // be to record the operand type list with the highest degree of matching.
            expected_operand_type = *mismatched_iters.second;
        }
    }

    if (mismatched_operand_index != operands_.size()) {
        // None of the operand type lists matched the user's provided operand list. Report
        // diagnostic message.
        std::cout << "error: operand " << mismatched_operand_index + 1 << " of instruction `"
                  << *this << "` should be of type `" << expected_operand_type << "`, but got `"
                  << operands_[mismatched_operand_index].type() << "`\n";

        return false;
    }

    // If the current instruction has an immediate operand, check if its range is valid.
    auto const imm_operand_iter =
        std::find_if(operands_.begin(), operands_.end(), [](Operand const& operand) {
            return operand.type() == Operand::Immediate || operand.type() == Operand::Number;
        });

    if (imm_operand_iter != operands_.end()) {
        // Get the valid range for the immediate operand of the current instruction.
        auto const valid_range = immediate_range();
        // Get the value of the immediate operand provided by the user.
        std::int16_t const imm_value = imm_operand_iter->type() == Operand::Immediate
            ? imm_operand_iter->immediate_value()
            : imm_operand_iter->regular_decimal();

        // Check if the immediate operand provided by the user is within the valid range.
        if (imm_value < valid_range.first || imm_value > valid_range.second) {
            std::cout << "error: immediate operand " << *imm_operand_iter << " of instruction `"
                      << *this << "` is out of range [" << valid_range.first << ", "
                      << valid_range.second << "]\n";
            return false;
        }
    }

    return true;
}

auto Instruction::allows_label() const -> bool {
    return opcode_ != Opcode::ORIG && opcode_ != Opcode::END;
}

auto Instruction::expected_operand_types() const -> std::vector<std::vector<Operand::OperandType>> {
    // clang-format off
    switch (opcode_) {
    case ADD: case AND:
        return {
            { Operand::Register, Operand::Register, Operand::Register  },
            { Operand::Register, Operand::Register, Operand::Immediate },
        };

    case BRn: case BRz: case BRp: case BR: case BRzp: case BRnp: case BRnz: case BRnzp: case JSR:
        return { { Operand::Label },
                 { Operand::Immediate} };

    case JMP: case JSRR:
        return { { Operand::Register } };

    case LD: case LDI: case LEA: case ST: case STI:
        return { { Operand::Register, Operand::Label } };

    case LDR: case STR:
        return { { Operand::Register, Operand::Register, Operand::Immediate } };

    case NOT:
        return { { Operand::Register, Operand::Register } };

    case RET: case RTI: case GETC: case OUT: case PUTS: case IN: case PUTSP: case HALT: case END:
    default:
        return { {} };

    case TRAP: case ORIG: case FILL:
        return { { Operand::Immediate } };

    case BLKW:
        return { { Operand::Number } };

    case STRINGZ:
        return { { Operand::StringLiteral } };
    }
    // clang-format on
}

auto operator<<(std::ostream& out, Instruction const& instr) -> std::ostream& {
    // Check if the instruction contains a label.
    if (instr.has_label()) {
        out << instr.get_label() << ' ';
    }

    // Print the opcode of the instruction.
    out << instr.get_opcode_spelling();

    // Print the operand list.
    if (instr.operand_size() != 0) {
        out << ' ';
        for (std::size_t i = 0; i != instr.operand_size(); ++i) {
            if (i != 0) {
                out << ", ";
            }

            out << instr.get_operand(i);
        }
    }

    return out;
}

//==================================================================================================
// C bindings for the `Instruction` class.
//==================================================================================================

namespace {
auto unwrap(InstructionRef instr) -> Instruction& {
    return *reinterpret_cast<Instruction*>(instr);
}

auto wrap(Operand const& operand) -> OperandRef {
    return reinterpret_cast<OperandRef>(&operand);
}

auto unwrap(TokenRef token) -> Token const& {
    return *reinterpret_cast<Token const*>(token);
}
}  // namespace

auto instruction_get_operand(InstructionRef instruction, std::size_t index) -> OperandRef {
    return wrap(unwrap(instruction).get_operand(index));
}

auto instruction_operand_size(InstructionRef instruction) -> std::size_t {
    return unwrap(instruction).operand_size();
}

void instruction_set_label(InstructionRef instruction, TokenRef token) {
    unwrap(instruction).set_label(unwrap(token));
}

auto instruction_get_label(InstructionRef instruction) -> char const* {
    return unwrap(instruction).get_label().c_str();
}

auto instruction_has_label(InstructionRef instruction) -> int {
    return unwrap(instruction).has_label();
}

void instruction_set_opcode(InstructionRef instruction, TokenRef token) {
    unwrap(instruction).set_opcode(unwrap(token));
}

auto instruction_get_opcode(InstructionRef instruction) -> InstructionOpcode {
    Instruction::Opcode const opcode = unwrap(instruction).get_opcode();
    return static_cast<InstructionOpcode>(
        static_cast<std::underlying_type<Instruction::Opcode>::type>(opcode)
    );
}

auto instruction_is_unknown(InstructionRef instruction) -> int {
    return unwrap(instruction).is_unknown();
}

void instruction_set_address(InstructionRef instruction, std::uint16_t address) {
    unwrap(instruction).set_address(address);
}

auto instruction_get_address(InstructionRef instruction) -> std::uint16_t {
    return unwrap(instruction).get_address();
}

#ifndef USE_CPP
namespace {
auto wrap(Instruction const& instr) -> InstructionRef {
    return reinterpret_cast<InstructionRef>(const_cast<Instruction*>(&instr));
}
}  // namespace

auto Instruction::immediate_range() const -> std::pair<std::int16_t, std::int16_t> {
    std::int16_t lb = 0, ub = 0;
    // Get the immediate range for the current instruction. This function will be implemented by the
    // student.
    //
    // Note that `wrap` will `const_cast` the `const` qualifier of `this` away. But the function
    // will not modify the object.
    get_instruction_immediate_range(wrap(*this), &lb, &ub);
    return { lb, ub };
}
#endif
