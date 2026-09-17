#include "cpu_config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <optional>
#include <cstdint>
#include <iomanip> // Voor std::setw en std::setfill

std::vector<std::string> readAssemblyFile(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    if (!file.is_open()) return lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    return lines;
}

bool isInstructionLine(const std::string& line) {
    std::string clean = line;
    size_t comment_pos = clean.find(';');
    if (comment_pos != std::string::npos) clean = clean.substr(0, comment_pos);
    for (char c : clean) {
        if (!std::isspace(static_cast<unsigned char>(c))) return true;
    }
    return false;
}

std::string sanitizeInstruction(const std::string& line) {
    std::string clean = line;
    size_t comment_pos = clean.find(';');
    if (comment_pos != std::string::npos) clean = clean.substr(0, comment_pos);
    size_t label_pos = clean.find(':');
    if (label_pos != std::string::npos) clean = clean.substr(label_pos + 1);
    return clean;
}

std::optional<int32_t> parseOperandStrict(const std::string& arg, int line_num, const std::string& arg_type) {
    if (arg.empty()) return std::nullopt;
    try {
        if (arg[0] == 'R' || arg[0] == 'r') {
            return std::stoi(arg.substr(1));
        }
        return std::stoi(arg, nullptr, 0);
    } catch (...) {
        std::cerr << "  [ASSEMBLER ERROR] Regel " << line_num 
                  << ": Ongeldige syntaxis voor " << arg_type << " '" << arg << "'\n";
        return std::nullopt;
    }
}

// Validator voor gecombineerde signed (-2^(w-1)) én unsigned (2^w - 1) bereiken
bool validateAndFormatImmediate(int32_t val, int width, uint16_t& out_bits) {
    int32_t min_signed = -(1 << (width - 1));
    int32_t max_unsigned = (1 << width) - 1;

    if (val < min_signed || val > max_unsigned) {
        return false;
    }

    uint32_t mask = (1U << width) - 1U;
    out_bits = static_cast<uint16_t>(val & mask);
    return true;
}

std::optional<uint16_t> assembleLine(const CpuConfig& config, const std::string& raw_line, int line_num) {
    std::string line = raw_line;
    std::replace(line.begin(), line.end(), ',', ' ');
    
    std::stringstream ss(line);
    std::string mnemonic;
    ss >> mnemonic;

    if (mnemonic.empty()) return std::nullopt;

    const Instruction* inst = config.findInstructionByName(mnemonic);
    if (!inst) {
        std::cerr << "  [ASSEMBLER ERROR] Regel " << line_num 
                  << ": Onbekende instructie '" << mnemonic << "'\n";
        return std::nullopt;
    }

    uint16_t machine_code = inst->base_value;

    std::vector<std::string> args;
    std::string arg;
    while (ss >> arg) args.push_back(arg);

    const int32_t MAX_REGISTER_INDEX = 7; 

    // -------------------------------------------------------------
    // 1. Eerste argument (args[0]) -> Moet ALTIJD een register zijn
    // -------------------------------------------------------------
    if (args.size() >= 1) {
        bool is_register = (args[0][0] == 'R' || args[0][0] == 'r');
        
        // Voorkom dat getallen (zoals '20' in 'ADD 20, r1') worden geïnterpreteerd als register
        if (!is_register) {
            std::cerr << "  [ASSEMBLER ERROR] Regel " << line_num 
                      << ": Instructie '" << mnemonic 
                      << "' verwacht een bestemmingsregister als eerste argument, maar kreeg '" << args[0] << "'.\n";
            return std::nullopt;
        }

        const Field* dest = config.getField("reg_dest");
        if (dest) {
            auto val = parseOperandStrict(args[0], line_num, "bestemmingsregister");
            if (!val.has_value()) return std::nullopt;

            if (val.value() < 0 || val.value() > MAX_REGISTER_INDEX) {
                std::cerr << "  [ASSEMBLER ERROR] Regel " << line_num 
                          << ": Register R" << val.value() << " bestaat niet!\n";
                return std::nullopt;
            }
            machine_code |= (static_cast<uint16_t>(val.value()) << dest->offset);
        }
    }
    
    // -------------------------------------------------------------
    // 2. Tweede argument (args[1]) -> Controle tegen rs_mode
    // -------------------------------------------------------------
    if (args.size() >= 2) {
        bool is_register = (args[1][0] == 'R' || args[1][0] == 'r');
        std::string rs_mode = inst->control.count("rs_mode") ? inst->control.at("rs_mode") : "";

        // Check 1: rs_mode is een register, maar gebruiker geeft een getal
        if ((rs_mode == "r8" || rs_mode == "r16") && !is_register) {
            std::cerr << "  [ASSEMBLER ERROR] Regel " << line_num 
                      << ": Instructie '" << mnemonic 
                      << "' verwacht een bronregister als tweede argument, maar kreeg '" << args[1] << "'.\n";
            return std::nullopt;
        }

        // Check 2: rs_mode is een immediate, maar gebruiker geeft een register
        if ((rs_mode == "imm4" || rs_mode == "imm8") && is_register) {
            std::cerr << "  [ASSEMBLER ERROR] Regel " << line_num 
                      << ": Instructie '" << mnemonic 
                      << "' verwacht een immediate waarde als tweede argument, maar kreeg register '" << args[1] << "'.\n";
            return std::nullopt;
        }

        // Verwerk register als rs_mode dat eist
        if (is_register) {
            const Field* src = config.getField("reg_src");
            auto val = parseOperandStrict(args[1], line_num, "bronregister");
            if (!val.has_value() || !src) return std::nullopt;

            if (val.value() < 0 || val.value() > MAX_REGISTER_INDEX) {
                std::cerr << "  [ASSEMBLER ERROR] Regel " << line_num 
                          << ": Register R" << val.value() << " bestaat niet!\n";
                return std::nullopt;
            }
            machine_code |= (static_cast<uint16_t>(val.value()) << src->offset);

        } else {
            // Verwerk immediate op basis van de exacte breedte uit rs_mode
            const Field* imm = config.getField(rs_mode); 
            if (!imm) imm = config.getField("imm8");
            if (!imm) imm = config.getField("imm4");

            auto val = parseOperandStrict(args[1], line_num, "immediate waarde");
            if (!val.has_value() || !imm) return std::nullopt;

            uint16_t formatted_bits = 0;
            if (!validateAndFormatImmediate(val.value(), imm->width, formatted_bits)) {
                int32_t min_signed = -(1 << (imm->width - 1));
                int32_t max_unsigned = (1 << imm->width) - 1;
                std::cerr << "  [ASSEMBLER ERROR] Regel " << line_num 
                          << ": Waarde " << val.value() << " valt buiten bereik van " 
                          << imm->width << "-bit field (toegestaan: " << min_signed 
                          << " t/m " << max_unsigned << ")\n";
                return std::nullopt;
            }

            machine_code |= (formatted_bits << imm->offset);
        }
    }

    return machine_code;
}

int main() {
    CpuConfig config;
    if (!config.loadFromFile("cpu_config.json")) return 1;

    std::vector<std::string> file_lines = readAssemblyFile("test.asm");
    std::vector<uint16_t> program_rom;
    int error_count = 0;

    std::cout << "--- Start Assembleren ---\n";

    for (size_t i = 0; i < file_lines.size(); ++i) {
        int line_num = static_cast<int>(i + 1);
        std::string clean_inst = sanitizeInstruction(file_lines[i]);

        if (isInstructionLine(clean_inst)) {
            auto machine_code = assembleLine(config, clean_inst, line_num);

            if (machine_code.has_value()) {
                program_rom.push_back(machine_code.value());
                
                // Formatteer de hex-output altijd met 4 karakters inclusief voorloopnullen
                std::cout << "[OK] Regel " << line_num << ": " << clean_inst 
                          << " -> Hex: 0x" 
                          << std::hex << std::setw(4) << std::setfill('0') << machine_code.value() 
                          << std::dec << "\n";
            } else {
                error_count++;
            }
        }
    }

    std::cout << "\n--- Assemblage Voltooid ---\n";
    std::cout << "Succesvol: " << program_rom.size() << " | Fouten: " << error_count << "\n";

    return (error_count == 0) ? 0 : 1;
}