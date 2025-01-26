#ifndef ASSEMBLER_C_OPERAND_H
#define ASSEMBLER_C_OPERAND_H

#include "assembler-c/extern_c.h"

#include <stddef.h>
#include <stdint.h>

EXTERN_C_BEGIN

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
/// of the string text (excluding the surrounding double quotes). You can call the
/// `operand_get_type()` function to obtain the type of the `Operand` and then call
/// `operand_get_register_id()`, `operand_get_immediate_value()`, `operand_get_label()`, or
/// `operand_get_string_literal()` to get the specific content of the operand.
typedef struct OpaqueOperand const* OperandRef;

/// Represents the kinds of operands that appear in LC-3. For the description of each kind, refer to
/// the comments in the `Operand` class definition.
typedef enum {
    RegisterOperand,
    ImmediateOperand,
    NumberOperand,
    LabelOperand,
    StringLiteralOperand,
} OperandType;

/// Returns the kind of the operand `operand`.
OperandType operand_get_type(OperandRef operand);
/// Returns the register number that the operand represents. For example, if the operand represents
/// register `R0`, this function returns 0.
///
/// This function should only be called if `operand_get_type()` returns `Register`, otherwise the
/// behavior is undefined.
uint8_t operand_get_register_id(OperandRef operand);
/// Returns the immediate value that the operand represents. For example, if the operand comes from
/// the immediate token `#42`, this function returns 42. Note that the return type is a 16-bit
/// signed integer.
///
/// This function should only be called if `operand_get_type()` returns `Immediate`, otherwise the
/// behavior is undefined.
int16_t operand_get_immediate_value(OperandRef operand);
/// Returns the regular decimal number that the operand represents. For example, if the operand
/// comes from the decimal number token `-42`, this function returns -42.
///
/// This function should only be called if `operand_get_type()` returns `Number`, otherwise the
/// behavior is undefined.
int16_t operand_get_regular_decimal(OperandRef operand);
/// Returns the label that the operand represents. For example, if the operand comes from the label
/// token `LOOP`, this function returns the string `LOOP`.
///
/// Note that the returned string is not null-terminated. You must provide a variable to store the
/// length of the string. You can pass a pointer to the variable to the `length` parameter.
///
/// This function should only be called if `operand_get_type()` returns `Label`, otherwise the
/// behavior is undefined.
char const* operand_get_label(OperandRef operand, size_t* length);
/// Returns the string literal that the operand represents. For example, if the operand comes from
/// the string literal token `"Hello"`, this function returns the string `Hello`. Note that the
/// returned result does not include the surrounding quotes.
///
/// Note that the returned string is not null-terminated. You must provide a variable to store the
/// length of the string. You can pass a pointer to the variable to the `length` parameter.
///
/// This function should only be called if `operand_get_type()` returns `StringLiteral`, otherwise
/// the behavior is undefined.
char const* operand_get_string_literal(OperandRef operand, size_t* length);

EXTERN_C_END

#endif  // ASSEMBLER_C_OPERAND_H
