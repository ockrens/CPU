#include <iostream>
#include <iomanip>
#include <bitset>
#include "cpu_config.hpp"

int main() {
    CpuConfig config;
    if (!config.loadFromFile("cpu_config.json")) {
        return 1;
    }

    // 1. Binnengekomen 16-bit instructie (bijv. uit de assembler: ADD R2, R3)
    uint16_t incoming_instruction = 0x0351;

    // 2. Decodeer welke instructie dit is op basis van match_mask & base_value
    const Instruction* inst = config.decodeInstruction(incoming_instruction);
    if (!inst) {
        std::cerr << "Ongeldige of onbekende machinetaal: 0x" 
                  << std::hex << incoming_instruction << "\n";
        return 1;
    }

    // 3. Haal de operanden (velden) uit de 16 bits met behulp van de bitmasks
    uint16_t opcode   = config.extractField(incoming_instruction, "opcode");
    uint16_t reg_dest = config.extractField(incoming_instruction, "reg_dest");
    uint16_t reg_src  = config.extractField(incoming_instruction, "reg_src");

    // 4. Print de gedecodeerde data en EEPROM-woorden
    std::cout << "=== INSTRUCTION DECODER ===\n";
    std::cout << "Input (Hex)  : 0x" << std::right << std::hex << std::setw(4) << std::setfill('0') << incoming_instruction
            << std::dec << std::setfill(' ') << "\n";
    std::cout << "Input (Bin)  : " << std::bitset<16>(incoming_instruction) << "\n\n";

    std::cout << "Herkend als  : " << inst->name << " R" << reg_dest << ", R" << reg_src << "\n";
    std::cout << "  - opcode   : " << opcode << "\n";
    std::cout << "  - reg_dest : R" << reg_dest << "\n";
    std::cout << "  - reg_src  : R" << reg_src << "\n\n";

    std::cout << "Control instellingen:\n";
    for (const auto& [key, val] : inst->control) {
        std::cout << "  - " << std::left << std::setw(15) << std::setfill(' ') << key << ": " << val << "\n";
    }

    std::cout << "\nEEPROM Uitgangen:\n" << std::right << std::setfill('0');
    std::cout << "  - EEPROM 1 : 0x" << std::hex << std::setw(2) << static_cast<int>(inst->eeprom1_word)
            << " (bin: " << std::bitset<8>(inst->eeprom1_word) << ")\n";
    std::cout << "  - EEPROM 2 : 0x" << std::hex << std::setw(2) << static_cast<int>(inst->eeprom2_word)
            << " (bin: " << std::bitset<8>(inst->eeprom2_word) << ")"
          << std::dec << std::setfill(' ') << "\n";

    return 0;
}