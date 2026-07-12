/**
 * @file main.cpp
 * @brief main file of the assembler.
 * 
 * @author R.J. Ockhuijsen
 * @date 2026-01-28
 * @version 0.1
 * 
 * @details
 * This file contains the main function of the assembler, it handles input/output files, line parsing and encoding.
 */
#ifndef MAIN_CPP
#define MAIN_CPP

#include "../include/main.hpp"


void calculatePattern(const std::string& cleanPattern) {
    uint16_t baseValue = 0;
    uint16_t matchMask = 0;

    for (int i = 0; i < 16; i++) {
        int bitPosition = 15 - i;
        char bitChar = cleanPattern[i];

        if (bitChar == '1') {
            baseValue |= (1 << bitPosition);
            matchMask |= (1 << bitPosition);
        } 
        else if (bitChar == '0') {
            // What happens to baseValue and matchMask here?
        }
        // Letters (d, s, i, etc.) are skipped, leaving them as 0
    }
}

InstructionPattern parsePattern(const std::string& rawPattern) {
    InstructionPattern result;
    
    // 🧼 First, remove any spaces from the JSON pattern string
    std::string cleanPattern = "";
    for (char c : rawPattern) {
        if (c != ' ') cleanPattern += c;
    }

    // 🔄 Loop through all 16 bits
    for (int i = 0; i < 16; i++) {
        int bitPosition = 15 - i;
        char bitChar = cleanPattern[i];

        if (bitChar == '1') {
            result.baseValue |= (1 << bitPosition);
            result.matchMask |= (1 << bitPosition);
        } 
        else if (bitChar == '0') {
            // It matters, so mark the mask, but leave baseValue as 0
            result.matchMask |= (1 << bitPosition);
        }
        // Any other character (d, s, i) is skipped!
    }
    return result;
}

int main() {
    CpuConfig config;
    
    try {
        std::cout << "🔄 Attempting to load cpu_config.json...\n";
        config.loadFromFile("cpu_config.json");
        std::cout << "✅ Configuration loaded successfully!\n\n";

        // Let's print out what it parsed to verify the bits!
        std::cout << "--- Parsed Instructions ---\n";
        for (const auto& inst : config.instructions) {
            std::cout << "Instruction: " << inst.name << "\n";
            std::cout << "  Raw Pattern: " << inst.rawPattern << "\n";
            std::cout << "  Base Value:  0x" << std::hex << inst.baseValue << "\n";
            std::cout << "  Match Mask:  0x" << std::hex << inst.matchMask << "\n\n";
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "❌ Test Failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
#endif