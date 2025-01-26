#include "assembler/solution.hpp"

#include "assembler/assembler.hpp"
#include "assembler/instruction.hpp"
#include "assembler/operand.hpp"
#include "assembler/parser.hpp"
#include "assembler/token.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <utility>

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
auto parse_decimal_number(char const* current, char const* end) -> char const* {
    if (current == end) {
        return end;
    }

    // Check for optional sign
    if (*current == '+' || *current == '-') {
        ++current;
    }

    // Parse digits
    while (current != end && std::isdigit(*current)) {
        ++current;
    }

    return current;
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
auto parse_string_literal(char const* current, char const* end) -> char const* {
    while (current != end) {
        if (*current == '"') {
            return ++current;
        }
        if (*current == '\n') {
            return current;
        }
        ++current;
    }
    return end;
}

/// Parses an instruction starting from the current token and returns the parsed instruction.
///
/// You can follow the comments in the function body to complete this function. You may use the
/// following functions during the implementation:
///
/// - `Parser::current_token()` in `parser.hpp`: Get the current token.
///
/// - `Parser::next_token()` in `parser.hpp`: Parse the next token and return it as the current
///   token.
///
/// - `Parser::emit_opcode_diag_at_current_token()` in `parser.hpp`: Emit diagnostic information at
///   the current token, reporting that the token cannot be an opcode.
///
/// - `Instruction::set_label(token)` in `instruction.hpp`: Add the content of `token` as a label
///   to instruction `*this`.
///
/// - `Instruction::set_opcode(token)` in `instruction.hpp`: Convert the content of `token` to an
///   opcode and add it to the instruction `*this`.
///
/// - `Token::kind()` in `token.hpp`: Get the kind of the token.
///
/// - You can read the comment at the beginning of `token.hpp` to understand the meaning of each
///   token kind. You will need to check the token kind to determine how to use the token.
auto Parser::parse_instruction() -> Instruction {
    Instruction instr;

    // Check if the current token is of type `Label`. If it is, we add the label to `instr` and move
    // to the next token.
    if (current_token().kind() == Token::Label) {
        instr.set_label(current_token());
        next_token();
    }

    while (current_token().kind() == Token::EOL)
    {
        next_token();
    }
    
    // Now `current_token()` points to the opcode. If the first token was a label, `opcode_token`
    // points to the second token, otherwise it points to the first token.
    Token const& opcode_token = current_token();

    // Check whether `current_token` represents a valid opcode or pseudo-instruction. If it does
    // not, emit a diagnostic message and return an unknown instruction. You can obtain an unknown
    // instruction by returning `{}`.
    if (opcode_token.kind() != Token::Opcode && opcode_token.kind() != Token::Pseudo) {
        emit_opcode_diag_at_current_token();
        return {};
    }

    // Add the opcode to `instr`.
    instr.set_opcode(opcode_token);

    // Move to the next token to continue parsing.
    next_token();

    // Now we need to parse the operand list. The operand list is a sequence of tokens separated by
    // `Token::Comma`.
    return parse_operand_list(std::move(instr));
}

/// Converts the string representation of an immediate to an integer value and sets `*ok` to `false`
/// if overflow occurs.
///
/// `content` is a string that represents a valid integer or immediate value. The string may
/// represent an immediate value with a prefix or a regular decimal integer. Regardless of the
/// presence of a prefix, when it represents a decimal integer, it may have a sign.
///
/// Overflow is defined as exceeding the maximum value of an unsigned 16-bit integer for positive
/// numbers or the minimum value of a signed 16-bit integer for negative numbers. You can use
/// `std::numeric_limits<std::uint16_t>::max()` and `std::numeric_limits<std::int16_t>::min()` to
/// get the maximum and minimum values of unsigned and signed 16-bit integers.
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
auto string_to_integer(std::string const& content, bool* ok) -> std::int16_t {
    try {
        std::size_t pos = 0;
        int base = 10;
        if (content[0] == '#') {
            pos = 1;
        } else if (content[0] == 'x') {
            base = 16;
            pos = 1;
        } else if (content[0] == 'b') {
            base = 2;
            pos = 1;
        }

        long long value = std::stoll(content.substr(pos), nullptr, base);

        if (value > std::numeric_limits<std::uint16_t>::max()
            || value < std::numeric_limits<std::int16_t>::min()) {
            *ok = false;
            return 0;
        }

        *ok = true;
        return static_cast<std::int16_t>(value);
    } catch (...) {
        *ok = false;
        return 0;
    }
}

/// Returns the valid range of an immediate operand in an instruction.
///
/// You need to return the range of the immediate operand via the returned pair, where the first
/// element is the lower bound and the second element is the upper bound. We have provided the
/// ranges for the `TRAP`, `ORIG`, `FILL`, and `BLKW` instructions. You need to fill in the
/// ranges for other instructions.
auto Instruction::immediate_range() const -> std::pair<std::int16_t, std::int16_t> {
    // clang-format off
    switch (opcode_) {
    case TRAP:
        // trapvect8
        return { static_cast<std::int16_t>(0), static_cast<std::int16_t>(255) };

    case ORIG: case FILL: case BLKW:
        // 16-bit integer
        return {
            std::numeric_limits<std::int16_t>::min(),
            std::numeric_limits<std::int16_t>::max(),
        };

    case ADD: case AND:
        // 5-bit signed integer
        return { static_cast<std::int16_t>(-16), static_cast<std::int16_t>(15) };

    case LD: case LDI: case LEA: case ST: case STI: 
    case BR: case BRn: case BRz: case BRp: case BRzp: case BRnp: case BRnz: case BRnzp:
        // 9-bit signed integer
        return { static_cast<std::int16_t>(-256), static_cast<std::int16_t>(255) };

    case LDR: case STR:
        // 6-bit signed integer
        return { static_cast<std::int16_t>(-32), static_cast<std::int16_t>(31) };

    case JSR:
        // 11-bit signed integer
        return { static_cast<std::int16_t>(-1024), static_cast<std::int16_t>(1023) };

    default:
        return {};
    }
    // clang-format on
}

/// Assigns addresses to all instructions.
///
/// We have provided a function framework for you. You need to complete the body. During this
/// process, you may use the following functions:
///
/// - `Assembler::get_instructions()` in `assembler.hpp`: Get the list of instructions.
///
/// - `Instruction::get_opcode()` in `instruction.hpp`: Get the opcode of the instruction `*this`.
///
/// - `Instruction::get_operand(i)` in `instruction.hpp`: Get the `i`-th operand of the instruction
///   `*this`. Note that `i` is 0-based.
///
/// - `Instruction::set_address(address)` in `instruction.hpp`: Set the address of the instruction
///   `*this` to `address`.
///
/// - `Operand::immediate_value()` in `operand.hpp`: Get the value of the immediate represented by
///   the operand `*this`.
///
/// - `Operand::regular_decimal()` in `operand.hpp`: Get the value of the decimal represented by the
///   operand `*this`. You need to use this function to get the value of the operand when processing
///   the `.BLKW` instruction.
///
/// - `Operand::string_literal()` in `operand.hpp`: Get the string literal represented by the
///   operand `*this`. You need to use this function to get the value of the operand when processing
///   the `.STRINGZ` instruction.
void Assembler::assign_addresses() {
    std::uint16_t address = get_instructions().front().get_operand(0).immediate_value();
    for (Instruction& instr : get_instructions()) {
        instr.set_address(address);

        switch (instr.get_opcode()) {
        // case Instruction::ORIG:
        //     address = instr.get_operand(0).immediate_value();
        //     break;
        case Instruction::FILL:
            address += 1;
            break;
        case Instruction::BLKW:
            address += instr.get_operand(0).regular_decimal();
            break;
        case Instruction::STRINGZ:
            address += instr.get_operand(0).string_literal().size() + 1;
            break;
        default:
            address += 1;
            break;
        }
    }
}

/// Scans all labels in the instructions and adds the label and its associated address to the symbol
/// table. If the label already exists in the symbol table, emits a diagnostic message. Returns
/// `true` if no errors occur during this process; otherwise, returns `false`.
///
/// You may use the following functions to complete this task:
///
/// - `Assembler::get_instructions()` in `assembler.hpp`: Get the list of instructions.
///
/// - `Assembler::add_label(label, address)` in `assembler.hpp`: Add the label `label` and its
///   corresponding address `address` to the symbol table. If the label already exists in the
///   symbol table, return `false`; otherwise, return `true`.
///
/// - `Assembler::emit_label_redefinition_diag(instr)` in `assembler.hpp`: Emit diagnostic
///   information for a redefined label attached to the instruction `instr`.
///
/// - `Instruction::has_label()` in `instruction.hpp`: Check if the instruction `*this` has a label.
///
/// - `Instruction::get_label()` in `instruction.hpp`: Get the label of the instruction `*this`.
///
/// - `Instruction::get_address()` in `instruction.hpp`: Get the address of the instruction `*this`.
auto Assembler::scan_label() -> bool {
    for (Instruction const& instr : get_instructions()) {
        if (instr.has_label()) {
            if (!add_label(instr.get_label(), instr.get_address())) {
                emit_label_redefinition_diag(instr);
                return false;
            }
        }
    }
    return true;
}

/// Translates an instruction opcode `opcode` to its corresponding 4-bit binary representation. We
/// have provided part of the implementation for you. You need to complete the rest.
auto Assembler::translate_opcode(Instruction::Opcode opcode) -> std::uint16_t {
    // clang-format off
    switch (opcode) {
    case Instruction::ADD:
        return 1;  // 0001
    case Instruction::AND:
        return 5;  // 0101
    case Instruction::BRn:  case Instruction::BRz:  case Instruction::BRp:  case Instruction::BR:
    case Instruction::BRzp: case Instruction::BRnp: case Instruction::BRnz: case Instruction::BRnzp:
        return 0;  // 0000
    case Instruction::JMP:
        return 12; // 1100
    case Instruction::JSR:
        return 4;  // 0100
    case Instruction::JSRR:
        return 4;  // 0100
    case Instruction::LD:
        return 2;  // 0010
    case Instruction::LDI:
        return 10; // 1010
    case Instruction::LDR:
        return 6;  // 0110
    case Instruction::LEA:
        return 14; // 1110
    case Instruction::NOT:
        return 9;  // 1001
    case Instruction::RET:
        return 12; // 1100
    case Instruction::RTI:
        return 8;  // 1000
    case Instruction::ST:
        return 3;  // 0011
    case Instruction::STI:
        return 11; // 1011
    case Instruction::STR:
        return 7;  // 0111
    case Instruction::TRAP: case Instruction::GETC:  case Instruction::OUT: case Instruction::PUTS:
    case Instruction::IN:   case Instruction::PUTSP: case Instruction::HALT:
        return 15;  // 1111
    default:
        return 13;  // 1101
    }
    // clang-format on
}

/// Translates a register operand `reg_operand` to its corresponding 3-bit binary representation.
/// You can get the ID of the register by calling `Operand::register_id()`. We set the ID of the
/// register to be its index in LC-3, i.e., `R0` corresponds to 0, `R1` corresponds to 1, and so on.
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
auto Assembler::translate_register(Operand const& reg_operand, unsigned position) -> std::uint16_t {
    std::uint16_t reg_id = reg_operand.register_id();
    return reg_id << position;
}

/// Translates an immediate operand `imm_operand` to its corresponding binary representation and
/// truncates it to `bits` bits. You can get the value of the immediate by calling
/// `Operand::immediate_value()`.
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
auto Assembler::translate_immediate(Operand const& imm_operand, unsigned bits) -> std::uint16_t {
    std::int16_t value = imm_operand.immediate_value();
    std::uint16_t mask = (1 << bits) - 1;
    return static_cast<std::uint16_t>(value) & mask;
}

/// Translates a label operand to its corresponding binary representation and truncates it to `bits`
/// bits. The label operand is the `operand_idx`-th operand of the instruction `instr`. You can get
/// this operand by calling `Instruction::get_operand(operand_idx)`, and then get the content of the
/// label by calling `Operand::label()`.
///
/// Similar to `translate_immediate()`, the `bits` parameter specifies the number of bits the label
/// will occupy in the final binary instruction. For example, if `bits` is 9, it means that the
/// binary representation of the label will appear in the lower 9 bits of the final binary
/// instruction. You need to check if the offset you calculate is within the range of `bits` bits.
/// If the offset is out of this range, you need to emit a diagnostic message and return
/// `static_cast<std::uint16_t>(-1)`.
///
/// You may use the following functions to complete this task:
///
/// - `Assembler::emit_label_not_found_diag(label_operand, instr)` in `assembler.hpp`: Emit
///   diagnostic information when the label represented by the label operand `label_operand` in the
///   instruction `instr` is not found in the symbol table.
///
/// - `Assembler::emit_label_offset_out_of_range_diag(label_operand, instr, offset)` in
///   `assembler.hpp`: Emit diagnostic information when the offset `offset` of the label represented
///   by the label operand `label_operand` in the instruction `instr` is out of range.
///
/// - `Instruction::get_address()` in `instruction.hpp`: Get the address of the instruction `*this`.
///
/// - You can access the private member `symbol_table_` in `Assembler` directly, which is a
///   `std::unordered_map`. If you want to look up the address of a label in the symbol table, you
///   can use the `find()` method provided by STL.
auto Assembler::translate_label(  //
    Instruction const& instr,
    std::size_t operand_idx,
    unsigned bits
) const -> std::uint16_t {
    auto const& label_operand = instr.get_operand(operand_idx);
    auto const& label = label_operand.label();
    auto const instr_address = instr.get_address();

    auto const iter = symbol_table_.find(label);
    if (iter == symbol_table_.end()) {
        emit_label_not_found_diag(label_operand, instr);
        return static_cast<std::uint16_t>(-1);
    }

    auto const label_address = iter->second;
    auto const offset = static_cast<std::int16_t>(label_address - instr_address - 1);

    auto const max_offset = (1 << (bits - 1)) - 1;
    auto const min_offset = -(1 << (bits - 1));

    if (offset < min_offset || offset > max_offset) {
        emit_label_offset_out_of_range_diag(label_operand, instr, offset);
        return static_cast<std::uint16_t>(-1);
    }

    return static_cast<std::uint16_t>(offset & ((1 << bits) - 1));
}

/// Translates a regular instruction to its corresponding 16-bit binary representation. The `instr`
/// parameter represents the instruction to be translated.
///
/// You need to use the small functions you implemented above to complete this function. We have
/// provided the implementation for the `ADD` and `AND` instructions. You need to complete the
/// implementation for other instructions. You may need to read page 656 of the textbook to
/// understand the format of each instruction.
auto Assembler::translate_regular_instruction(Instruction const& instr) const -> std::uint16_t {
    std::uint16_t result = translate_opcode(instr.get_opcode()) << 12;

    switch (instr.get_opcode()) {
    case Instruction::ADD:
    case Instruction::AND:
        result |= translate_register(instr.get_operand(0), 9);
        result |= translate_register(instr.get_operand(1), 6);
        if (instr.get_operand(2).type() == Operand::Immediate) {
            result |= 1u << 5;
            result |= translate_immediate(instr.get_operand(2), 5);
        } else {
            result |= translate_register(instr.get_operand(2), 0);
        }
        break;

    case Instruction::BR:
        result |= 0x7 << 9;
        result |= translate_label(instr, 0, 9);
        break;
    case Instruction::BRn:
        result |= 0x4 << 9;
        result |= translate_label(instr, 0, 9);
        break;
    case Instruction::BRz:
        result |= 0x2 << 9;
        result |= (instr.get_operand(0).type() == Operand::Label) ? translate_label(instr, 0, 9) : translate_immediate(instr.get_operand(0), 9);
        break;
    case Instruction::BRp:
        result |= 0x1 << 9;
        result |= translate_label(instr, 0, 9);
        break;
    case Instruction::BRzp:
        result |= 0x3 << 9;
        result |= translate_label(instr, 0, 9);
        break;
    case Instruction::BRnp:
        result |= 0x5 << 9;
        result |= translate_label(instr, 0, 9);
        break;
    case Instruction::BRnz:
        result |= 0x6 << 9;
        result |= translate_label(instr, 0, 9);
        break;
    case Instruction::BRnzp:
        result |= 0x7 << 9;
        result |= translate_label(instr, 0, 9);
        break;

    case Instruction::JMP:
    case Instruction::JSRR:
        result |= translate_register(instr.get_operand(0), 6);
        break;

    case Instruction::JSR:
        result |= 1u << 11;
        result |= translate_label(instr, 0, 11);
        break;

    case Instruction::LD:
    case Instruction::LDI:
    case Instruction::LEA:
        result |= translate_register(instr.get_operand(0), 9);
        result |= translate_label(instr, 1, 9);
        break;

    case Instruction::LDR:
    case Instruction::STR:
        result |= translate_register(instr.get_operand(0), 9);
        result |= translate_register(instr.get_operand(1), 6);
        result |= translate_immediate(instr.get_operand(2), 6);
        break;

    case Instruction::NOT:
        result |= translate_register(instr.get_operand(0), 9);
        result |= translate_register(instr.get_operand(1), 6);
        result |= 0x3F;
        break;

    case Instruction::RET:
        result |= 0x7 << 6;
        break;

    case Instruction::RTI:
        break;

    case Instruction::ST:
    case Instruction::STI:
        result |= translate_register(instr.get_operand(0), 9);
        result |= translate_label(instr, 1, 9);
        break;

    case Instruction::TRAP:
        result |= translate_immediate(instr.get_operand(0), 8);
        break;
    case Instruction::GETC:
        result |= 0x20;
        break;
    case Instruction::OUT:
        result |= 0x21;
        break;
    case Instruction::PUTS:
        result |= 0x22;
        break;
    case Instruction::IN:
        result |= 0x23;
        break;
    case Instruction::PUTSP:
        result |= 0x24;
        break;
    case Instruction::HALT:
        result |= 0x25;
        break;

    default:
        break;
    }

    return result;
}

/// Translates a pseudo-instruction to its corresponding binary representation. The `instr`
/// parameter represents the instruction to be translated. Your translated binary instruction should
/// be added to the **back** of the `results` vector.
///
/// Since a pseudo-instruction may be translated into multiple instructions (multiple
/// `std::uint16_t` results), this function takes an additional `results` parameter. You need to
/// append the new translation result to the end of `results`.
///
/// We have implemented the translation of the `.FILL` instruction for you. You need to complete the
/// translation of the remaining 4 pseudo-instructions.
///
/// You may use the following functions to complete this task:
///
/// - `Operand::immediate_value()` in `operand.hpp`: Get the value of the immediate represented by
///   the operand `*this`. This function is used to get the operand of the `.ORIG` and `.FILL`
///   instructions.
///
/// - `Operand::regular_decimal()` in `operand.hpp`: Get the value of the decimal number represented
///   by the operand `*this`. This function is used to get the operand of the `.BLKW` instruction.
///
/// - `Operand::string_literal()` in `operand.hpp`: Get the string literal represented by the
///   operand `*this`. This function is used to get the oeprand of the `.STRINGZ` instruction.
void Assembler::translate_pseudo(Instruction const& instr, std::vector<std::uint16_t>& results) {
    switch (instr.get_opcode()) {
    case Instruction::FILL:
        // `.FILL` will fill the memory location with the value of the operand.
        results.push_back(static_cast<std::uint16_t>(instr.get_operand(0).immediate_value()));
        break;

    case Instruction::ORIG:
        // `.ORIG` sets the starting address, no binary output needed.
        break;

    case Instruction::BLKW:
        // `.BLKW` reserves a block of memory.
        results.resize(results.size() + instr.get_operand(0).regular_decimal(), 0);
        break;

    case Instruction::STRINGZ:
        // `.STRINGZ` stores a null-terminated string.
        for (char c : instr.get_operand(0).string_literal()) {
            results.push_back(static_cast<std::uint16_t>(c));
        }
        results.push_back(0);  // Null terminator
        break;

    case Instruction::END:
        // `.END` does not produce any binary output.
        break;

    default:
        break;
    }
}
