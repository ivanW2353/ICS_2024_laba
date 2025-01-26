#include "assembler-c/solution.h"

#include "assembler-c/assembler.h"
#include "assembler-c/instruction.h"
#include "assembler-c/operand.h"
#include "assembler-c/parser.h"
#include "assembler-c/token.h"

#include <stddef.h>
#include <stdint.h>

/// Parses a decimal number token, returning the end of the token.
///
/// You need to parse an integer starting from `current` until the first character that is not a
/// valid digit or reaching `end`. Note that `current` and `end` limit the valid range you can
/// process: `current` points to the start of the range you need to process, and `end` points to the
/// position after the last valid character. In other words, they form an interval [current, end).
///
/// The return value of this function is a pointer to the first character that is not a valid digit.
/// If all characters from `current` to `end` are valid digits, return `end`. In other words, the
/// interval [current, res) marks the range of the integer token, where `res` is the return value of
/// this function.
///
/// A valid decimal number token may start with an optional sign and is followed by any number of
/// digit characters (0 ~ 9).
///
/// Here are some examples of input and output:
///
/// Example 1: Suppose the string you need to process is `123Hello`, the parameters will be given
/// as:
///
///     1 2 3 H e l l o
///     ^               ^
///     |               |
///     current         end
///
/// Note that `end` points to the position after the last character `o`.
///
/// This function needs to find the first character that is not a valid digit in the range [current,
/// end). In this case, the first character that is not a valid digit is `H`, since a valid decimal
/// number cannot contain the character `H`. The function should return a pointer to the character
/// `H`.
///
/// Example 2: For input `+123`, the parameters will be given as:
///
///     + 1 2 3
///     ^       ^
///     |       |
///     current end
///
/// The function should return `end`, because all characters in the range [current, end) are valid.
///
/// Example 3: For input `+1+2`, the function should return a pointer to the second `+`, because a
/// valid integer token can only have one sign at the beginning.
///
/// Example 4: Suppose the string you need to process is `+`, the function should return `end`,
/// because `+` is a valid integer token: it starts with a `+` sign and is followed by any number
/// (in this case, 0) of valid digit characters.
///
/// Note:
///
///     1. You must ensure that `current != end` before dereferencing `current`. If you change
///        `current`, you must ensure that `current != end` is `true` when you dereference it again.
///     2. The string pointed to by `current` is not null-terminated, so you cannot use string
///        processing functions that rely on null-terminated strings, such as `strlen`, `strcmp`,
///        etc.
///     3. You return a pointer to the first character that is not valid, not to the last valid
///        character.
///     4. Even if you do not encounter an invalid character, you must stop processing and return
///        `end` when `current` reaches `end`.
char const* parse_decimal_number(char const* current, char const* end) {
    // Replace the following 5 lines with your implementation.
    if (current == end) {
        return end;
    } else {
        return ++current;
    }
}

/// Parses a string literal token, returning the end of the token.
///
/// The input to this function is similar to `parse_decimal_number()`, where [current, end) forms
/// the valid range of the string you need to process. You need to find the first character that is
/// not part of a valid string literal in the range [current, end), or stop until you reach `end`.
///
/// A valid string literal is a sequence of characters enclosed in double quotes. The string literal
/// can contain any character, including spaces, punctuation, etc. However, a string literal must be
/// on the same line. If you encounter a newline character `\n` during parsing, you should stop
/// parsing and return the position of the `\n` character. This is usually due to the user
/// forgetting to close the quotes, causing the string literal to span multiple lines.
///
/// Note that `current` points to the first character after the opening quote, not the quote itself.
///
/// Here are some examples of input and output:
///
/// Example 1: Suppose the string you need to process is `"Hello"abc`, the parameters will be given
/// as:
///
///     " H e l l o " a b c
///       ^                 ^
///       |                 |
///       current           end
///
/// Note that `current` points to the first character after the opening quote, and `end` points to
/// the position after the last character `c`.
///
/// Starting from `current`, we find the first quote after the character `o`. Since we need to
/// consume this quote, the function should return a pointer to the character `a`.
///
/// Example 2: For input `"Hello\nWorld"`, `current` points to the character `H`. During the search
/// from `current`, we did not find another quote, but encountered a newline character `\n` first.
/// Since a string literal cannot span multiple lines, we should stop parsing and return a pointer
/// to the newline character `\n`.
///
/// Example 3: For input `""abc`, `current` points to the second quote. In this case, we should
/// return a pointer to the character `a`.
///
/// Note:
///
///     1. You must ensure that `current != end` before dereferencing `current`. If you change
///        `current`, you must ensure that `current != end` is `true` when you dereference it again.
///     2. The string pointed to by `current` is not null-terminated, so you cannot use string
///        processing functions that rely on null-terminated strings, such as `strlen`, `strcmp`,
///        etc.
///     3. You should return a pointer to the first character that is not part of a valid string
///        literal, or to the newline character if the string literal is not closed.
///     4. Even if you do not encounter an invalid character, you must stop processing and return
///        `end` when `current` reaches `end`.
char const* parse_string_literal(char const* current, char const* end) {
    // Replace the following 5 lines with your implementation.
    if (current == end) {
        return end;
    } else {
        return ++current;
    }
}

/// Parses an instruction starting from the current token. You need to fill in the instruction
/// `instr` with the parsed information.
///
/// You can follow the comments in the function body to complete this function. You may use the
/// following functions during the implementation:
///
/// - `parser_get_current_token(parser)` in `parser.h`: Get the current token.
///
/// - `parser_move_to_next_token(parser)` in `parser.h`: Parse the next token and return it as the
///   current token.
///
/// - `parser_emit_opcode_diag_at_current_token(parser)` in `parser.h`: Emit diagnostic information
///   at the current token, reporting that the token cannot be an opcode.
///
/// - `instruction_set_label(instr, token)` in `instruction.h`: Add the content of `token` as a
///   label to the instruction `instr`.
///
/// - `instruction_set_opcode(instr, token)` in `instruction.h`: Convert the content of `token` to
///   an opcode and add it to the instruction `instr`.
///
/// - `token_get_kind(token)` in `token.h`: Get the kind of the token.
///
/// - You can read the comment at the beginning of `token.h` to understand the meaning of each token
///   kind. You will need to check the token kind to determine how to use the token.
void parse_instruction(ParserRef parser, InstructionRef instr) {
    // Get the current token.
    TokenRef current_token = parser_get_current_token(parser);

    // Check if the current token is of type `Label`. If it is, we add the label to `instr` and move
    // to the next token.
    //
    // Insert your code here.

    // Now `current_token` points to the opcode. If the first token was a label, `current_token`
    // points to the second token, otherwise it points to the first token.
    TokenKind const current_token_kind = token_get_kind(current_token);

    // Check whether `current_token` represents a valid opcode or pseudo-instruction. If it does
    // not, emit a diagnostic message and return.
    //
    // Insert your code here.

    // Add the opcode to `instr`.
    //
    // Insert your code here.

    // Move to the next token to continue parsing.
    //
    // Insert your code here.

    // Now we need to parse the operand list. The operand list is a sequence of tokens separated by
    // `CommaToken`.
    parser_parse_operand_list(parser, instr);
}

/// Converts the string representation of an immediate to an integer value and sets `*ok` to 0 if
/// overflow occurs.
///
/// `content` is a null-terminated string that represents a valid integer or immediate value. The
/// string may represent an immediate value with a prefix or a regular decimal integer. Regardless
/// of the presence of a prefix, when it represents a decimal integer, it may have a sign.
///
/// Overflow is defined as exceeding the maximum value of an unsigned 16-bit integer for positive
/// numbers or the minimum value of a signed 16-bit integer for negative numbers. You can use
/// `UINT16_MAX` and `INT16_MIN` to represent the maximum and minimum values of unsigned and signed
/// 16-bit integers.
///
/// Regarding hexadecimal immediates: The letters in hexadecimal immediates may be uppercase or
/// lowercase, such as `xAB`, `xAb`, or `xaB`. However, the prefix will always be the lowercase
/// letter `x`.
///
/// Here are some examples:
///
/// Example 1: For input `12`, return 12 and set `*ok` to 1.
/// Example 2: For input `#12`, return 12 and set `*ok` to 1.
/// Example 3: For input `+12`, return 12 and set `*ok` to 1.
/// Example 4: For input `-12`, return -12 and set `*ok` to 1.
/// Example 5: For input `#+12`, return 12 and set `*ok` to 1.
/// Example 6: For input `#-12`, return -12 and set `*ok` to 1.
/// Example 7: For input `x12`, return 18 and set `*ok` to 1.
/// Example 8: For input `b101`, return 5 and set `*ok` to 1.
/// Example 9: For input `65536`, set `*ok` to 0 and return any value.
/// Example 10: For input `-32769`, set `*ok` to 0 and return any value.
int16_t string_to_integer(const char* content, int* ok) {
    // Replace the following 3 lines with your implementation.
    (void) content;
    *ok = 1;
    return 0;
}

/// Returns the valid range of an immediate operand in an instruction.
///
/// You need to store the range of the immediate operand in the `lb` and `ub` parameters. These two
/// parameters represent the lower and upper bounds of the immediate operand. We have provided the
/// ranges for the `TRAPOp`, `ORIGOp`, `FILLOp`, and `BLKWOp` instructions. You need to fill in the
/// ranges for other instructions.
void get_instruction_immediate_range(InstructionRef instr, int16_t* lb, int16_t* ub) {
    // clang-format off
    switch (instruction_get_opcode(instr)) {
    case TRAPOp:
        // trapvect8
        *lb = 0;
        *ub = 255;
        break;

    case ORIGOp: case FILLOp: case BLKWOp:
        // 16-bit integer
        *lb = INT16_MIN;
        *ub = INT16_MAX;
        break;

    // Insert your code here.

    default:
        break;
    }
    // clang-format on
}

/// Assigns addresses to all instructions.
///
/// We have provided a function framework for you. You need to complete the body. During this
/// process, you may use the following functions:
///
/// - `assembler_get_instruction_size(assembler)` in `assembler.h`: Get the number of instructions
///   to process.
///
/// - `assembler_get_instruction(assembler, i)` in `assembler.h`: Get the instruction at index `i`.
///   Note that `i` is 0-based.
///
/// - `instruction_get_opcode(instr)` in `instruction.h`: Get the opcode of the instruction `instr`.
///
/// - `instruction_get_operand(instr, i)` in `instruction.h`: Get the `i`-th operand of the
///   instruction `instr`. Note that `i` is 0-based.
///
/// - `instruction_set_address(instr, address)` in `instruction.h`: Set the address of the
///   instruction `instr` to `address`.
///
/// - `operand_get_immediate_value(operand)` in `operand.h`: Get the value of the immediate
///   represented by the operand `operand`.
///
/// - `operand_get_regular_decimal(operand)` in `operand.h`: Get the value of the decimal
///   represented by the operand `operand`. You need to use this function to get the value of the
///   operand when processing the `.BLKW` instruction.
///
/// - `operand_get_string_literal(operand, &length)` in `operand.h`: Get the string literal
///   represented by the operand `operand`. You need to read the documentation of this function to
///   understand how to use it. You need to use this function to get the value of the operand when
///   processing the `.STRINGZ` instruction.
void assign_addresses(AssemblerRef assembler) {
    // The size of the instruction sequence.
    size_t const instruction_size = assembler_get_instruction_size(assembler);

    for (size_t i = 0; i != instruction_size; ++i) {
        InstructionRef const instr = assembler_get_instruction(assembler, i);

        // Replace the following line with your implementation. You may define additional variables
        // outside the loop if needed.
        (void) instr;
    }
}

/// Scans all labels in the instructions and adds the label and its associated address to the symbol
/// table. If the label already exists in the symbol table, emits a diagnostic message. Returns 1 if
/// no errors occur during this process; otherwise, returns 0.
///
/// You may use the following functions to complete this task:
///
/// - `assembler_get_instruction_size(assembler)` in `assembler.h`: Get the number of instructions
///   to process.
///
/// - `assembler_get_instruction(assembler, i)` in `assembler.h`: Get the instruction at index `i`.
///   Note that `i` is 0-based.
///
/// - `assembler_add_label(assembler, label, address)` in `assembler.h`: Add the label `label` and
///   its corresponding address `address` to the symbol table. If the label already exists in the
///   symbol table, return `0`; otherwise, return `1`.
///
/// - `assembler_emit_label_redefinition_diag(instr)` in `assembler.h`: Emit diagnostic information
///   for a redefined label attached to the instruction `instr`.
///
/// - `instruction_has_label(instr)` in `instruction.h`: Check if the instruction `instr` has a
///   label.
///
/// - `instruction_get_label(instr)` in `instruction.h`: Get the label of the instruction `instr`.
///
/// - `instruction_get_address(instr)` in `instruction.h`: Get the address of the instruction
/// `instr`.
int scan_label(AssemblerRef assembler) {
    // Replace the following 2 lines with your implementation.
    (void) assembler;
    return 1;
}

/// Translates an instruction opcode `opcode` to its corresponding 4-bit binary representation. We
/// have provided part of the implementation for you. You need to complete the rest.
uint16_t translate_opcode(InstructionOpcode opcode) {
    // clang-format off
    switch (opcode) {
    case ADDOp:
        return 1;  // 0001
    case ANDOp:
        return 5;  // 0101
    case BRnOp:  case BRzOp:  case BRpOp:  case BROp:
    case BRzpOp: case BRnpOp: case BRnzOp: case BRnzpOp:
        return 0;  // 0000
    case JMPOp:
        return 12;  // 1100
    case TRAPOp: case GETCOp:  case OUTOp: case PUTSOp: case INOp: case PUTSPOp: case HALTOp:
        return 15;  // 1111
    default:
        return 13;  // 1101
    }
    // clang-format on
}

/// Translates a register operand to its corresponding 3-bit binary representation. The register
/// operand is the `operand_idx`-th operand of the instruction `instr`. You can get this operand by
/// calling `instruction_get_operand(instr, operand_idx)`, and then get the ID of the register by
/// calling `operand_get_register_id(operand)`. We set the ID of the register to be its index in
/// LC-3, i.e., `R0` corresponds to 0, `R1` corresponds to 1, and so on.
///
/// The `position` parameter specifies the position of the register in the final binary instruction.
/// For example, if `position` is 9, it means that the binary representation of the register will
/// appear from the 9th bit to the 11th bit in the final binary instruction.
///
/// Here are some examples:
///
/// - If the operand represents the register `R1` and `position` is 3, return 0x8 (i.e., binary
///   `1000`).
/// - If the operand represents the register `R7` and `position` is 6, return 0x1C0 (i.e., binary
///   `0001 1100 0000`).
uint16_t translate_register(InstructionRef instr, size_t operand_idx, unsigned position) {
    // Replace the following 4 lines with your implementation.
    (void) instr;
    (void) operand_idx;
    (void) position;
    return 0;
}

/// Translates an immediate operand to its corresponding binary representation and truncates it to
/// `bits` bits. The immediate operand is the `operand_idx`-th operand of the instruction `instr`.
/// You can get this operand by calling `instruction_get_operand(instr, operand_idx)`, and then get
/// the value of the immediate by calling `operand_get_immediate_value(operand)`.
///
/// The `bits` parameter specifies the number of bits the immediate will occupy in the final binary
/// instruction. For example, if `bits` is 5, it means that the binary representation of the
/// immediate will appear in the lower 5 bits of the final binary instruction.
///
/// Here are some examples:
///
/// - If the operand represents the immediate `#5` and `bits` is 4, return 0x0005 (i.e., binary
///   `0000 0000 0000 0101`).
/// - If the operand represents the immediate `#-5` and `bits` is 4, return 0x000B (i.e., binary
///   `0000 0000 0000 1011`).
/// - If the operand represents the immediate `#150` and `bits` is 3, return 0x0006 (i.e., binary
///   `0000 0000 0000 0110`).
uint16_t translate_immediate(InstructionRef instr, size_t operand_idx, unsigned bits) {
    // Replace the following 4 lines with your implementation.
    (void) instr;
    (void) operand_idx;
    (void) bits;
    return 0;
}

/// Translates a label operand to its corresponding binary representation and truncates it to `bits`
/// bits. The label operand is the `operand_idx`-th operand of the instruction `instr`. You can get
/// this operand by calling `instruction_get_operand(instr, operand_idx)`, and then get the content
/// of the label by calling `operand_get_label(operand, &length)`. You may need to read the
/// documentation of the `operand_get_label()` function to understand how to use it. This function
/// is declared in `include/assembler-c/operand.h`.
///
/// Similar to `translate_immediate()`, the `bits` parameter specifies the number of bits the label
/// will occupy in the final binary instruction. For example, if `bits` is 9, it means that the
/// binary representation of the label will appear in the lower 9 bits of the final binary
/// instruction. You need to check if the offset you calculate is within the range of `bits` bits.
/// If the offset is out of this range, you need to emit a diagnostic message and return `(uint16_t)
/// -1`.
///
/// You may use the following functions to complete this task:
///
/// - `assembler_get_label(assembler, label, length, &ok)` in `assembler.h`: Look up the address of
///   the label `label` in the symbol table. If the label does not exist, set `ok` to `0`.
///
/// - `assembler_emit_label_not_found_diag(label_operand, instr)` in `assembler.h`: Emit diagnostic
///   information when the label represented by the label operand `label_operand` in the instruction
///   `instr` is not found in the symbol table.
///
/// - `assembler_emit_label_offset_out_of_range_diag(label_operand, instr, offset)` in
///   `assembler.h`: Emit diagnostic information when the offset `offset` of the label represented
///   by the label operand `label_operand` in the instruction `instr` is out of range.
///
/// - `instruction_get_address(instr)` in `instruction.h`: Get the address of the instruction
///   `instr`.
uint16_t translate_label(
    AssemblerRef assembler,
    InstructionRef instr,
    size_t operand_idx,
    unsigned bits
) {
    // Replace the following 5 lines with your implementation.
    (void) assembler;
    (void) instr;
    (void) operand_idx;
    (void) bits;
    return (uint16_t) -1;
}

/// Translates a regular instruction to its corresponding 16-bit binary representation. The `instr`
/// parameter represents the instruction to be translated.
///
/// You need to use the small functions you implemented above to complete this function. We have
/// provided the implementation for the `ADDOp` and `ANDOp` instructions. You need to complete the
/// implementation for other instructions. You may need to read page 656 of the textbook to
/// understand the format of each instruction.
uint16_t translate_regular_instruction(AssemblerRef assembler, InstructionRef instr) {
    uint16_t result = translate_opcode(instruction_get_opcode(instr)) << 12;

    switch (instruction_get_opcode(instr)) {
    case ADDOp:
    case ANDOp:
        // The first 2 operands of `ADD` and `AND` instructions are register operands.
        result |= translate_register(instr, /*operand_idx=*/0, /*position=*/9);
        result |= translate_register(instr, /*operand_idx=*/1, /*position=*/6);
        // The 3rd operand can be either an immediate or a register.
        if (operand_get_type(instruction_get_operand(instr, 2)) == ImmediateOperand) {
            // For an immediate operand, the 5th bit needs to be set to 1.
            result |= 1u << 5;
            // Extract the lower 5 bits of the immediate and append them to the end of `result`.
            result |= translate_immediate(instr, 2, 5);
        } else {
            result |= translate_register(instr, 2, 0);
        }
        break;

        // Insert your code here.

    default:
        break;
    }

    return result;
}

/// Translates a pseudo-instruction to its corresponding binary representation. The `instr`
/// parameter represents the instruction to be translated.
///
/// Since a pseudo-instruction may be translated into multiple binary instructions (multiple
/// `uint16_t` values), we need to return a `uint16_t` array. You need to call the
/// `create_translate_result_array()` (declared in `assembler.h`) function to create this array and
/// store the translation result in it. You do not need to be responsible for freeing this array.
/// You need to store the length of the result in the `result_length` parameter.
///
/// We have implemented the translation of the `.FILL` instruction for you. You need to complete the
/// translation of the remaining 4 pseudo-instructions.
///
/// You may use the following functions to complete this task:
///
/// - `create_translate_result_array(count)` in `assembler.h`: Create an array of `uint16_t` values
///   with the specified `count`. You need to store the translation result in this array.
///
/// - `operand_get_immediate_value(operand)` in `operand.h`: Get the value of the immediate
///   represented by the operand `operand`. This function is used to get the operand of the `.ORIG`
///   and `.FILL` instructions.
///
/// - `operand_get_regular_decimal(operand)` in `operand.h`: Get the value of the decimal number
///   represented by the operand `operand`. This function is used to get the operand of the `.BLKW`
///   instruction.
///
/// - `operand_get_string_literal(operand, &length)` in `operand.h`: Get the string literal
///   represented by the operand `operand`. You need to read the documentation of this function to
///   understand how to use it. You need to use this function to get the value of the operand when
///   processing the `.STRINGZ` instruction.
uint16_t* translate_pseudo(InstructionRef instr, size_t* result_length) {
    // The result of the translation.
    uint16_t* results = NULL;
    // The number of results.
    *result_length = 0;

    switch (instruction_get_opcode(instr)) {
    case FILLOp:
        // `.FILL` will fill the memory location with the value of the operand.
        results = create_translate_result_array(1);
        results[0] = (uint16_t) operand_get_immediate_value(instruction_get_operand(instr, 0));
        *result_length = 1;
        break;

        // Insert your code here.

    default:
        break;
    }

    return results;
}
