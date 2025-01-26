#include "assembler/parser.hpp"
#include "assembler/token.hpp"

#include "gtest/gtest.h"

#include <string>

namespace {
/// Helper function for constructing a token in unit tests, which avoids writing `Token(kind,
/// code.data() + beg, code.data() + end)`, which would result in long lines of code.
auto construct_token(
    Token::TokenKind kind,
    std::string const& code,
    std::size_t beg,
    std::size_t end
) -> Token {
    return Token(kind, code.data() + beg, code.data() + end);
}
}  // namespace

TEST(ParserTest, TokenEOLAndEOF) {
    {
        // For empty text, always return `Token::End`.
        std::string const code;
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 0, 0));
        // Ensure that when `parser` reaches the end of the code, it always returns `Token::End`.
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 0, 0));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 0, 0));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 0, 0));
    }

    {
        // Whitespace in the code is ignored. Ensure the parser handles this and points to the
        // correct code range.
        std::string const code = " ";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 1, 1));
        // Ensure that when `parser` reaches the end of the code, it always returns `Token::End`.
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 1, 1));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 1, 1));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 1, 1));
    }

    {
        // Comments in the code are ignored, which means we always produce `Token::End`.
        std::string const code = "; Hello world!";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, code.size(), code.size()));
        // Ensure that when `parser` reaches the end of the code, it always returns `Token::End`.
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, code.size(), code.size()));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, code.size(), code.size()));
    }

    {
        // When encountering '\n', `Token::EOL` should be produced first.
        std::string const code = "\n";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 0, 1));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 1, 1));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 1, 1));
    }

    {
        // Verify the parser produces the correct number and positions of `Token::EOL`.
        std::string const code = "\n\n \n \n ";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 0, 1));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 1, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 3, 4));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 5, 6));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 7, 7));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 7, 7));
    }

    {
        // Ensure the parser correctly handles spaces and comments in the code.
        std::string const code = " \n ; Hello world!\n ";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 1, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 17, 18));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 19, 19));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 19, 19));
    }

    {
        // Check if the parser is affected by other characters.
        std::string const code = "#3\nabc\n";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 0, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 2, 3));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 3, 6));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 6, 7));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 7, 7));
    }
}

TEST(ParserTest, TokenOpcode) {
    {
        // Check if the parser can correctly handle all opcode tokens.
        std::string const code =
            "ADD AND BR BRn BRz BRp BRzp BRnp BRnz BRnzp JMP JSR JSRR LD LDI LDR LEA NOT RET RTI "
            "ST STI STR TRAP GETC OUT PUTS IN PUTSP HALT\n";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 0, 3));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 4, 7));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 8, 10));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 11, 14));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 15, 18));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 19, 22));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 23, 27));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 28, 32));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 33, 37));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 38, 43));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 44, 47));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 48, 51));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 52, 56));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 57, 59));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 60, 63));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 64, 67));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 68, 71));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 72, 75));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 76, 79));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 80, 83));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 84, 86));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 87, 90));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 91, 94));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 95, 99));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 100, 104));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 105, 108));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 109, 113));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 114, 116));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 117, 122));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 123, 127));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 127, 128));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 128, 128));
    }

    {
        // Check if the parser maintains case sensitivity for opcode tokens.
        std::string const code = "add ADD Add aDd\n";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 0, 3));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 4, 7));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 8, 11));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 12, 15));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 15, 16));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 16, 16));
    }
}

TEST(ParserTest, TokenPseudo) {
    {
        // Check if the parser can correctly handle all pseudo-instructions.
        std::string const code = ".ORIG .FILL .BLKW .STRINGZ .END\n";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Pseudo, code, 0, 5));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Pseudo, code, 6, 11));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Pseudo, code, 12, 17));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Pseudo, code, 18, 26));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Pseudo, code, 27, 31));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 31, 32));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 32, 32));
    }

    {
        // For tokens starting with '.', they are always parsed as pseudo-instructions, even if they
        // are invalid. For tokens not starting with '.', they will never be parsed as
        // pseudo-instructions.
        std::string const code = ".orig .ORIG .OrIg .oRiG .APPLE .ABCD ORIG FILL\n";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Unknown, code, 0, 5));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Pseudo, code, 6, 11));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Unknown, code, 12, 17));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Unknown, code, 18, 23));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Unknown, code, 24, 30));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Unknown, code, 31, 36));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 37, 41));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 42, 46));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 46, 47));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 47, 47));
    }
}

TEST(ParserTest, TokenRegister) {
    {
        // Check if the parser can correctly handle all register identifiers.
        std::string const code = "R0 R1 R2 R3\nR4 R5 R6 R7";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Register, code, 0, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Register, code, 3, 5));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Register, code, 6, 8));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Register, code, 9, 11));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 11, 12));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Register, code, 12, 14));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Register, code, 15, 17));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Register, code, 18, 20));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Register, code, 21, 23));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 23, 23));
    }

    {
        // Check if it can correctly handle identifiers that resemble register names but are not
        // valid register names.
        std::string const code = "R8 R9 R10 RA RN R0R1";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 0, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 3, 5));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 6, 9));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 10, 12));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 13, 15));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 16, 20));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 20, 20));
    }
}

/// Check if the parser can correctly handle different types of whitespace characters.
TEST(ParserTest, TokenWhitespace) {
    std::string const code = " \t\f\v\n\r\n ";
    Parser parser(code);

    EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 4, 5));
    EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 6, 7));
    EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 8, 8));
}

/// Check if the parser can correctly handle identifiers.
TEST(ParserTest, TokenIdentifier) {
    std::string const code = "a  a1  a12  a1a2 ZZ 0Z";
    Parser parser(code);

    EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 0, 1));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 3, 5));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 7, 10));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 12, 16));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 17, 19));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 20, 21));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 21, 22));
    EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 22, 22));
}

/// Check if the parser can correctly handle string literals.
TEST(ParserTest, TokenStringLiteral) {
    {
        // Regular string literal.
        std::string const code = "\"Hello, world!\"";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::String, code, 0, 15));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 15, 15));
    }

    {
        // Empty string literal.
        std::string const code = "\"\"";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::String, code, 0, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 2, 2));
    }

    {
        // The parser should stop at the end of the line even if the string literal is missing a
        // closing quote.
        std::string const code = "\"Hello, world!\nADD";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::String, code, 0, 14));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 14, 15));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 15, 18));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 18, 18));
    }

    {
        // Double quotes should be correctly paired.
        std::string const code = "\"\"\"";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::String, code, 0, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::String, code, 2, 3));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 3, 3));
    }
}

/// Check if the parser can correctly handle immediates and numbers.
TEST(ParserTest, TokenImmediateAndNumber) {
    {
        // Decimal immediate.
        std::string const code = "#1234 #0 #+123 #-123";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 0, 5));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 6, 8));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 9, 14));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 15, 20));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 20, 20));
    }

    {
        // Decimal immediate must only contain valid characters.
        std::string const code = "#+ #- # ## #+a #+12a";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 0, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 3, 5));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 6, 7));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 8, 9));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 9, 10));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 11, 13));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 13, 14));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 15, 19));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 19, 20));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 20, 20));
    }

    {
        // Hexadecimal immediate.
        std::string const code = "x1234 x0c xaf xAd23F";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 0, 5));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 6, 9));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 10, 13));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 14, 20));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 20, 20));
    }

    {
        // Check if invalid hexadecimal characters are parsed as `Token::Label`.
        std::string const code = "x1g x1G x1x xx x x+ X1234";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 0, 3));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 4, 7));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 8, 11));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 12, 14));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 15, 16));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 17, 18));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 18, 19));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 20, 25));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 25, 25));
    }

    {
        // Binary immediate.
        std::string const code = "b101 b0 b1 b101010";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 0, 4));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 5, 7));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 8, 10));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 11, 18));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 18, 18));
    }

    {
        // Check if invalid binary characters are parsed as `Token::Label`.
        std::string const code = "b2 ba b bb b+ b- B101";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 0, 2));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 3, 5));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 6, 7));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 8, 10));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 11, 12));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 12, 13));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 14, 15));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 15, 16));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Label, code, 17, 21));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 21, 21));
    }

    {
        // Parse decimal integers without prefix.
        std::string const code = "1234 0 +123 -123 + - 1+234";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 0, 4));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 5, 6));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 7, 11));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 12, 16));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 17, 18));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 19, 20));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 21, 22));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Number, code, 22, 26));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 26, 26));
    }
}

TEST(ParserTest, TokenComment) {
    {
        // Check if the parser can correctly handle comments.
        std::string const code = "ADD #1234 ; AND #2\n";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 0, 3));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 4, 9));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 18, 19));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 19, 19));
    }

    {
        // Ensure comments only extend to the end of the line.
        std::string const code = "ADD #1234 ; ADD R2\nADD #1234";
        Parser parser(code);

        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 0, 3));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 4, 9));
        EXPECT_EQ(parser.next_token(), construct_token(Token::EOL, code, 18, 19));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Opcode, code, 19, 22));
        EXPECT_EQ(parser.next_token(), construct_token(Token::Immediate, code, 23, 28));
        EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 28, 28));
    }
}

TEST(ParserTest, TokenSymbol) {
    std::string const code = ",,?!;";
    Parser parser(code);

    EXPECT_EQ(parser.next_token(), construct_token(Token::Comma, code, 0, 1));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Comma, code, 1, 2));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Unknown, code, 2, 3));
    EXPECT_EQ(parser.next_token(), construct_token(Token::Unknown, code, 3, 4));
    EXPECT_EQ(parser.next_token(), construct_token(Token::End, code, 5, 5));
}

/// Use some complex code snippets to test if the parser can correctly handle mixed types of tokens.
TEST(ParserTest, TokenMixed) {
    // An example from the book.
    std::string const code = R"(
;
; Program to multiply an integer by the constant 6.
; Before execution, an integer must be stored in NUMBER.
;
        .ORIG x3000
        LD      R1, SIX
        LD      R2, NUMBER
        AND     R3, R3, #0      ; Clear R3. It will
                                ; contain the product.
; The inner loop
;
AGAIN   ADD     R3, R3, R2
        ADD     R1, R1, #-1     ; R1 keeps track of
        BRp     AGAIN           ; the iterations
;
        HALT
;
NUMBER  .BLKW   1
SIX     .FILL   x0006
;
        .END)";
    Parser parser(code);

    Token::TokenKind const expected[] = {
        Token::EOL,      Token::EOL,       Token::EOL,      Token::EOL,       Token::EOL,
        Token::Pseudo,   Token::Immediate, Token::EOL,      Token::Opcode,    Token::Register,
        Token::Comma,    Token::Label,     Token::EOL,      Token::Opcode,    Token::Register,
        Token::Comma,    Token::Label,     Token::EOL,      Token::Opcode,    Token::Register,
        Token::Comma,    Token::Register,  Token::Comma,    Token::Immediate, Token::EOL,
        Token::EOL,      Token::EOL,       Token::EOL,      Token::Label,     Token::Opcode,
        Token::Register, Token::Comma,     Token::Register, Token::Comma,     Token::Register,
        Token::EOL,      Token::Opcode,    Token::Register, Token::Comma,     Token::Register,
        Token::Comma,    Token::Immediate, Token::EOL,      Token::Opcode,    Token::Label,
        Token::EOL,      Token::EOL,       Token::Opcode,   Token::EOL,       Token::EOL,
        Token::Label,    Token::Pseudo,    Token::Number,   Token::EOL,       Token::Label,
        Token::Pseudo,   Token::Immediate, Token::EOL,      Token::EOL,       Token::Pseudo,
        Token::End,
    };

    for (auto const kind : expected) {
        Token const token = parser.next_token();
        EXPECT_EQ(token.kind(), kind) << "Unexpected token " << token;
    }
}