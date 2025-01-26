#ifndef ASSEMBLER_C_PARSER_H
#define ASSEMBLER_C_PARSER_H

#include "assembler-c/extern_c.h"
#include "assembler-c/instruction.h"
#include "assembler-c/token.h"

EXTERN_C_BEGIN

typedef struct OpaqueParser* ParserRef;

/// Returns the current token held by `parser`.
TokenRef parser_get_current_token(ParserRef parser);
/// Parses a token starting from the current position and moves the current position to the end of
/// the token. The generated token will be returned by this function. It will also saves the token
/// in the parser and you can get it by calling `parser_get_current_token()` later.
TokenRef parser_move_to_next_token(ParserRef parser);
/// Parses an operand list starting from the current token. The parsed operands are added to the
/// instruction `instr`. Returns the modified instruction. If an error is encountered during this
/// process, diagnostic information is emitted and an unknown instruction is returned.
void parser_parse_operand_list(ParserRef parser, InstructionRef instr);
/// This function is used to generate diagnostic messages when an error occurs while setting the
/// opcode from the current token. We can only construct an opcode from tokens of type
/// `Token::Opcode` and `Token::Pseudo`. If an incorrect token type is used, we will generate an
/// error message.
void parser_emit_opcode_diag_at_current_token(ParserRef parser);

EXTERN_C_END

#endif  // ASSEMBLER_C_PARSER_H
