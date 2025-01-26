#include "assembler/token.hpp"

#include "assembler-c/token.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <type_traits>

auto Token::display_content() const -> std::string {
    std::string result;
    result.reserve(size());

    for (char const ch : *this) {
        switch (ch) {
        case 0:
            result.append("\\0");
            break;

        case '\n':
            result.append("\\n");
            break;

        case '\t':
            result.append("\\t");
            break;

        default:
            result.push_back(ch);
            break;
        }
    }

    return result;
}

auto operator<<(std::ostream& out, Token::TokenKind kind) -> std::ostream& {
    return out << "Token::" << [&] {
        switch (kind) {
        case Token::EOL:
            return "EOL";
        case Token::End:
            return "End";
        case Token::Opcode:
            return "Opcode";
        case Token::Label:
            return "Label";
        case Token::Register:
            return "Register";
        case Token::Pseudo:
            return "Pseudo";
        case Token::Immediate:
            return "Immediate";
        case Token::Number:
            return "Number";
        case Token::String:
            return "String";
        case Token::Comma:
            return "Comma";
        default:
            return "Unknown";
        }
    }();
}

//==================================================================================================
// C bindings for the `Token` class.
//==================================================================================================

namespace {
auto unwrap(TokenRef token) -> Token const& {
    return *reinterpret_cast<Token const*>(token);
}
}  // namespace

auto token_get_kind(TokenRef token) -> TokenKind {
    Token::TokenKind const kind = unwrap(token).kind();
    return static_cast<TokenKind>(static_cast<std::underlying_type<Token::TokenKind>::type>(kind));
}

auto token_get_begin(TokenRef token) -> char const* {
    return unwrap(token).begin();
}

auto token_get_end(TokenRef token) -> char const* {
    return unwrap(token).end();
}

auto token_is_empty(TokenRef token) -> int {
    return unwrap(token).empty();
}

auto token_get_size(TokenRef token) -> std::size_t {
    return unwrap(token).size();
}

auto token_get_front(TokenRef token) -> char {
    return unwrap(token).front();
}

auto token_get_back(TokenRef token) -> char {
    return unwrap(token).back();
}

auto token_get_char(TokenRef token, std::size_t index) -> char {
    return unwrap(token)[index];
}
