#include "assembler/assembler.hpp"

#include "assembler-c/assembler.h"
#include "assembler-c/instruction.h"
#include "assembler-c/operand.h"
#include "assembler/instruction.hpp"
#include "assembler/operand.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#ifndef USE_CPP
    #include "assembler-c/solution.h"
#endif

auto Assembler::translate() const -> std::vector<std::uint16_t> {
    std::vector<std::uint16_t> results;
    results.reserve(instructions_.size());

    for (Instruction const& instr : instructions_) {
        switch (instr.get_opcode()) {
        case Instruction::ORIG:
        case Instruction::END:
        case Instruction::FILL:
        case Instruction::BLKW:
        case Instruction::STRINGZ:
            translate_pseudo(instr, results);
            break;

        default:
            std::uint16_t const result = translate_regular_instruction(instr);
            if (result == static_cast<std::uint16_t>(-1)) {
                // An error occurred during translation. Return an empty vector.
                return {};
            }
            results.push_back(result);
            break;
        }
    }

    return results;
}

auto Assembler::run() -> std::vector<std::uint16_t> {
    if (std::any_of(instructions_.begin(), instructions_.end(), [](Instruction const& instr) {
            return !instr.validate_and_emit_diagnostics();
        })) {
        // There was an error during validation, so we return an empty vector.
        return {};
    }

    // We need to check whether the final instruction sequence starts with `.ORIG` and whether it
    // contains only one `.ORIG` instruction.
    if (!instructions_.empty() && instructions_.front().get_opcode() != Instruction::ORIG) {
        // If the instruction sequence does not start with `.ORIG`, we need to report an error.
        std::cout << "error: expected the first instruction to be `.ORIG`, but got `"
                  << instructions_.front() << "`\n";

        return {};
    }

    if (std::any_of(
            std::next(instructions_.begin()),
            instructions_.end(),
            [](Instruction const& instr) { return instr.get_opcode() == Instruction::ORIG; }
        )) {
        // If the instruction sequence contains multiple `.ORIG` instructions, we need to report
        // an error.
        std::cout << "error: multiple `.ORIG` pseudo-instructions found\n";
        return {};
    }

    assign_addresses();
    if (!scan_label()) {
        // There was an error during scanning labels, so we return an empty vector.
        return {};
    }

    return translate();
}

//==================================================================================================
// C bindings for the `Assembler` class.
//==================================================================================================

namespace {
auto unwrap(AssemblerRef assembler) -> Assembler& {
    return *reinterpret_cast<Assembler*>(assembler);
}

auto wrap(Instruction const& instr) -> InstructionRef {
    return reinterpret_cast<InstructionRef>(const_cast<Instruction*>(&instr));
}

auto unwrap(InstructionRef instr) -> Instruction& {
    return *reinterpret_cast<Instruction*>(instr);
}

auto unwrap(OperandRef operand) -> Operand const& {
    return *reinterpret_cast<Operand const*>(operand);
}

#ifndef USE_CPP
auto wrap(Assembler const& assembler) -> AssemblerRef {
    return reinterpret_cast<AssemblerRef>(const_cast<Assembler*>(&assembler));
}
#endif
}  // namespace

auto assembler_get_instruction_size(AssemblerRef assembler) -> std::size_t {
    return unwrap(assembler).get_instructions().size();
}

auto assembler_get_instruction(AssemblerRef assembler, std::size_t index) -> InstructionRef {
    return wrap(unwrap(assembler).get_instructions()[index]);
}

auto assembler_add_label(AssemblerRef assembler, char const* label, std::uint16_t address) -> int {
    return unwrap(assembler).add_label(label, address);
}

auto assembler_get_label(  //
    AssemblerRef assembler,
    char const* label,
    std::size_t length,
    int* ok
) -> std::uint16_t {
    bool _ok = false;
    std::uint16_t const address = unwrap(assembler).get_label({ label, length }, &_ok);
    *ok = static_cast<int>(_ok);
    return address;
}

void assembler_emit_label_redefinition_diag(InstructionRef instr) {
    Assembler::emit_label_redefinition_diag(unwrap(instr));
}

void assembler_emit_label_not_found_diag(OperandRef label_operand, InstructionRef instr) {
    Assembler::emit_label_not_found_diag(unwrap(label_operand), unwrap(instr));
}

void assembler_emit_label_offset_out_of_range_diag(
    OperandRef label_operand,
    InstructionRef instr,
    std::int16_t offset
) {
    Assembler::emit_label_offset_out_of_range_diag(unwrap(label_operand), unwrap(instr), offset);
}

auto create_translate_result_array(std::size_t size) -> std::uint16_t* {
    return new std::uint16_t[size];
}

void free_translate_result_array(std::uint16_t* array) {
    delete[] array;
}

#ifndef USE_CPP
void Assembler::assign_addresses() {
    // This function will be implemented by the student.
    ::assign_addresses(wrap(*this));
}

auto Assembler::scan_label() -> bool {
    // This function will be implemented by the student.
    return ::scan_label(wrap(*this));
}

auto Assembler::translate_regular_instruction(Instruction const& instr) const -> std::uint16_t {
    // This function will be implemented by the student.
    return ::translate_regular_instruction(wrap(*this), wrap(instr));
}

void Assembler::translate_pseudo(Instruction const& instr, std::vector<std::uint16_t>& results) {
    size_t result_length = 0;
    // This function will be implemented by the student.
    std::uint16_t* const result = ::translate_pseudo(wrap(instr), &result_length);
    results.insert(results.end(), result, result + result_length);
    free_translate_result_array(result);
}
#endif
