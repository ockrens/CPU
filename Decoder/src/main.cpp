/**
 * @file main.cpp
 * @brief Main file for the decoder program.
 * 
 * @author R.J. Ockhuijsen
 * @date 2026-02-23
 * @version 0.1
 */

#include "../include/main.hpp"

#ifndef MAIN_CPP
#define MAIN_CPP

using namespace std;
using json = nlohmann::json;

uint8_t rom1Arr[MemorySize]{0};
uint8_t rom2Arr[MemorySize]{0};


namespace nlohmann
{
    template <>
    struct adl_serializer<Block>{
        static void from_json(const json& j, Block& b){
            j.at("name").get_to(b.name);
            j.at("type").get_to(b.type);

            // memory block
            if (b.type == "block")
            {
                b.startAddress = j.value("startAddress", 0);
                b.endAddress   = j.value("endAddress", 0);
            }
            // item block
            else if (b.type == "item")
            {
                if (j.contains("address") && j["address"].is_array())
                    j.at("address").get_to(b.addresses);
                else
                    b.addresses = {};
            }
            else if (b.type == "bitmode")
            {
               b.bitMode = j.value("bits", string(16,'0'));
            }


            // controlSignals sub-object (nested)
            if (j.contains("controlSignals") && j["controlSignals"].is_object())
            {
                const auto& cs = j["controlSignals"];
                b.RDMode   = cs.value("RDMode", "NoRD");
                b.RSMode   = cs.value("RSMode", "NoRS");
                b.FuncUnit = cs.value("FuncUnit", "None");
                b.FuncData = cs.value("FuncData", 0);
                b.FlagMode = cs.value("FlagMode", "None");
                b.PCMode   = cs.value("PCMode", "None");
            }
            else
            {
                b.RDMode   = "NULL";
                b.RSMode   = "NULL";
                b.FuncUnit = "MOVE";
                b.FuncData = 0;
                b.FlagMode = "NULL";
                b.PCMode   = "STEP";
            }
        }
    };
}


int main(){
    ifstream jsonFile("test.json");
    if (!jsonFile){
        cerr << "Cannot open JSON file\n";
        return 1;
    }

    json jdata;
    jsonFile >> jdata;

    vector<Block> blocks = jdata.at("blocks").get<vector<Block>>();

    ofstream rom1File("rom1.bin", ios::out | ios::binary);
    ofstream rom2File("rom2.bin", ios::out | ios::binary);

    if (!rom1File || !rom2File){
        cerr << "Failed to open ROM output files\n";
        return 1;
    }

/*////////////////////////////////////////////////////
////// Convert JSON data to ROM control singals //////
////////////////////////////////////////////////////*/
    for(const auto& b : blocks){

        // rom 1 control signals
        uint8_t rom1control = 0x00;

        // RDMode bitset
        if (b.RDMode == "R") rom1control    |= 0x01; // bit 0
        if (b.RDMode == "W") rom1control    |= 0x02; // bit 1
        if (b.RDMode == "RW") rom1control   |= 0x03; // bit 0-1
        // RSMode bitset
        cout << "RDMode: " << hex << rom1control << endl;
        if (b.RSMode == "R8") rom1control   |= 0x00; // bit 2-3       (maybe not needed if its starting at 0x00 (all bits are already 0))
        if (b.RSMode == "R16") rom1control  |= 0x04; // bit 2-3
        if (b.RSMode == "Imm4") rom1control |= 0x0C; // bit 2-3
        if (b.RSMode == "Imm8") rom1control |= 0x08; // bit 2-3
        cout << "RSMode: " << hex << rom1control << endl;
        // FlagMode bitset
        if (b.FlagMode == "R") rom1control  |= 0x10; // bit 4
        if (b.FlagMode == "W") rom1control  |= 0x20; // bit 5

        // PCMode bitset
        if (b.PCMode == "STEP") rom1control |= 0x00; // bit 6-7     (maybe not needed if its starting at 0x00 (all bits are already 0))
        if (b.PCMode == "JMPR") rom1control |= 0x40; // bit 6-7 
        if (b.PCMode == "JMPA") rom1control |= 0x80; // bit 6-7 
    

        // rom 2 control signals
        uint8_t rom2control = 0x00;

        // FuncUnit bitset
        if (b.FuncUnit == "ALU"){
            rom2control |= 0x40; // bit 6-7
            rom2control |= (b.FuncData & 0x3F); // bit 0-5 for ALU function code
        };

        if (b.type == "bitmode"){
            // Remove spaces from bitMode string
            std::string pattern;
            for (char c : b.bitMode){
                if (!std::isspace(static_cast<unsigned char>(c)))
                    pattern += c;
            }

            if (pattern.size() != 16){
                cerr << "bitMode must contain 16 bits (excluding spaces)\n";
                continue;
            }

            vector<int> floatingPositions;
            uint16_t staticMask = 0;

            // Build static mask and floating positions
            for (int i = 0; i < 16; i++){
                int bitPosition = 15 - i;  // MSB left

                if (pattern[i] == '1'){
                    staticMask |= (1 << bitPosition);
                }
                else if (pattern[i] == '2'){
                    floatingPositions.push_back(bitPosition);
                }
            }

            int floatingCount = floatingPositions.size();
            int combinations = 1 << floatingCount;

            for (int comb = 0; comb < combinations; comb++){
                uint16_t address = staticMask;

                for (int bit = 0; bit < floatingCount; bit++){
                    if (comb & (1 << bit))
                        address |= (1 << floatingPositions[bit]);
                    else
                        address &= ~(1 << floatingPositions[bit]);
                }
                //cout << "Generated address: " << address << dec << endl;

                if (address < MemorySize){
                    rom1Arr[address] = rom1control;
                    rom2Arr[address] = rom2control;
                }
            }
        }

        // Write control signals to ROM arrays based on block type item
        if (b.type == "item"){
            for (int addr : b.addresses){
                if (addr < MemorySize){
                    rom1Arr[addr] = rom1control;
                    rom2Arr[addr] = rom2control;
                }
                else{
                    cerr << "Address " << addr << " exceeds memory size\n";
                }
            }
        }

        // Write control signals to ROM arrays based on block type block
        if (b.type == "block"){
            if (b.startAddress < 0 || b.endAddress >= MemorySize || b.startAddress > b.endAddress)
            {
                std::cerr << "Invalid address range in block: "
                        << b.startAddress << " - " << b.endAddress << "\n";
            }
            else
            {
                for (int addr = b.startAddress; addr <= b.endAddress; addr++)
                {
                    rom1Arr[addr] = rom1control;
                    rom2Arr[addr] = rom2control;
                }
            }
        }
    }
    
    // ----------------------
    // Write ROM arrays to files
    // ----------------------
    rom1File.write(reinterpret_cast<char*>(rom1Arr), MemorySize);
    rom2File.write(reinterpret_cast<char*>(rom2Arr), MemorySize);

    rom1File.close();
    rom2File.close();
    jsonFile.close();

    return 0;
}

#endif