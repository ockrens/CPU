#include "cpu_config.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool CpuConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file " << filename << std::endl;
        return false;
    }

    json config;
    try {
        file >> config;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return false;
    }

    // 1. Parse Fields (opcode, reg_dest, reg_src, imm4, imm8)
    if (config.contains("fields")) {
        fields.clear();
        for (auto& [name, data] : config["fields"].items()) {
            fields[name] = Field{data.value("offset", 0), data.value("width", 0)};
        }
    }

    // 2. Parse EEPROM Layout (Dynamische toewijzing aan EEPROM 1 of EEPROM 2)
    if (config.contains("eeprom_layout")) {
        eeprom_layout.clear();
        for (auto& [name, data] : config["eeprom_layout"].items()) {
            EepromSignalConfig sig;
            sig.eeprom = data.value("eeprom", 1);
            sig.offset = data.value("offset", 0);
            sig.width = data.value("width", 0);
            
            if (data.contains("options")) {
                for (auto& [opt_name, opt_val] : data["options"].items()) {
                    sig.options[opt_name] = opt_val.get<int>();
                }
            }
            eeprom_layout[name] = sig;
        }
    }

    // 3. Parse Instructions & Genereer de fysieke EEPROM bytes
    if (config.contains("instructions")) {
        instructions_by_name.clear();
        for (auto& [name, data] : config["instructions"].items()) {
            Instruction inst;
            inst.name = name;
            inst.pattern = data.value("pattern", "");
            calculateMasks(inst);
            
            // Fysieke bytes beginnen standaard in de veilige ruststand (0x00)
            inst.eeprom1_word = 0;
            inst.eeprom2_word = 0;
            
            if (data.contains("control")) {
                auto ctrl = data["control"];
                
                // Verwerk alle dynamische lay-out signalen (rd_mode, rs_mode, flags_mode, func_unit)
                for (auto& [sig_name, sig_cfg] : eeprom_layout) {
                    if (ctrl.contains(sig_name)) {
                        uint8_t final_val = 0;
                        
                        if (ctrl[sig_name].is_string()) {
                            // Zet tekst (bijv. "read_and_write") om naar de bit-waarde via de opties
                            std::string choice = ctrl[sig_name].get<std::string>();
                            if (sig_cfg.options.count(choice)) {
                                final_val = sig_cfg.options.at(choice);
                            } else {
                                std::cerr << "Warning: Gekozen optie '" << choice 
                                          << "' bestaat niet voor signaal '" << sig_name << "'\n";
                            }
                        } else if (ctrl[sig_name].is_number()) {
                            // Direct getal (bijv. 0 of 1)
                            final_val = ctrl[sig_name].get<uint8_t>();
                        }
                        
                        // Schuif de bits in de juiste fysieke EEPROM chip
                        if (sig_cfg.eeprom == 1) {
                            inst.eeprom1_word |= (final_val << sig_cfg.offset);
                        } else if (sig_cfg.eeprom == 2) {
                            inst.eeprom2_word |= (final_val << sig_cfg.offset);
                        }
                    }
                }

                // Verwerk func_unit_data (als los binair getal binnen het control-blok)
                if (ctrl.contains("func_unit_data")) {
                    uint8_t data_val = ctrl["func_unit_data"].get<uint8_t>();
                    // Shift 3 bits omhoog om ruimte te maken voor de func_unit (bits 0-2) op EEPROM 2
                    inst.eeprom2_word |= (data_val << 3); 
                }
            }
            instructions_by_name[name] = inst;
        }
    }
    return true;
}

const Field* CpuConfig::getField(const std::string& name) const {
    auto it = fields.find(name);
    if (it != fields.end()) {
        return &(it->second);
    }
    return nullptr;
}

const Instruction* CpuConfig::findInstructionByName(const std::string& name) const {
    auto it = instructions_by_name.find(name);
    if (it != instructions_by_name.end()) {
        return &(it->second);
    }
    return nullptr;
}

const Instruction* CpuConfig::findInstructionByPattern(uint16_t machine_code) const {
    for (auto& [name, inst] : instructions_by_name) {
        if ((machine_code & inst.match_mask) == inst.base_value) {
            return &inst;
        }
    }
    return nullptr;
}

void CpuConfig::calculateMasks(Instruction& inst) {
    uint16_t base = 0;
    uint16_t mask = 0;
    int bit_position = 15;

    // Haal spaties uit het patroon string weg
    std::string clean_pattern = "";
    for (char c : inst.pattern) {
        if (c != ' ') clean_pattern += c;
    }

    // Bouw de bitmasks op van bit 15 naar bit 0
    for (char c : clean_pattern) {
        if (bit_position < 0) break;

        if (c == '0') {
            mask |= (1 << bit_position);
        } else if (c == '1') {
            mask |= (1 << bit_position);
            base |= (1 << bit_position);
        }
        bit_position--;
    }

    inst.base_value = base;
    inst.match_mask = mask;
}