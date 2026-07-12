/**
 * @file main.hpp
 * @brief main definitions file.
 * 
 * @author R.J. Ockhuijsen
 * @date 2026-01-28
 * @version 0.1
 */
#ifndef MAIN_H
#define MAIN_H


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <stdexcept>
#include <nlohmann/json.hpp>
// 📑 Represents a specific bit-slice inside a 16-bit instruction
struct Field {
    std::string name;   // e.g., "reg_dest"
    uint8_t offset;     // Bit position where this field starts (0-15)
    uint8_t bitWidth;   // How many bits this field occupies
    
    // 🧮 Dynamically generates a bitmask for this field
    uint16_t getMask() const {
        return ((1 << bitWidth) - 1) << offset;
    }
};

// ⚙️ Represents a single CPU instruction definition
struct InstructionDef {
    std::string name;          // e.g., "ADD" or "LDI"
    std::string bitPattern;    // e.g., "0001 dddd ssss 0000"
    std::vector<std::string> operands; // Fields used by this instruction
    
    // 🔌 Control signals for the decoder EEPROMs
    std::map<std::string, std::string> controlSignals;
};
#endif

struct InstructionPattern {
    uint16_t baseValue = 0;
    uint16_t matchMask = 0;
};
struct CpuField {
    std::string name;
    uint8_t offset;
    uint8_t bitWidth;
    bool isSigned; // 🪧 New flag to support signed immediate ranges
};

struct CpuInstruction {
    std::string name;
    std::string rawPattern;
    uint16_t baseValue;
    uint16_t matchMask;
    std::vector<std::string> fieldsUsed;
    std::map<std::string, std::string> controlSignals;
};

using json = nlohmann::json;

// 📑 Structure for instruction bit-fields
class CpuConfig {
public:
    uint32_t memorySize = 65536;
    std::map<std::string, CpuField> fields;
    std::vector<CpuInstruction> instructions;

    // 📂 Opens, reads, and parses the configuration JSON file
    void loadFromFile(const std::string& filename) {
        std::ifstream configFile(filename);
        if (!configFile.is_open()) {
            throw std::runtime_error("Failed to open configuration file: " + filename);
        }

        json j;
        configFile >> j; 

        // 1. Load general parameters
        memorySize = j["parameters"].value("memory_size", 65536);

        // 2. Load the bit fields
        for (auto& [name, fieldData] : j["fields"].items()) {
            CpuField field;
            field.name = name;
            field.offset = fieldData["offset"];
            field.bitWidth = fieldData["width"];
            field.isSigned = fieldData.value("is_signed", false); 
            
            fields[name] = field;
        }

        // 3. Load the instructions
        for (auto& instData : j["instructions"]) {
            std::string name = instData["name"];
            std::string pattern = instData["pattern"];
            std::vector<std::string> fieldsUsed = instData["fields"];
            
            std::map<std::string, std::string> signals;
            if (instData.contains("control_signals")) {
                signals = instData["control_signals"].get<std::map<std::string, std::string>>();
            }

            instructions.push_back(createInstruction(name, pattern, fieldsUsed, signals));
        }

        configFile.close();
    }

    // 🛡️ Safety check to validate if a parsed operand fits in its field
    void validateOperandRange(const std::string& fieldName, int32_t value) const {
        if (fields.find(fieldName) == fields.end()) {
            throw std::runtime_error("Unknown field: " + fieldName);
        }

        const CpuField& field = fields.at(fieldName);
        
        if (field.isSigned) {
            // 🪓 Signed range split: -2^(n-1) to 2^(n-1) - 1
            int32_t minVal = -(1 << (field.bitWidth - 1));
            int32_t maxVal = (1 << (field.bitWidth - 1)) - 1;
            if (value < minVal || value > maxVal) {
                throw std::out_of_range("Operand '" + fieldName + "' out of signed range (" 
                                       + std::to_string(minVal) + " to " + std::to_string(maxVal) + ")");
            }
        } else {
            // 🪙 Unsigned range: 0 to 2^n - 1
            uint32_t maxVal = (1 << field.bitWidth) - 1;
            if (value < 0 || static_cast<uint32_t>(value) > maxVal) {
                throw std::out_of_range("Operand '" + fieldName + "' out of unsigned range (0 to " 
                                       + std::to_string(maxVal) + ")");
            }
        }
    }

private:
    // 🔄 Helper to parse pattern strings into base values and match masks
    CpuInstruction createInstruction(const std::string& name, 
                                     const std::string& rawPattern,
                                     const std::vector<std::string>& fieldsUsed,
                                     const std::map<std::string, std::string>& signals) {
        CpuInstruction inst;
        inst.name = name;
        inst.rawPattern = rawPattern;
        inst.fieldsUsed = fieldsUsed;
        inst.controlSignals = signals;

        std::string cleanPattern = "";
        for (char c : rawPattern) {
            if (c != ' ') cleanPattern += c;
        }

        inst.baseValue = 0;
        inst.matchMask = 0;

        for (int i = 0; i < 16; i++) {
            int bitPosition = 15 - i;
            char bitChar = cleanPattern[i];

            if (bitChar == '1') {
                inst.baseValue |= (1 << bitPosition);
                inst.matchMask |= (1 << bitPosition);
            } 
            else if (bitChar == '0') {
                inst.matchMask |= (1 << bitPosition);
            }
        }
        return inst;
    }
};