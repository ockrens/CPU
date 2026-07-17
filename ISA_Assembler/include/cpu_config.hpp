#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

struct Field {
    int offset;
    int width;
};

struct EepromSignalConfig {
    int eeprom = 1; // 1 of 2
    int offset = 0;
    int width = 0;
    std::unordered_map<std::string, int> options;
};

struct Instruction {
    std::string name;
    std::string pattern;
    uint16_t base_value;
    uint16_t match_mask;
    uint8_t eeprom1_word;   // Fysieke byte voor EEPROM 1
    uint8_t eeprom2_word;   // Fysieke byte voor EEPROM 2
};

class CpuConfig {
public:
    CpuConfig() = default;
    bool loadFromFile(const std::string& filename);

    const Field* getField(const std::string& name) const;
    const Instruction* findInstructionByPattern(uint16_t machine_code) const;
    const Instruction* findInstructionByName(const std::string& name) const;

private:
    std::unordered_map<std::string, Field> fields;
    std::unordered_map<std::string, EepromSignalConfig> eeprom_layout;
    std::unordered_map<std::string, Instruction> instructions_by_name;
    
    void calculateMasks(Instruction& inst);
};