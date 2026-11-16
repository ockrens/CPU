#include "cpu_config.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool CpuConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[CpuConfig Error] Kan bestand niet openen: " << filename << "\n";
        return false;
    }

    json config;
    try {
        file >> config;
    } catch (const json::parse_error& e) {
        std::cerr << "[CpuConfig Error] JSON syntaxfout: " << e.what() << "\n";
        return false;
    }

    // 0. Parse & Valideer Settings
    settings = CpuSettings{}; // Reset naar standaardwaarden
    if (config.contains("settings")) {
        const auto& s = config["settings"];

        if (s.contains("eeprom_size")) {
            if (!s["eeprom_size"].is_number_unsigned() || s["eeprom_size"].get<size_t>() == 0) {
                std::cerr << "[CpuConfig Error] Setting 'eeprom_size' moet een positief getal zijn.\n";
                return false;
            }
            settings.eeprom_size = s["eeprom_size"].get<size_t>();
        }

        if (s.contains("eeprom_count")) {
            if (!s["eeprom_count"].is_number_integer() || s["eeprom_count"].get<int>() <= 0) {
                std::cerr << "[CpuConfig Error] Setting 'eeprom_count' moet groter zijn dan 0.\n";
                return false;
            }
            settings.eeprom_count = s["eeprom_count"].get<int>();
        }

        if (s.contains("register_count")) {
            if (!s["register_count"].is_number_integer() || s["register_count"].get<int>() <= 0) {
                std::cerr << "[CpuConfig Error] Setting 'register_count' moet groter zijn dan 0.\n";
                return false;
            }
            settings.register_count = s["register_count"].get<int>();
        }

        if (s.contains("instruction_width")) {
            if (!s["instruction_width"].is_number_integer() || s["instruction_width"].get<int>() <= 0) {
                std::cerr << "[CpuConfig Error] Setting 'instruction_width' moet groter zijn dan 0.\n";
                return false;
            }
            settings.instruction_width = s["instruction_width"].get<int>();
        }
    }

    // 1. Parse Fields en bereken het bitmasker per veld
    if (config.contains("fields")) {
        fields.clear();
        for (const auto& [name, data] : config["fields"].items()) {
            Field f;
            f.name = name;
            f.offset = data.value("offset", 0);
            f.width = data.value("width", 0);

            uint16_t raw_mask = (f.width >= 16) ? 0xFFFF : ((1U << f.width) - 1U);
            f.mask = raw_mask << f.offset;

            fields[name] = f;
        }
    }

    // 2. Parse EEPROM Layout (case-sensitive)
    if (config.contains("eeprom_layout")) {
        eeprom_layout.clear();
        for (const auto& [name, data] : config["eeprom_layout"].items()) {
            EepromSignal sig;
            sig.name = name;
            sig.eeprom = data.value("eeprom", 1);
            sig.offset = data.value("offset", 0);
            sig.width = data.value("width", 0);

            if (data.contains("options")) {
                for (const auto& [opt_name, opt_val] : data["options"].items()) {
                    sig.options[opt_name] = opt_val.get<uint8_t>();
                }
            }
            eeprom_layout[name] = sig;
        }
    }

    // 3. Parse & Valideer Instructions
    if (config.contains("instructions")) {
        instructions.clear();
        bool has_config_errors = false;

        for (const auto& [name, data] : config["instructions"].items()) {
            Instruction inst;
            inst.name = name;
            inst.pattern = data.value("pattern", "");
            calculateMasks(inst);

            if (data.contains("control")) {
                const auto& ctrl = data["control"];

                for (const auto& [sig_name, raw_val] : ctrl.items()) {
                    // Uitzondering: func_unit_data (5 bits in EEPROM 2 -> max waarde 31)
                    if (sig_name == "func_unit_data") {
                        if (!raw_val.is_number_integer()) {
                            std::cerr << "[CpuConfig Error] Instructie '" << name
                                      << "': 'func_unit_data' moet een getal zijn.\n";
                            has_config_errors = true;
                            continue;
                        }
                        int data_val = raw_val.get<int>();
                        if (data_val < 0 || data_val > 31) {
                            std::cerr << "[CpuConfig Error] Instructie '" << name
                                      << "': 'func_unit_data' (" << data_val << ") valt buiten bereik (0..31).\n";
                            has_config_errors = true;
                            continue;
                        }
                        inst.control[sig_name] = std::to_string(data_val);
                        inst.eeprom2_word |= ((static_cast<uint8_t>(data_val) & 0x1F) << 3);
                        continue;
                    }

                    // Check 1: Bestaat dit signaal in eeprom_layout?
                    auto layout_it = eeprom_layout.find(sig_name);
                    if (layout_it == eeprom_layout.end()) {
                        std::cerr << "[CpuConfig Error] Instructie '" << name
                                  << "': Onbekend control-signaal '" << sig_name
                                  << "' (bestaat niet in eeprom_layout).\n";
                        has_config_errors = true;
                        continue;
                    }

                    const EepromSignal& sig_cfg = layout_it->second;
                    uint8_t resolved_val = 0;

                    // Check 2: String-waarde exact (case-sensitive) controleren tegen options
                    if (raw_val.is_string()) {
                        std::string choice_raw = raw_val.get<std::string>();

                        auto opt_it = sig_cfg.options.find(choice_raw);
                        if (opt_it == sig_cfg.options.end()) {
                            std::cerr << "[CpuConfig Error] Instructie '" << name
                                      << "': Ongeldige waarde '" << choice_raw
                                      << "' voor '" << sig_name << "'. Toegestaan: ";
                            for (const auto& [opt_k, _] : sig_cfg.options) {
                                std::cerr << "'" << opt_k << "' ";
                            }
                            std::cerr << "\n";
                            has_config_errors = true;
                            continue;
                        }

                        resolved_val = opt_it->second;
                        inst.control[sig_name] = choice_raw;

                    // Check 3: Numerieke waarde controleren tegen bit-width
                    } else if (raw_val.is_number_integer()) {
                        int num_val = raw_val.get<int>();
                        int max_val = (1 << sig_cfg.width) - 1;

                        if (num_val < 0 || num_val > max_val) {
                            std::cerr << "[CpuConfig Error] Instructie '" << name
                                      << "': Waarde " << num_val << " voor '" << sig_name
                                      << "' past niet in " << sig_cfg.width << " bits (0.." << max_val << ").\n";
                            has_config_errors = true;
                            continue;
                        }

                        resolved_val = static_cast<uint8_t>(num_val);
                        inst.control[sig_name] = std::to_string(num_val);

                    } else {
                        std::cerr << "[CpuConfig Error] Instructie '" << name
                                  << "': Ongeldig datatype voor '" << sig_name << "'.\n";
                        has_config_errors = true;
                        continue;
                    }

                    // Verwerk in het juiste EEPROM-woord
                    uint8_t raw_mask = (1U << sig_cfg.width) - 1U;
                    uint8_t shifted_val = (resolved_val & raw_mask) << sig_cfg.offset;

                    if (sig_cfg.eeprom == 1) {
                        inst.eeprom1_word |= shifted_val;
                    } else if (sig_cfg.eeprom == 2) {
                        inst.eeprom2_word |= shifted_val;
                    }
                }
            }

            instructions[name] = inst;
        }

        // Stop en geef false terug bij ongeldige waardes in de JSON
        if (has_config_errors) {
            return false;
        }
    }

    return true;
}

const Field* CpuConfig::getField(const std::string& name) const {
    auto it = fields.find(name);
    return (it != fields.end()) ? &(it->second) : nullptr;
}

const Instruction* CpuConfig::findInstructionByName(const std::string& name) const {
    auto it = instructions.find(name);
    return (it != instructions.end()) ? &(it->second) : nullptr;
}

bool CpuConfig::packField(uint16_t& machine_code, const std::string& field_name, uint16_t value) const {
    const Field* f = getField(field_name);
    if (!f) return false;
    machine_code = (machine_code & ~f->mask) | f->pack(value);
    return true;
}

const Instruction* CpuConfig::decodeInstruction(uint16_t machine_code) const {
    for (const auto& [name, inst] : instructions) {
        if ((machine_code & inst.match_mask) == inst.base_value) {
            return &inst;
        }
    }
    return nullptr;
}

uint16_t CpuConfig::extractField(uint16_t machine_code, const std::string& field_name) const {
    const Field* f = getField(field_name);
    return f ? f->extract(machine_code) : 0;
}

void CpuConfig::calculateMasks(Instruction& inst) {
    uint16_t base = 0;
    uint16_t mask = 0;
    int bit_position = 15;

    for (char c : inst.pattern) {
        if (c == ' ') continue;
        if (bit_position < 0) break;

        if (c == '0') {
            mask |= (1U << bit_position);
        } else if (c == '1') {
            mask |= (1U << bit_position);
            base |= (1U << bit_position);
        }
        bit_position--;
    }

    inst.base_value = base;
    inst.match_mask = mask;
}