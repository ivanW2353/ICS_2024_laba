#ifndef ASSEMBLER_C_SOLUTION_H
#define ASSEMBLER_C_SOLUTION_H

#include "assembler-c/assembler.h"
#include "assembler-c/extern_c.h"
#include "assembler-c/instruction.h"
#include "assembler-c/parser.h"

#include <stddef.h>
#include <stdint.h>

EXTERN_C_BEGIN

char const* parse_decimal_number(char const* current, char const* end);
char const* parse_string_literal(char const* current, char const* end);
void parse_instruction(ParserRef parser, InstructionRef instr);
int16_t string_to_integer(char const* content, int* ok);
void get_instruction_immediate_range(InstructionRef instr, int16_t* lb, int16_t* ub);
void assign_addresses(AssemblerRef assembler);
int scan_label(AssemblerRef assembler);
uint16_t translate_regular_instruction(AssemblerRef assembler, InstructionRef instr);
uint16_t* translate_pseudo(InstructionRef instr, size_t* result_length);

EXTERN_C_END

#endif  // ASSEMBLER_C_SOLUTION_H
