#include "assembler/instruction.hpp"
#include "assembler/operand.hpp"
#include "assembler/token.hpp"

#include "gtest/gtest.h"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>

namespace {
/// Helper function to construct a `Token` object with the specified `kind` and `content`, allowing
/// us to directly specify the content of the `Token`.
auto construct_token(Token::TokenKind kind, char const* content) -> Token {
    return Token(kind, content, content + std::strlen(content));
}
}  // namespace

TEST(InstructionTest, AddRegisterOperand) {
    Instruction instr;

    // Tokens of type `Token::Register` are always valid.
    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Register, "R0")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 1);
    EXPECT_EQ(instr.get_operand(0).type(), Operand::Register);
    EXPECT_EQ(instr.get_operand(0).register_id(), 0);

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Register, "R3")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 2);
    EXPECT_EQ(instr.get_operand(0).type(), Operand::Register);
    EXPECT_EQ(instr.get_operand(0).register_id(), 0);
    EXPECT_EQ(instr.get_operand(1).type(), Operand::Register);
    EXPECT_EQ(instr.get_operand(1).register_id(), 3);

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Register, "R7")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 3);
    EXPECT_EQ(instr.get_operand(0).type(), Operand::Register);
    EXPECT_EQ(instr.get_operand(0).register_id(), 0);
    EXPECT_EQ(instr.get_operand(1).type(), Operand::Register);
    EXPECT_EQ(instr.get_operand(1).register_id(), 3);
    EXPECT_EQ(instr.get_operand(2).type(), Operand::Register);
    EXPECT_EQ(instr.get_operand(2).register_id(), 7);
}

/// Helper function to check the conversion of integer values to immediate operands. The provided
/// `constructor` function is used to convert the integer to a string, which is then passed to the
/// `Instruction::add_operand` function.
template <class Func>
void check_integer_range_conversion(int beg, int end, Func constructor) {
    for (int val = beg; val != end; ++val) {
        Instruction instr;

        // Convert the integer to a string by the `constructor`. We do not use `std::string` and
        // `std::to_string` to avoid frequent memory allocations.
        char integer_content[64];
        constructor(val, integer_content);

        EXPECT_EQ(
            instr.add_operand(construct_token(Token::Immediate, integer_content)),
            OperandConstructionErrorType::NoError
        );

        EXPECT_EQ(instr.operand_size(), 1);
        EXPECT_EQ(instr.get_operands().back().type(), Operand::Immediate);
        EXPECT_EQ(instr.get_operands().back().immediate_value(), static_cast<std::int16_t>(val))
            << "input: " << integer_content;
    }
}

TEST(InstructionTest, AddIntegerOperand) {
    // Valid immediate values.
    {
        Instruction instr;

        char const* const inputs[] = {
            "#0",      "+0",      "-0",      "#1",     "#+1",      "#-1",
            "#000",    "#-000",   "+000",    "#42",    "#0042",    "#-123",
            "#-00123", "#5432",   "#-12123", "#32767", "#-32768",  "#65535",
            "x0",      "x1",      "x000",    "x001",   "x42",      "x0042",
            "x7FFF",   "x007FFF", "xFFFF",   "xb",     "x1b",      "xabc",
            "xCbE",    "b0",      "b00",     "b1",     "b0001010", "b111111111111111",
        };
        std::int16_t const expected[] = {
            0,      0,      0,      1,     1,      -1,    0,     0, 0, 42, 42,   -123,
            -123,   5432,   -12123, 32767, -32768, -1,    0,     1, 0, 1,  0x42, 0x42,
            0x7FFF, 0x7FFF, -1,     0xB,   0x1B,   0xABC, 0xCBE, 0, 0, 1,  10,   0x7FFF,
        };

        // We can also use `std::size()` to get the size of the array, which is a C++17 feature.
        constexpr std::size_t arr_size = sizeof(inputs) / sizeof(inputs[0]);
        for (std::size_t i = 0; i != arr_size; ++i) {
            EXPECT_EQ(
                instr.add_operand(construct_token(Token::Immediate, inputs[i])),
                OperandConstructionErrorType::NoError
            );
            EXPECT_EQ(instr.operand_size(), i + 1);
            EXPECT_EQ(instr.get_operands().back().type(), Operand::Immediate);
            EXPECT_EQ(instr.get_operands().back().immediate_value(), expected[i]);
        }
    }

    // Test that all possible immediate values are handled correctly.
    {
        // Decimal immediate values.
        check_integer_range_conversion(
            static_cast<int>(std::numeric_limits<std::int16_t>::min()),
            static_cast<int>(std::numeric_limits<std::uint16_t>::max()),
            // We explicitly specify the size of the array to cause a compilation error if the size
            // of the array is changed in the future.
            [](int val, char(&output)[64]) { std::snprintf(output, sizeof(output), "#%d", val); }
        );

        // Hexadecimal immediate values.
        check_integer_range_conversion(
            0,
            static_cast<int>(std::numeric_limits<std::uint16_t>::max()),
            [](int val, char(&output)[64]) { std::snprintf(output, sizeof(output), "x%x", val); }
        );
        check_integer_range_conversion(
            0,
            static_cast<int>(std::numeric_limits<std::uint16_t>::max()),
            [](int val, char(&output)[64]) { std::snprintf(output, sizeof(output), "x%X", val); }
        );

        // Binary immediate values.
        check_integer_range_conversion(
            0,
            static_cast<int>(std::numeric_limits<std::uint16_t>::max()),
            [](int val, char(&output)[64]) {
                std::snprintf(
                    output,
                    sizeof(output),
                    "b%s",
                    std::bitset<16>(val).to_string().c_str()
                );
            }
        );
    }

    // Valid regular decimal numbers.
    {
        Instruction instr;

        char const* const inputs[] = {
            "0",  "+0",   "-0",   "1",      "+1",   "-1",     "000",   "-000",   "+000",
            "42", "0042", "-123", "-00123", "5432", "-12123", "32767", "-32768", "65535",
        };
        std::int16_t const expected[] = {
            0, 0, 0, 1, 1, -1, 0, 0, 0, 42, 42, -123, -123, 5432, -12123, 32767, -32768, -1,
        };

        // We can also use `std::size()` to get the size of the array, which is a C++17 feature.
        constexpr std::size_t arr_size = sizeof(inputs) / sizeof(inputs[0]);
        for (std::size_t i = 0; i != arr_size; ++i) {
            EXPECT_EQ(
                instr.add_operand(construct_token(Token::Number, inputs[i])),
                OperandConstructionErrorType::NoError
            );
            EXPECT_EQ(instr.operand_size(), i + 1);
            EXPECT_EQ(instr.get_operands().back().type(), Operand::Number);
            EXPECT_EQ(instr.get_operands().back().regular_decimal(), expected[i]);
        }
    }

    // Test that all possible regular decimal numbers are handled correctly.
    check_integer_range_conversion(
        static_cast<int>(std::numeric_limits<std::int16_t>::min()),
        static_cast<int>(std::numeric_limits<std::uint16_t>::max()),
        [](int val, char(&output)[64]) { std::snprintf(output, sizeof(output), "%d", val); }
    );

    // Invalid immediate values.
    {
        Instruction instr;

        char const* const inputs[] = { "#", "#+", "#-", "x", "x+", "x-", "b", "b+", "b-" };

        for (char const* const input : inputs) {
            EXPECT_EQ(
                instr.add_operand(construct_token(Token::Immediate, input)),
                OperandConstructionErrorType::InvalidNumber
            );
            EXPECT_EQ(instr.operand_size(), 0);
        }
    }

    // Invalid regular decimal numbers.
    {
        Instruction instr;

        char const* const inputs[] = { "+", "-" };

        for (char const* const input : inputs) {
            EXPECT_EQ(
                instr.add_operand(construct_token(Token::Number, input)),
                OperandConstructionErrorType::InvalidNumber
            );
            EXPECT_EQ(instr.operand_size(), 0);
        }
    }

    // Integer out of range.
    {
        Instruction instr;

        // The following inputs are out of range for a 16-bit signed integer.
        char const* const inputs[] = {
            "#65536",  "#-32769", "#65537",
            "#-32770", "#100000", "#-100000",
            "x10000",  "x10001",  "#b10000000000000000",
        };

        for (char const* const input : inputs) {
            EXPECT_EQ(
                instr.add_operand(construct_token(Token::Immediate, input)),
                OperandConstructionErrorType::IntegerOverflow
            );
            EXPECT_EQ(instr.operand_size(), 0);
        }
    }

    {
        Instruction instr;

        // The following inputs are out of range for a 16-bit signed integer.
        char const* const inputs[] = {
            "65536", "-32769", "65537", "-32770", "100000", "-100000",
        };

        for (char const* const input : inputs) {
            EXPECT_EQ(
                instr.add_operand(construct_token(Token::Number, input)),
                OperandConstructionErrorType::IntegerOverflow
            );
            EXPECT_EQ(instr.operand_size(), 0);
        }
    }
}

TEST(InstructionTest, AddStringLiteralOperand) {
    Instruction instr;

    // Valid string literals.
    EXPECT_EQ(
        instr.add_operand(construct_token(Token::String, "\"\"")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 1);
    EXPECT_EQ(instr.get_operands().back().type(), Operand::StringLiteral);
    EXPECT_EQ(instr.get_operands().back().string_literal(), "");

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::String, "\"Hello\"")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 2);
    EXPECT_EQ(instr.get_operands().back().type(), Operand::StringLiteral);
    EXPECT_EQ(instr.get_operands().back().string_literal(), "Hello");

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::String, "\"!@#$%^&*()\"")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 3);
    EXPECT_EQ(instr.get_operands().back().type(), Operand::StringLiteral);
    EXPECT_EQ(instr.get_operands().back().string_literal(), "!@#$%^&*()");

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::String, "\"")),
        OperandConstructionErrorType::MissingQuote
    );
    EXPECT_EQ(instr.operand_size(), 3);

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::String, "\"Hello")),
        OperandConstructionErrorType::MissingQuote
    );
    EXPECT_EQ(instr.operand_size(), 3);
}

TEST(InstructionTest, AddLabelOperand) {
    Instruction instr;

    // Valid label operands.
    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Label, "LOOP")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 1);
    EXPECT_EQ(instr.get_operands().back().type(), Operand::Label);
    EXPECT_EQ(instr.get_operands().back().label(), "LOOP");

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Label, "END")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 2);
    EXPECT_EQ(instr.get_operands().back().type(), Operand::Label);
    EXPECT_EQ(instr.get_operands().back().label(), "END");

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Label, "HELLO")),
        OperandConstructionErrorType::NoError
    );
    EXPECT_EQ(instr.operand_size(), 3);
    EXPECT_EQ(instr.get_operands().back().type(), Operand::Label);
    EXPECT_EQ(instr.get_operands().back().label(), "HELLO");
}

TEST(InstructionTest, AddInvalidOperand) {
    Instruction instr;

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Opcode, "ADD")),
        OperandConstructionErrorType::InvalidTokenKind
    );
    EXPECT_EQ(instr.operand_size(), 0);

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::EOL, "\n")),
        OperandConstructionErrorType::InvalidTokenKind
    );
    EXPECT_EQ(instr.operand_size(), 0);

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::End, "")),
        OperandConstructionErrorType::InvalidTokenKind
    );
    EXPECT_EQ(instr.operand_size(), 0);

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Comma, ",")),
        OperandConstructionErrorType::InvalidTokenKind
    );
    EXPECT_EQ(instr.operand_size(), 0);

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Unknown, "")),
        OperandConstructionErrorType::InvalidTokenKind
    );
    EXPECT_EQ(instr.operand_size(), 0);

    EXPECT_EQ(
        instr.add_operand(construct_token(Token::Pseudo, ".ORIG")),
        OperandConstructionErrorType::InvalidTokenKind
    );
    EXPECT_EQ(instr.operand_size(), 0);
}

TEST(InstructionTest, SetLabel) {
    Instruction instr;

    instr.set_label("abc");
    EXPECT_EQ(instr.get_label(), "abc");

    instr.set_label("def");
    EXPECT_EQ(instr.get_label(), "def");

    instr.set_label(construct_token(Token::Label, "Hello"));
    EXPECT_EQ(instr.get_label(), "Hello");

    instr.set_label(construct_token(Token::Label, "World"));
    EXPECT_EQ(instr.get_label(), "World");
}

TEST(InstructionTest, SetOpcode) {
    Instruction instr;

    instr.set_opcode(Instruction::ADD);
    EXPECT_EQ(instr.get_opcode(), Instruction::ADD);

    instr.set_opcode(Instruction::AND);
    EXPECT_EQ(instr.get_opcode(), Instruction::AND);

    instr.set_opcode(construct_token(Token::Opcode, "BR"));
    EXPECT_EQ(instr.get_opcode(), Instruction::BR);

    instr.set_opcode(construct_token(Token::Opcode, "JMP"));
    EXPECT_EQ(instr.get_opcode(), Instruction::JMP);

    instr.set_opcode(construct_token(Token::Pseudo, ".ORIG"));
    EXPECT_EQ(instr.get_opcode(), Instruction::ORIG);

    instr.set_opcode(construct_token(Token::Pseudo, ".END"));
    EXPECT_EQ(instr.get_opcode(), Instruction::END);
}
