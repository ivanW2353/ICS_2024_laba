#ifndef ASSEMBLER_PARSER_HPP
#define ASSEMBLER_PARSER_HPP

#include "assembler/instruction.hpp"
#include "assembler/token.hpp"

#include <cstring>
#include <ostream>
#include <string>
#include <vector>

class Parser {
public:
    /// Constructs a parser to parse the source code `source`. Note that the `Parser` does not own
    /// `source`, so the user must ensure that `source` remains valid during parsing.
    explicit Parser(std::string const& source) :
        source_begin_(source.data()),
        source_end_(source.data() + source.size()),
        current_(source_begin_) { }

    /// Prevents the user from constructing a `Parser` with a temporary `std::string`, as the string
    /// would be destroyed almost immediately.
    Parser(std::string const&&) = delete;

    /// Produces a token starting from the current position `current_` is pointing to. Moves
    /// `current_` to the end of the token. The generated token is stored in `cur_token_` and
    /// returned by this function.
    auto next_token() -> Token const&;

    auto current_token() const -> Token const& {
        return cur_token_;
    }

    /// Parses a sequence of instructions from the given source code until encountering the `.END`
    /// pseudo-instruction or reaching the end of the code. If an error is encountered during this
    /// process (e.g., encountering an invalid token, using incorrect syntax, etc.), the parser will
    /// provide diagnostic information and return a `vector` containing only one unknown
    /// instruction.
    auto parse_instructions() -> std::vector<Instruction>;

    /// Parses an operand list starting from the current token. The parsed operands are added to the
    /// instruction `instr`. Returns the modified instruction. If an error is encountered during
    /// this process, diagnostic information is emitted and an unknown instruction is returned.
    ///
    /// We make this function a `public` interface so that it can be exposed to C interfaces for
    /// student use.
    auto parse_operand_list(Instruction instr) -> Instruction;

private:
    /// The starting position of the source code.
    char const* source_begin_;
    /// The position just past the end of the source code.
    char const* source_end_;
    /// Points to the character currently being parsed.
    char const* current_;
    /// Stores the current token to simplify the parser's implementation.
    Token cur_token_;

    /// Emits a diagnostic message at the location of the current token (returned by
    /// `current_token()`).
    auto emit_diagnostic_at_current_token() const -> std::ostream&;

public:
    /// This function is used to generate diagnostic messages when an error occurs while setting the
    /// opcode from the current token. We can only construct an opcode from tokens of type
    /// `Token::Opcode` and `Token::Pseudo`. If an incorrect token type is used, we will generate an
    /// error message.
    ///
    /// We make this function a `public` interface so that it can be exposed to C interfaces for
    /// student use.
    void emit_opcode_diag_at_current_token() const;

private:
    /// This function is used to generate diagnostic messages when an error occurs while
    /// constructing the `Operand` from the current token. The error type is provided by `error`,
    /// and the `token` that caused the error is obtained from `current_token()`.
    void emit_operand_diag_at_current_token(OperandConstructionErrorType error) const;

    /// Parses a single instruction starting from the current token (the return value of
    /// `current_token()`). If an error is encountered during parsing, an unknown instruction is
    /// returned (i.e., `Instruction::is_unknown()` returns `true`).
    auto parse_instruction() -> Instruction;
};

#endif  // ASSEMBLER_PARSER_HPP
