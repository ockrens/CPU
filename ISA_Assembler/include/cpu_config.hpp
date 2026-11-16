#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

struct CpuSettings {
    size_t eeprom_size = 65536;    // Bijv. 64K (2^16 adressen voor 16-bit instructies)
    int eeprom_count = 2;          // Aantal EEPROM-chips
    int register_count = 8;        // Aantal registers (bijv. R0 t/m R7)
    int instruction_width = 16;    // Breedte van een instructie in bits
};

struct Field {
    std::string name;
    int offset = 0;
    int width = 0;
    uint16_t mask = 0;

    // Voor de ASSEMBLER: zet een waarde op de juiste bitpositie
    uint16_t pack(uint16_t value) const {
        uint16_t raw_mask = (width >= 16) ? 0xFFFF : ((1U << width) - 1U);
        return (value & raw_mask) << offset;
    }

    // Voor de INST_DECODER: haal de waarde uit een 16-bit instructie
    uint16_t extract(uint16_t machine_code) const {
        return (machine_code & mask) >> offset;
    }
};

struct EepromSignal {
    std::string name;
    int eeprom = 1; // 1 of 2
    int offset = 0;
    int width = 0;
    std::unordered_map<std::string, uint8_t> options;
};

struct Instruction {
    std::string name;
    std::string pattern;

    uint16_t base_value = 0;
    uint16_t match_mask = 0;

    uint8_t eeprom1_word = 0;
    uint8_t eeprom2_word = 0;

    std::unordered_map<std::string, std::string> control;

    std::string getControl(const std::string& key, const std::string& default_val = "") const {
        auto it = control.find(key);
        return (it != control.end()) ? it->second : default_val;
    }
};

class CpuConfig {
public:
    CpuConfig() = default;
    bool loadFromFile(const std::string& filename);

    // Algemene instellingen & data toegang
    const CpuSettings& getSettings() const { return settings; }
    const Field* getField(const std::string& name) const;
    const std::unordered_map<std::string, Field>& getAllFields() const { return fields; }
    const std::unordered_map<std::string, Instruction>& getAllInstructions() const { return instructions; }

    // Voor de ASSEMBLER
    const Instruction* findInstructionByName(const std::string& name) const;
    bool packField(uint16_t& machine_code, const std::string& field_name, uint16_t value) const;

    // Voor de INST_DECODER
    const Instruction* decodeInstruction(uint16_t machine_code) const;
    uint16_t extractField(uint16_t machine_code, const std::string& field_name) const;

private:
    CpuSettings settings;
    std::unordered_map<std::string, Field> fields;
    std::unordered_map<std::string, EepromSignal> eeprom_layout;
    std::unordered_map<std::string, Instruction> instructions;

    static void calculateMasks(Instruction& inst);
};