#ifndef ASSEMBLER_C_TOKEN_H
#define ASSEMBLER_C_TOKEN_H

#include "assembler-c/extern_c.h"

#include <stddef.h>

EXTERN_C_BEGIN

/// Represents the kinds of tokens that appear in LC-3.
typedef enum {
    /// Unknown token type. For tokens in the source code that are unrecognized or erroneous, we set
    /// them as `UnknownToken`. For example:
    ///
    ///   + `3D5` is neither a valid label nor a valid immediate value, so it will be marked as
    ///     `UnknownToken`.
    ///   + The character `?` should not appear outside of comments, so it will also be marked as
    ///     `UnknownToken`.
    UnknownToken,
    /// Marks the end of a line. When we encounter a newline character '\n', we produce an
    /// `EOLToken` token.
    EOLToken,
    /// Marks the end of the file. When the parser reaches the end of the source code, it will
    /// always return an `EndToken` token.
    EndToken,

    // The following token types represent the various components of an LC-3 assembly instruction.
    // The format of an LC-3 instruction is:
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
    OpcodeToken,
    /// A label identifier, which can be any valid identifier except for `OpcodeToken` and
    /// `RegisterToken`, such as `LOOP`, `AGAIN`, etc.
    ///
    /// Note that in LC-3, a label must consist of 1 to 20 alphanumeric characters, and the first
    /// character must be a letter. You can find more details about labels on page 234 of the book.
    LabelToken,
    /// Represents a register, such as `R0`, `R1`, `R2`, etc. Note that LC-3 defines 8
    /// general-purpose registers, so names beyond this range will be parsed as `LabelToken` rather
    /// than `RegisterToken`. For example, `R8` is a `LabelToken` rather than a `RegisterToken`.
    RegisterToken,
    /// The opcode of a pseudo-instruction, such as `.ORIG`, `.FILL`, `.STRINGZ`, etc. You can find
    /// more details about pseudo-instructions on page 236 of the book.
    PseudoToken,
    /// Represents an immediate value. We use the `ImmediateToken` type for prefixed numbers, as
    /// they are commonly used as operands. For example, `#-5`, `x3000`, `b101`, etc.
    ///
    /// LC-3 supports 3 different immediate value prefixes: `#` for decimal, `x` for hexadecimal,
    /// and `b` for binary. Note that these numbers can be optionally negative.
    ImmediateToken,
    /// Represents a regular decimal number without a prefix, used as an operand for the `.BLKW`
    /// pseudo-instruction.
    NumberToken,
    /// Represents a string literal, such as `"Hello"`. String literals should be enclosed in double
    /// quotes. They are used as operands for the `.STRINGZ` pseudo-instruction.
    StringToken,

    // The following token types represent symbols that appear in LC-3 assembly code.

    /// A comma `,`, used to separate operands in an instruction.
    CommaToken,
} TokenKind;

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
typedef struct OpaqueToken const* TokenRef;

/// Returns the kind of the token `token`.
TokenKind token_get_kind(TokenRef token);
/// Returns a pointer to the first character of the code snippet referred to by `token`. For
/// example, if `token` represents the word `Hello` in the sentence `Hello, world!`, this function
/// will return a pointer to the character `H`.
char const* token_get_begin(TokenRef token);
/// Returns a pointer to the character immediately following the last character of the code snippet
/// referred to by `token`. For example, if `token` represents the word `Hello` in the sentence
/// `Hello, world!`, this function will return a pointer to the character `,`.
char const* token_get_end(TokenRef token);
/// Returns whether the content referred to by `token` is empty. Equivalent to returning
/// `token_get_begin(token) == token_get_end(token)`.
int token_is_empty(TokenRef token);
/// Returns the size of the content referred to by `token`. Equivalent to returning
/// `token_get_end(token) - token_get_begin(token)`.
size_t token_get_size(TokenRef token);
/// Returns the first character of the content referred to by `token`. Equivalent to returning
/// `*token_get_begin(token)`. Note that if `token` is empty (i.e. `token_is_empty()` returns
/// `true`), this function will cause undefined behavior.
char token_get_front(TokenRef token);
/// Returns the last character of the content referred to by `token`. Equivalent to returning
/// `*(token_get_end(token) - 1)`. Note that if `token` is empty (i.e. `token_is_empty()` returns
/// `true`), this function will cause undefined behavior.
char token_get_back(TokenRef token);
/// Returns the character at the specified `index` in the content referred to by `token`. Note that
/// if `index` is greater than or equal to `token_get_size(token)`, this function will cause
/// undefined behavior.
char token_get_char(TokenRef token, size_t index);

EXTERN_C_END

#endif  // ASSEMBLER_C_TOKEN_H
