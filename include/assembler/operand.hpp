#ifndef ASSEMBLER_OPERAND_HPP
#define ASSEMBLER_OPERAND_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <ostream>
#include <string>

/// Represents an operand in an LC-3 assembly instruction.
///
/// We refer to the part of an LC-3 assembly instruction other than the label and opcode as
/// operands. Operands can be registers, immediates, regular decimal numbers, labels, or string
/// literals. For example, in the following instruction:
///
///     LOOP AND R3, R3, #0
///
/// There are three operands: `R3`, `R3`, and `#0`. The first two operands are register operands,
/// and the last operand is an immediate operand.
///
/// The `Operand` class can represent different types of operands. When it represents a register
/// operand, it stores the register number (e.g., for register `R0`, it stores the value 0). When
/// representing an immediate or number, it directly stores the value of the integer (as a 16-bit
/// signed integer). When representing a label, it stores two pointers to the beginning and end of
/// the label. When representing a string literal, it stores two pointers to the beginning and end
/// of the string text (excluding the surrounding double quotes). You can call the `type()` member
/// function to obtain the type of the `Operand` and then call `register_id()`, `immediate_value()`,
/// `label()`, or `string_literal()` to get the specific content of the operand.
class Operand {
public:
    enum OperandType : std::uint8_t {
        Register,
        Immediate,
        Number,
        Label,
        StringLiteral,
    };

    auto type() const -> OperandType {
        return type_;
    }

    /// Returns the register number that the operand represents. For example, if the operand
    /// represents register `R0`, this function returns 0.
    ///
    /// This function should only be called if `type()` returns `Register`, otherwise the behavior
    /// is undefined.
    auto register_id() const -> std::uint8_t {
        assert(type_ == Register);
        return reg_id_;
    }

    /// Returns the immediate value that the operand represents. For example, if the operand comes
    /// from the immediate token `#42`, this function returns 42. Note that the return type is a
    /// 16-bit signed integer.
    ///
    /// This function should only be called if `type()` returns `Immediate`, otherwise the behavior
    /// is undefined.
    auto immediate_value() const -> std::int16_t {
        assert(type_ == Immediate);
        return imm_;
    }

    /// Returns the regular decimal number that the operand represents. For example, if the operand
    /// comes from the decimal number token `-42`, this function returns -42.
    ///
    /// This function should only be called if `type()` returns `Number`, otherwise the behavior is
    /// undefined.
    auto regular_decimal() const -> std::int16_t {
        assert(type_ == Number);
        return imm_;
    }

    /// Returns the label that the operand represents. For example, if the operand comes from the
    /// label token `LOOP`, this function returns the string `LOOP`.
    ///
    /// This function should only be called if `type()` returns `Label`, otherwise the behavior is
    /// undefined.
    auto label() const -> std::string {
        assert(type_ == Label);
        return { string_content_[0], string_content_[1] };
    }

    /// Returns the string literal that the operand represents. For example, if the operand comes
    /// from the string literal token `"Hello"`, this function returns the string `Hello`. Note that
    /// the returned result does not include the surrounding quotes.
    ///
    /// This function should only be called if `type()` returns `StringLiteral`, otherwise the
    /// behavior is undefined.
    auto string_literal() const -> std::string {
        assert(type_ == StringLiteral);
        return { string_content_[0], string_content_[1] };
    }

    /// Constructs an `Operand` object from a register ID, so that the operand represents a
    /// register.
    static auto from_register(std::uint8_t reg_id) -> Operand {
        return Operand(RegisterTag {}, reg_id);
    }

    /// Constructs an `Operand` object from an immediate or regular number. If `is_immediate` is
    /// `true`, the operand represents an immediate (of type `Immediate`); otherwise, it represents
    /// a regular number (of type `Number`).
    static auto from_integer(bool is_immediate, std::int16_t imm) -> Operand {
        if (is_immediate) {
            return Operand(ImmediateTag {}, imm);
        } else {
            return Operand(NumberTag {}, imm);
        }
    }

    /// Constructs an `Operand` object from a label, so that the operand represents a label. `begin`
    /// and `end` point to the first character of the label and the position just past the last
    /// character, respectively.
    static auto from_label(char const* begin, char const* end) -> Operand {
        return Operand(LabelTag {}, begin, end);
    }

    /// Constructs an `Operand` object from a string literal, so that the operand represents a
    /// string literal. `begin` and `end` point to the first character of the string literal and the
    /// position just past the last character, respectively. Note that the surrounding quotes are
    /// not included.
    static auto from_string_literal(char const* begin, char const* end) -> Operand {
        // We need to ensure that [begin, end) contains at least 2 characters, i.e., the string's
        // surrounding quotes.
        assert(end - begin >= 2);
        // Remove the surrounding quotes.
        return Operand(StringLiteralTag {}, ++begin, --end);
    }

private:
    union {
        /// Represents the register number. For example, register `R0` will be stored as 0.
        ///
        /// This member is valid only when `type_` is `Register`.
        std::uint8_t reg_id_;
        /// Represents the value of an immediate or regular number.
        ///
        /// We use a 16-bit integer to store the value since integer operands are no larger than 16
        /// bits in LC-3. Some instructions further constrain the range of the integer operand,
        /// which requires additional checks based on the instruction type.
        ///
        /// This member is valid only when `type_` is `Immediate` or `Number`.
        std::int16_t imm_;
        /// Represents the content of a label or a string literal.
        ///
        /// For labels, this member stores two pointers to the first character and the position just
        /// past the last character of the label. For string literals, it stores in a similar way
        /// but excludes the surrounding quotes.
        ///
        /// We do not use `std::string` to store labels because that would cause the `Operand`'s
        /// special member functions to be defined as deleted, requiring us to manually implement
        /// them. A better approach would be to use `std::string_view`, but that is a C++17 feature.
        ///
        /// This member is valid only when `type_` is `Label` or `StringLiteral`.
        std::array<char const*, 2> string_content_;
    };
    /// The type of the operand. The appropriate member will be accessed based on the operand type.
    OperandType type_;

    // The following 5 tag types are used to explicitly specify the operand type (i.e., the `type_`
    // member) when constructing an `Operand` object. This avoids the need to write verbose `switch`
    // statements.

    struct RegisterTag { };
    struct ImmediateTag { };
    struct NumberTag { };
    struct LabelTag { };
    struct StringLiteralTag { };

    Operand(RegisterTag, std::uint8_t reg_id) : reg_id_(reg_id), type_(Register) { }
    Operand(ImmediateTag, std::int16_t imm) : imm_(imm), type_(Immediate) { }
    Operand(NumberTag, std::int16_t imm) : imm_(imm), type_(Number) { }
    Operand(LabelTag, char const* begin, char const* end) :
        string_content_ { begin, end }, type_(Label) { }
    Operand(StringLiteralTag, char const* begin, char const* end) :
        string_content_ { begin, end }, type_(StringLiteral) { }
};

/// Overloads `operator<<` to support printing an operand type.
auto operator<<(std::ostream& out, Operand::OperandType operand_type) -> std::ostream&;

/// Overloads `operator<<` to support printing an operand.
auto operator<<(std::ostream& out, Operand const& operand) -> std::ostream&;

#endif  // ASSEMBLER_OPERAND_HPP
