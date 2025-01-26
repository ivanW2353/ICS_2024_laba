#ifndef ASSEMBLER_TOKEN_HPP
#define ASSEMBLER_TOKEN_HPP

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <tuple>

/// Represents a token in LC-3 assembly code, which consists of one or more consecutive characters
/// from the source code.
///
/// In our assembler, we break the source code into a series of tokens and parse the code on a
/// per-token basis. This allows us to ignore spaces and comments. For example, the following code:
///
///   LOOP AND R3, R3, #0,  ; Clear R3
///
/// will be broken down into the following tokens:
///
///   + `LOOP` as a `Label` token.
///   + `AND` as an `Opcode` token.
///   + `R3` as a `Register` token.
///   + `,` as a `Comma` token.
///   + `R3` as a `Register` token.
///   + `,` as a `Comma` token.
///   + `#0` as an `Immediate` token.
///   + `,` as a `Comma` token.
///   + An additional `EOL` token indicating the end of the line.
///
/// During this process, all spaces and comments are ignored. We can easily detect that there is an
/// extra comma after `#0`, allowing us to report this as a syntax error.
///
/// The `Token` class is not responsible for parsing the source code. The `Parser` is used to break
/// the source code into a sequence of `Token` objects.
class Token {
public:
    /// Represents the kinds of tokens that appear in LC-3.
    enum TokenKind : std::uint8_t {
        /// Unknown token type. For tokens in the source code that are unrecognized or erroneous, we
        /// set them as `Unknown`. For example:
        ///
        ///   + `3D5` is neither a valid label nor a valid immediate value, so it will be marked as
        ///     `Unknown`.
        ///   + The character `?` should not appear outside of comments, so it will also be marked
        ///     as `Unknown`.
        Unknown,
        /// Marks the end of a line. When we encounter a newline character '\n', we produce an `EOL`
        /// token.
        EOL,
        /// Marks the end of the file. When the parser reaches the end of the source code, it will
        /// always return an `EOF` token.
        End,

        // The following token types represent the various components of an LC-3 assembly
        // instruction. The format of an LC-3 instruction is:
        //
        //   Label(optional) Opcode OperandList   ; Comment(optional)
        //
        // For assembler directives (pseudo-instructions), the format is:
        //
        //   Label(optional) Pseudo OperandList   ; Comment(optional)
        //
        // This allows us to uniformly parse instructions and pseudo-instructions.

        /// The opcode of an LC-3 instruction, such as `ADD`, `AND`, `BR`, etc. You can find all the
        /// opcodes in the book on page 656.
        Opcode,
        /// A label identifier, which can be any valid identifier except for `Opcode` and
        /// `Register`, such as `LOOP`, `AGAIN`, etc.
        ///
        /// Note that in LC-3, a label must consist of 1 to 20 alphanumeric characters, and the
        /// first character must be a letter. You can find more details about labels on page 234 of
        /// the book.
        Label,
        /// Represents a register, such as `R0`, `R1`, `R2`, etc. Note that LC-3 defines 8
        /// general-purpose registers, so names beyond this range will be parsed as `Label` rather
        /// than `Register`. For example, `R8` is a `Label` rather than a `Register`.
        Register,
        /// The opcode of a pseudo-instruction, such as `.ORIG`, `.FILL`, `.STRINGZ`, etc. You can
        /// find more details about pseudo-instructions on page 236 of the book.
        Pseudo,
        /// Represents an immediate value. We use the `Immediate` type for prefixed numbers, as they
        /// are commonly used as operands. For example, `#-5`, `x3000`, `b101`, etc.
        ///
        /// LC-3 supports 3 different immediate value prefixes: `#` for decimal, `x` for
        /// hexadecimal, and `b` for binary. Note that these numbers can be optionally negative.
        Immediate,
        /// Represents a regular decimal number without a prefix, used as an operand for the `.BLKW`
        /// pseudo-instruction.
        Number,
        /// Represents a string literal, such as `"Hello"`. String literals should be enclosed in
        /// double quotes. They are used as operands for the `.STRINGZ` pseudo-instruction.
        String,

        // The following token types represent symbols that appear in LC-3 assembly code.

        /// A comma `,`, used to separate operands in an instruction.
        Comma,
    };

    Token() : kind_(Unknown), begin_(nullptr), end_(nullptr) { }
    Token(TokenKind kind, char const* begin, char const* end) :
        kind_(kind), begin_(begin), end_(end) { }

    auto kind() const -> TokenKind {
        return kind_;
    }

    auto begin() const -> char const* {
        return begin_;
    }

    auto end() const -> char const* {
        return end_;
    }

    auto content() const -> std::string {
        return { begin_, end_ };
    }

    /// Returns the content of the token, formatted for display in diagnostic messages. For example,
    /// '\n' will be displayed as `\\n`, '\t' as `\\t`, etc.
    auto display_content() const -> std::string;

    auto empty() const -> bool {
        return begin_ == end_;
    }

    auto size() const -> std::size_t {
        return static_cast<std::size_t>(end_ - begin_);
    }

    auto front() const -> char {
        return *begin_;
    }

    auto back() const -> char {
        return *(end_ - 1);
    }

    auto operator[](std::size_t index) const -> char {
        return begin_[index];
    }

    /// Returns whether two `Token` objects are equal.
    ///
    /// Two `Token` objects are considered equal if and only if they have the same kind and refer to
    /// the same range in the source code. Note that even if the content of the source code
    /// corresponding to two `Token` objects is identical, it does not necessarily mean they refer
    /// to the same range, as the same code may appear in different locations.
    friend auto operator==(Token const& lhs, Token const& rhs) -> bool {
        return std::tie(lhs.kind_, lhs.begin_, lhs.end_)
            == std::tie(rhs.kind_, rhs.begin_, rhs.end_);
    }

    friend auto operator!=(Token const& lhs, Token const& rhs) -> bool {
        return !(lhs == rhs);
    }

private:
    /// The type of the token.
    TokenKind kind_;
    /// Points to the beginning of the token text in the source code, while `end_` points to the
    /// position right after the end of the token. Note that `Token` does not own the text.
    char const* begin_;
    char const* end_;
};

/// Overloads the `operator<<` on output streams, allowing us to print a `TokenKind`, so that we can
/// print the kind of a `Token` in the diagnostic message.
auto operator<<(std::ostream& out, Token::TokenKind kind) -> std::ostream&;

/// Overloads the `operator<<` on output streams, allowing us to print a `Token`. This is useful for
/// observing the contents of a `Token` during testing.
inline auto operator<<(std::ostream& out, Token const& token) -> std::ostream& {
    return out << "Token { " << token.kind() << ", '" << token.display_content() << "' }";
}

#endif  // ASSEMBLER_TOKEN_HPP
