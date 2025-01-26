#ifndef ASSEMBLER_C_INSTRUCTION_H
#define ASSEMBLER_C_INSTRUCTION_H

#include "assembler-c/extern_c.h"
#include "assembler-c/operand.h"
#include "assembler-c/token.h"

#include <stddef.h>
#include <stdint.h>

EXTERN_C_BEGIN

typedef enum {
    UnknownOp,
#define OPCODE(name) name##Op,
#define PSEUDO(name) name##Op,
#include "assembler/opcode.def"
} InstructionOpcode;

typedef struct OpaqueInstruction* InstructionRef;
/// Returns the operand at index `index` of the instruction `instruction`.
OperandRef instruction_get_operand(InstructionRef instruction, size_t index);
/// Returns the number of operands in the instruction `instruction`.
size_t instruction_operand_size(InstructionRef instruction);
/// Sets the label of the instruction `instruction` to the content of the token `label`.
void instruction_set_label(InstructionRef instruction, TokenRef token);
/// Returns the label of the instruction `instruction`.
char const* instruction_get_label(InstructionRef instruction);
/// Returns whether the instruction `instruction` has a label.
int instruction_has_label(InstructionRef instruction);
/// Sets the opcode based on the content of the `token`. The `token` must be of type `Opcode` or
/// `Pseudo`.
///
/// This method converts the textual content of the `token` into an `InstructionOpcode` enum value.
void instruction_set_opcode(InstructionRef instruction, TokenRef token);
/// Returns the opcode of the instruction `instruction`.
InstructionOpcode instruction_get_opcode(InstructionRef instruction);
/// Returns whether the instruction `instruction` is unknown.
///
/// An unknown instruction is typically used to represent a failed instruction construction. For
/// example, if a syntax error is encountered during parsing, an unknown instruction is returned.
int instruction_is_unknown(InstructionRef instruction);
/// Sets the address of the instruction `instruction`.
void instruction_set_address(InstructionRef instruction, uint16_t address);
/// Returns the address of the instruction `instruction`.
uint16_t instruction_get_address(InstructionRef instruction);

EXTERN_C_END

#endif  // ASSEMBLER_C_INSTRUCTION_H
