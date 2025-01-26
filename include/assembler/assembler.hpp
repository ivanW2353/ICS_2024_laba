#ifndef ASSEMBLER_ASSEMBLER_HPP
#define ASSEMBLER_ASSEMBLER_HPP

#include "assembler/instruction.hpp"
#include "assembler/operand.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef USE_CPP
    #include <cstddef>
#endif

class Assembler {
public:
    explicit Assembler(std::vector<Instruction> instructions) :
        instructions_(std::move(instructions)) { }

    auto get_instructions() const -> std::vector<Instruction> const& {
        return instructions_;
    }

    auto get_instructions() -> std::vector<Instruction>& {
        return instructions_;
    }

    /// Adds the label `label` and its corresponding address `address` to the symbol table. If the
    /// label already exists in the symbol table, returns `false`; otherwise, returns `true`.
    auto add_label(std::string label, std::uint16_t address) -> bool {
        return symbol_table_.emplace(std::move(label), address).second;
    }

    /// Returns the address of the label `label`. If the label does not exist, sets `ok` to `false`.
    auto get_label(std::string const& label, bool* ok) -> std::uint16_t {
        auto const iter = symbol_table_.find(label);
        if (iter == symbol_table_.end()) {
            *ok = false;
            return 0;
        } else {
            *ok = true;
            return iter->second;
        }
    }

    /// Assigns an address to each instruction.
    void assign_addresses();

    /// Returns the starting address of the instruction sequence.
    auto start_address() const -> std::uint16_t {
        return instructions_.front().get_address();
    }

    /// Scans the instruction sequence and adds any labels found, along with their corresponding
    /// addresses, to the symbol table. If an error occurs during the scan, returns `false`.
    auto scan_label() -> bool;

    /// Translates the instruction into binary form. Note that a single instruction does not
    /// necessarily correspond to exactly one `std::uint16_t` value. For example, `.STRINGZ` may
    /// generate multiple values.
    auto translate() const -> std::vector<std::uint16_t>;

    /// Assembles the instructions and returns the binary representation of the program.
    ///
    /// This method will
    ///
    ///     1. Check the validity of the instructions.
    ///     2. Assign addresses to the instructions.
    ///     3. Scan the instructions for labels and compute the address of each label.
    ///     4. Translate the instructions into binary form.
    ///
    /// If an error occurs during any of these steps, the method will return an empty vector.
    auto run() -> std::vector<std::uint16_t>;

    /// Emits diagnostic information for a redefined label `label`. We make it a `public` interface
    /// for students to use.
    static void emit_label_redefinition_diag(Instruction const& instr) {
        std::cout << "error: label `" << instr.get_label() << "` redefined by instruction `"
                  << instr << "`\n";
    }

    /// Emits diagnostic information for a label not found in an instruction. We make it a `public`
    /// interface for students to use.
    static void emit_label_not_found_diag(Operand const& label_operand, Instruction const& instr) {
        std::cout << "error: label `" << label_operand.label() << "` in instruction `" << instr
                  << "` not found\n";
    }

    /// Emits diagnostic information for an offset of a label in an instruction that is out of
    /// range. We make it a `public` interface for students to use.
    static void emit_label_offset_out_of_range_diag(
        Operand const& label_operand,
        Instruction const& instr,
        std::int16_t offset
    ) {
        std::cout << "error: offset " << offset << " of label `" << label_operand.label()
                  << "` in instruction `" << instr << "` is out of range\n";
    }

private:
    /// The instructions to be assembled.
    std::vector<Instruction> instructions_;
    /// The symbol table that maps labels to their addresses.
    std::unordered_map<std::string, std::uint16_t> symbol_table_;

    // The following member functions are used to translate the parts of an instruction into binary
    // form.
    //
    // Many of these methods are defined as `static` because they do not need any knowledge owned by
    // `Assembler`. Therefore, it would be better to define them in assembler.cpp and make them have
    // internal linkage.
    //
    // However, `translate_label` needs to access the symbol table, so it cannot be `static`.
    // Therefore, we define all these methods as member functions of the class to keep them
    // consistent.

#ifdef USE_CPP
    /// Translates the opcode `opcode` into its binary form.
    static auto translate_opcode(Instruction::Opcode opcode) -> std::uint16_t;

    /// Translates the register operand `reg_operand` into its binary form. The register operand
    /// will be placed at the `position` bit.
    static auto translate_register(Operand const& reg_operand, unsigned position) -> std::uint16_t;

    /// Translates the immediate operand `imm_operand` into its binary form, keeping only the lower
    /// `bits` bits.
    ///
    /// Note that this function does not check whether the representation of `imm_operand` exceeds
    /// `bits` because `Instruction::validate_and_emit_diagnostics()` has already verified that each
    /// immediate operand is within the valid range.
    static auto translate_immediate(Operand const& imm_operand, unsigned bits) -> std::uint16_t;

    /// Converts the operand at index `operand_idx` of the instruction `instr` into its binary form.
    /// This operand must be a label. The function will look up the symbol table to obtain the
    /// address of the label and compute the actual offset based on the label's address and the
    /// instruction's address. Finally, it returns the lower `bits` bits of the offset.
    ///
    /// If the label specified by the operand does not exist or the offset exceeds the range of
    /// `bits`, this function will emit diagnostic information.
    auto translate_label(  //
        Instruction const& instr,
        std::size_t operand_idx,
        unsigned bits
    ) const -> std::uint16_t;
#endif

    /// Translates a pseudo-instruction `instr`. The result will be appended to the end of
    /// `results`.
    ///
    /// Since some pseudo-instructions may generate multiple results, we pass `results` directly as
    /// a parameter.
    static void translate_pseudo(Instruction const& instr, std::vector<std::uint16_t>& results);

    /// Translates a regular instruction (any instruction that is not a pseudo-instruction) into a
    /// 16-bit binary representation.
    auto translate_regular_instruction(Instruction const& instr) const -> std::uint16_t;
};

#endif  // ASSEMBLER_ASSEMBLER_HPP
