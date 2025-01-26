#ifndef ASSEMBLER_C_ASSEMBLER_H
#define ASSEMBLER_C_ASSEMBLER_H

#include "assembler-c/extern_c.h"
#include "assembler-c/instruction.h"
#include "assembler-c/operand.h"

#include <stddef.h>
#include <stdint.h>

EXTERN_C_BEGIN

typedef struct OpaqueAssembler* AssemblerRef;
/// Returns the size of the instruction sequence in the assembler `assembler`.
size_t assembler_get_instruction_size(AssemblerRef assembler);
/// Returns the instruction at index `index` in the assembler `assembler`.
InstructionRef assembler_get_instruction(AssemblerRef assembler, size_t index);
/// Adds the label `label` and its corresponding address `address` to the symbol table. If the label
/// already exists in the symbol table, returns `0`; otherwise, returns `1`.
int assembler_add_label(AssemblerRef assembler, char const* label, uint16_t address);
/// Returns the address of the label `label`. If the label does not exist, sets `ok` to `0`. Note
/// that `label` is not a null-terminated string.
uint16_t assembler_get_label(AssemblerRef assembler, char const* label, size_t length, int* ok);
/// Emits diagnostic information for a redefined label `label`.
void assembler_emit_label_redefinition_diag(InstructionRef instr);
/// Emits diagnostic information for a label not found in an instruction.
void assembler_emit_label_not_found_diag(OperandRef label_operand, InstructionRef instr);
/// Emits diagnostic information for an offset of a label in an instruction that is out of range.
void assembler_emit_label_offset_out_of_range_diag(
    OperandRef label_operand,
    InstructionRef instr,
    int16_t offset
);
/// Creates an array of `uint16_t` elements with size `size`. The array is used to store the
/// translation result.
uint16_t* create_translate_result_array(size_t size);
/// Frees the memory allocated for the translation result array `array`.
void free_translate_result_array(uint16_t* array);

EXTERN_C_END

#endif  // ASSEMBLER_C_ASSEMBLER_H
