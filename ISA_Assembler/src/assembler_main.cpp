#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include "cpu_config.hpp"
#include <stdexcept>
#include <cstring>
#include <vector>

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}


int main(int argc, char *argv[]) {

    /*---------------------------------
    ----- Terminal Input Handling -----
    ---------------------------------*/
    if (argc < 2) {
        throw std::runtime_error("No program argument provided. Specify a .s or .asm file.");
    }

    const char *filepointer = argv[1];
    size_t len = std::strlen(filepointer);
    std::string filename = argv[1];
    std::string filename_sanitized = filename.substr(0, filename.find_last_of('.')) + ".txt";

    bool is_s = (len >= 2 && std::strcmp(filepointer+ len - 2, ".s") == 0);
    bool is_asm = (len >= 4 && std::strcmp(filepointer+ len - 4, ".asm") == 0);

    if (!is_s && !is_asm) {
        throw std::runtime_error("Provide a valid .s or .asm file as an argument.");
    }




    /*----------------------
    ----- File Loading -----
    ----------------------*/

    // Laad de CPU-configuratie
    CpuConfig config;
    if (!config.loadFromFile("cpu_config.json")) {
        return 1;
    }
    // Print het bestand
    std::ifstream asm_file(filepointer);
    std::ofstream sanitized_file(filename_sanitized);


    /*---------------------------
    ----- File Sanitization -----
    ---------------------------*/
    std::vector<std::string> sanitized_lines;
    std::string line;
    while (std::getline(asm_file, line)) {
        // 1. Verwijder commentaar
        size_t comment_pos = line.find(';');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        // 2. Trim witruimte
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            continue;
        }
        size_t last = line.find_last_not_of(" \t\r\n");
        line = line.substr(first, (last - first + 1));

        // 3. Sla intern op in de vector
        sanitized_lines.push_back(line);

        // 4. (Optioneel) Schrijf ook weg naar bestand ter controle
        if (sanitized_file.is_open()) {
            sanitized_file << line << '\n';
        }
    }
    /*-------------------------
    ----- Label handeling -----
    -------------------------*/
    // Vanaf hier werk je verder met 'sanitized_lines':
    std::unordered_map<std::string, uint32_t> symbol_table;
    const uint32_t INSTRUCTION_SIZE = 1; // <-- Pas aan naar het aantal bytes/woorden per instructie (bijv. 1, 2 of 4)
    uint32_t current_address = 0;

    // =========================================================================
    // PASS 1: Labels en ORG-offsets verzamelen (sanitized_lines blijft intact)
    // =========================================================================
    for (const std::string& instruction : sanitized_lines) {
        std::string temp = instruction;

        // 1. Check op een label (bijv. "loop:" of "start: MOV AX, 1")
        size_t colon_pos = temp.find(':');
        if (colon_pos != std::string::npos) {
            std::string label_name = trim(temp.substr(0, colon_pos));
            
            if (symbol_table.count(label_name)) {
                std::cerr << "Fout: Label '" << label_name << "' is dubbel gedefinieerd!\n";
                return 1;
            }
            symbol_table[label_name] = current_address;

            // Kijk of er ná het label nog iets op dezelfde regel staat
            temp = trim(temp.substr(colon_pos + 1));
            if (temp.empty()) {
                continue; // Alleen een label op deze regel -> adres verandert niet
            }
        }

        // 2. Check op ORG directive (bijv. "ORG 0x100" of "org 256")
        if (temp.rfind("ORG ", 0) == 0 || temp.rfind("org ", 0) == 0) {
            std::string addr_str = trim(temp.substr(4));
            current_address = std::stoul(addr_str, nullptr, 0);
            continue; // ORG neemt zelf geen geheugenruimte in
        }

        // 3. Het is een echte instructie -> hoog het adres op met de instructiegrootte
        current_address += INSTRUCTION_SIZE;
    }

    // =========================================================================
    // PASS 2: Instructies daadwerkelijk parsen met behulp van symbol_table
    // =========================================================================
    current_address = 0; // Reset de teller weer naar het begin

    for (const std::string& instruction : sanitized_lines) {
        std::string line = instruction;

        // 1. Strip het label er nu wél vanaf (het adres staat immers al in symbol_table)
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            line = trim(line.substr(colon_pos + 1));
            if (line.empty()) {
                continue; // Regel bevatte alleen een label, niets om te parsen
            }
        }

        // 2. Verwerk ORG opnieuw zodat current_address synchroon loopt
        if (line.rfind("ORG ", 0) == 0 || line.rfind("org ", 0) == 0) {
            std::string addr_str = trim(line.substr(4));
            current_address = std::stoul(addr_str, nullptr, 0);
            continue;
        }

        // 3. Parse de overgebleven instructie!
        // 'line' is nu gegarandeerd een schone instructie (bijv. "JMP einde" of "MOV R0, 5")
        // en 'current_address' is het exacte adres waar deze instructie komt te staan.

        // ... Jouw parse-logica hier ...

        // Verhoog na het parsen weer het adres met dezelfde grootte
        current_address += INSTRUCTION_SIZE;
    }












    // Stel: we assembleren "ADD R2, R3"
    std::string mnemonic = "ADD";
    uint16_t dest_reg = 2; // R2
    uint16_t src_reg  = 3; // R3

    // 1. Zoek de instructie op
    const Instruction* inst = config.findInstructionByName(mnemonic);
    if (!inst) {
        std::cerr << "Onbekende instructie: " << mnemonic << "\n";
        return 1;
    }

    // 2. Start met de vaste opcode-bits uit het patroon ("0000 ssss dddd 0001" -> 0x0001)
    uint16_t machine_code = inst->base_value;

    // 3. Gebruik de bitmasks uit CpuConfig om de velden te vullen
    config.packField(machine_code, "reg_dest", dest_reg);

    // Controleer via rs_mode of het tweede argument een register of immediate is
    std::string rs_mode = inst->getControl("rs_mode");
    if (rs_mode == "r8" || rs_mode == "r16") {
        config.packField(machine_code, "reg_src", src_reg);
    } else if (rs_mode == "imm4" || rs_mode == "imm8") {
        config.packField(machine_code, rs_mode, src_reg);
    }

    // 4. Print het resultaat
    std::cout << "=== ASSEMBLER OUTPUT ===\n";
    std::cout << "Instructie   : " << mnemonic << " R" << dest_reg << ", R" << src_reg << "\n";
    std::cout << "Patroon      : " << inst->pattern << "\n";
    std::cout << "Machinetaal  : 0x" << std::hex << std::setw(4) << std::setfill('0') << machine_code << std::dec << "\n";
    std::cout << "Binair       : " << std::bitset<16>(machine_code) << "\n";

    return 0;
}