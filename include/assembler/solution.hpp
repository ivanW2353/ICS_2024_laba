#ifndef ASSEMBLER_SOLUTION_HPP
#define ASSEMBLER_SOLUTION_HPP

#include <cstdint>
#include <string>

auto parse_decimal_number(char const* current, char const* end) -> char const*;
auto parse_string_literal(char const* current, char const* end) -> char const*;
auto string_to_integer(std::string const& content, bool* ok) -> std::int16_t;

#endif  // ASSEMBLER_SOLUTION_HPP
