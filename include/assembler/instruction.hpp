#ifndef ASSEMBLER_INSTRUCTION_HPP
#define ASSEMBLER_INSTRUCTION_HPP

#include "assembler/operand.hpp"
#include "assembler/token.hpp"

#include <cstdint>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

/// Represents the error types that may occur when constructing an `Operand` from a `Token`. This
/// type is returned when adding an operand using `Instruction::add_operand`.
enum class OperandConstructionErrorType {
    /// Indicates that no error occurred during the construction of the `Operand`, and the
    /// constructed `Operand` was successfully inserted into the `Instruction`.
    NoError,
    /// Indicates that the token used to construct the `Operand` has an invalid type.
    ///
    /// `Operand` only supports five types: `Register`, `Immediate`, `Number`, `Label`, and
    /// `StringLiteral`, which correspond to the token types `Token::Register`, `Token::Immediate`,
    /// `Token::Number`, `Token::Label`, and `Token::String`. If the token type passed in is not one
    /// of these five types, this error is returned.
    InvalidTokenKind,
    /// Invalid immediate number.
    ///
    /// The parser only checks if the characters that form the immediate number are valid, but a
    /// token composed of valid characters may still represent an invalid number. For example,
    /// `#+` consists of valid decimal characters but is not a valid immediate number.
    InvalidNumber,
    /// Indicates that the integer value represented by the token is out of range.
    ///
    /// In the `Operand`, we use a 16-bit signed integer to store integer values. If the integer
    /// value represented by the token exceeds the range of a 16-bit signed integer, this error is
    /// returned.
    ///
    /// It is worth noting that many instructions impose further restrictions on the range of
    /// integer values. For example, the `ADD` instruction only allocates 5 bits of space for an
    /// immediate operand. However, these details are not checked when constructing an `Operand`
    /// from a token, as doing so would require knowledge of which instruction the operand belongs
    /// to.
    IntegerOverflow,
    /// String literal error due to a missing closing quotation mark.
    ///
    /// When the token type is `Token::String`, this error should be reported if the content is
    /// missing a closing quotation mark.
    MissingQuote,
};

/// Outputs the error type `error` to the output stream `out`. Used for unit testing.
auto operator<<(std::ostream& out, OperandConstructionErrorType error) -> std::ostream&;

class Instruction {
public:
    enum Opcode : std::uint8_t {
        UnknownOp,
#define OPCODE(name) name,
#define PSEUDO(name) name,
#include "assembler/opcode.def"
    };

    /// Checks the content of the `token` and, if the `token` is valid, converts it into an
    /// `Operand` and inserts it into `operands_`. Otherwise, it returns an error type.
    ///
    /// Specifically, this function needs to accomplish the following two tasks:
    ///
    /// 1. Check the validity of the `token`.
    ///
    /// (1) The type of the `token` must be valid as an operand, meaning it must be one of
    /// `Token::Register`, `Token::Immediate`, `Token::Number`, `Token::Label`, or `Token::String`.
    /// If this condition is not met, return `OperandConstructionErrorType::InvalidTokenKind`.
    ///
    /// (2) Tokens of type `Token::Register` and `Token::Label` are always valid, because only valid
    /// register names will be constructed as `Register` type tokens, and there are no restrictions
    /// on the content of labels.
    ///
    /// (3) For tokens of type `Immediate` and `Number`, we need to check whether the number is
    /// valid. The parser ensures that such tokens contain only valid characters (e.g., hexadecimal
    /// immediates will not include the character `G`), but a token composed of valid characters is
    /// not necessarily a valid number. For example, `#+` is a decimal immediate composed of valid
    /// characters but is not a valid number. Such cases are uniformly treated as `InvalidNumber`
    /// errors.
    ///
    /// (4) For tokens of type `Immediate` and `Number`, we also need to check whether the integer
    /// value represented by the `token` exceeds the range of a 16-bit signed integer. In such
    /// cases, we need to return an `IntegerOverflow` error.
    ///
    /// (5) For tokens of type `String`, we need to check whether the closing quote is present. If
    /// it is missing, we should return a `MissingQuote` error. The parser ensures that the string
    /// contains an opening quote and valid string content.
    ///
    /// 2. If the `token` is valid, convert it into the appropriate type of `Operand` and insert it
    /// into `operands_`.
    auto add_operand(Token const& token) -> OperandConstructionErrorType;

    auto get_operand(std::size_t index) const -> Operand const& {
        return operands_[index];
    }

    auto get_operands() const -> std::vector<Operand> const& {
        return operands_;
    }

    auto operand_size() const -> std::size_t {
        return operands_.size();
    }

    /// Sets the label of the instruction to `token.content()`.
    void set_label(Token const& token) {
        set_label(token.content());
    }

    void set_label(std::string label) {
        label_ = std::move(label);
    }

    auto get_label() const -> std::string const& {
        return label_;
    }

    auto has_label() const -> bool {
        return !label_.empty();
    }

    /// Sets the opcode based on the content of the `token`. The `token` must be of type
    /// `Token::Opcode` or `Token::Pseudo`.
    ///
    /// This method converts the textual content of the `token` into an `Instruction::Opcode` enum
    /// value.
    void set_opcode(Token const& token);

    void set_opcode(Opcode opcode) {
        opcode_ = opcode;
    }

    auto get_opcode() const -> Opcode {
        return opcode_;
    }

    /// Returns whether the current instruction is unknown.
    ///
    /// An unknown instruction is typically used to represent a failed instruction construction. For
    /// example, if a syntax error is encountered during parsing, an unknown instruction is
    /// returned. A default-constructed `Instruction` represents an unknown instruction.
    auto is_unknown() const -> bool {
        return opcode_ == UnknownOp;
    }

    /// Returns the spelling of the opcode as a string. This function is used to print the
    /// instruction.
    auto get_opcode_spelling() const -> char const*;

    /// Validates the current instruction.
    ///
    /// This function performs further checks on the operands based on the specifics of the
    /// instruction. For example, it checks whether the number of operands matches the requirements
    /// of the instruction. It also verifies if the operand types are correct. For instance, in an
    /// `ADD` instruction, the first and second operands must be registers, and the third operand
    /// can be either a register or an immediate. For a detailed description of all the checks
    /// performed by this function, refer to the comments in the function definition.
    ///
    /// This function also emits diagnostic messages to the `std::cout` stream during validation. If
    /// the validation passes, the function returns `true`; otherwise, it returns `false`.
    auto validate_and_emit_diagnostics() const -> bool;

    void set_address(std::uint16_t address) {
        address_ = address;
    }

    auto get_address() const -> std::uint16_t {
        return address_;
    }

private:
    std::string label_;
    std::vector<Operand> operands_;
    Opcode opcode_ = UnknownOp;
    /// The address where the current instruction will be placed.
    std::uint16_t address_;

    /// Returns whether the current instruction allows a label. In the current implementation, we
    /// cannot attach labels to `.ORIG` and `.END`.
    auto allows_label() const -> bool;

    /// Returns the expected types of operands for the current instruction. Some instructions may
    /// accept a list of multiple operand types. For example, in an `ADD` instruction, the first and
    /// second operands must be registers, while the third operand can be either a register or an
    /// immediate. Therefore, we return a nested `std::vector`.
    auto expected_operand_types() const -> std::vector<std::vector<Operand::OperandType>>;

    /// Returns the range for the immediate operand in this instruction (if any). We will check if
    /// the immediate operand provided by the user exceeds this range.
    auto immediate_range() const -> std::pair<std::int16_t, std::int16_t>;
};

/// Outputs the `Instruction` object `instr` to the output stream `out`.
auto operator<<(std::ostream& out, Instruction const& instr) -> std::ostream&;

#endif  // ASSEMBLER_INSTRUCTION_HPP
