#include "assembler/operand.hpp"

#include "assembler-c/operand.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <type_traits>

auto operator<<(std::ostream& out, Operand::OperandType operand_type) -> std::ostream& {
    switch (operand_type) {
    case Operand::Register:
        return out << "Register";
    case Operand::Immediate:
        return out << "Immediate";
    case Operand::Number:
        return out << "Number";
    case Operand::Label:
        return out << "Label";
    case Operand::StringLiteral:
        return out << "StringLiteral";
    default:
        return out << "UnknownOperandType";
    }
}

auto operator<<(std::ostream& out, Operand const& operand) -> std::ostream& {
    switch (operand.type()) {
    case Operand::Register:
        return out << 'R' << static_cast<unsigned>(operand.register_id());

    case Operand::Immediate:
        // Always print immediates in decimal form.
        return out << '#' << operand.immediate_value();

    case Operand::Number:
        return out << operand.regular_decimal();

    case Operand::Label:
        return out << operand.label();

    case Operand::StringLiteral:
        return out << '"' << operand.string_literal() << '"';

    default:
        return out << "UnknownOperand";
    }
}

//==================================================================================================
// C bindings for the `Operand` class.
//==================================================================================================

namespace {
auto unwrap(OperandRef operand) -> Operand const& {
    return *reinterpret_cast<Operand const*>(operand);
}
}  // namespace

auto operand_get_type(OperandRef operand) -> OperandType {
    Operand::OperandType const type = unwrap(operand).type();
    return static_cast<OperandType>(
        static_cast<std::underlying_type<Operand::OperandType>::type>(type)
    );
}

auto operand_get_register_id(OperandRef operand) -> std::uint8_t {
    return unwrap(operand).register_id();
}

auto operand_get_immediate_value(OperandRef operand) -> std::int16_t {
    return unwrap(operand).immediate_value();
}

auto operand_get_regular_decimal(OperandRef operand) -> std::int16_t {
    return unwrap(operand).regular_decimal();
}

auto operand_get_label(OperandRef operand, std::size_t* length) -> char const* {
    static_assert(
        std::is_standard_layout<Operand>::value,
        "Operand must be standard layout to support the implementation of this function."
    );

    // This function requires us to get a pointer to the string stored inside `Operand`. We cannot
    // call the `label()` member function and return its `data()` pointer, as the `std::string`
    // returned by `label()` will be destroyed after the function ends.
    //
    // We need to read the value of `string_content_` directly. One approach is to add a `public`
    // interface to get the content of the original `string_content_` member. Another approach is to
    // add this function as a friend of `Operand`.
    //
    // Here we try a trick. We have already determined that `Operand` is a standard layout type, so
    // `Operand` and its first non-static data member are pointer-interconvertible, and `union` and
    // its non-static data members are also pointer-interconvertible. Therefore, we can safely use
    // `reinterpret_cast` to convert a pointer to an `Operand` object to a pointer to
    // `string_content_`.
    auto const string_content =
        *reinterpret_cast<std::array<char const*, 2> const*>(&unwrap(operand));
    *length = string_content[1] - string_content[0];
    return string_content[0];
}

auto operand_get_string_literal(OperandRef operand, std::size_t* length) -> char const* {
    return operand_get_label(operand, length);
}
