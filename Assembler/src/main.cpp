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

#include "../include/main.h"
#include "../include/inst_def.h"

using namespace std;

// ---------- UTILS ----------
string toUpper(string s) {
    for (char& c : s)
        c = toupper(static_cast<unsigned char>(c));
    return s;
}

string changeExtensionToBin(const string& filename) {
    size_t pos = filename.rfind('.');
    if (pos == string::npos) {
        // No extension, just append .bin
        return filename + ".bin";
    }
    return filename.substr(0, pos) + ".bin";
}

string toLower(const string& s) {
    string result = s;
    for (char& c : result) c = tolower(static_cast<unsigned char>(c));
    return result;
}

bool isRightExtention(const string& filename) {
    size_t pos = filename.rfind('.');
    if (pos == string::npos) return false;

    string ext = filename.substr(pos);
    ext = toLower(ext);  // convert to lowercase for case-insensitive match

    return (ext == ".asm" || ext == ".s");
}
const OpcodeFormat& findOpcodeFormat(const std::string& opcode, int lineNumber) {
    for (const auto& op : OPCODES) {
        if (op.opcode == opcode) {
            return op;
        }
    }

    throw std::runtime_error(
        "Line " + std::to_string(lineNumber) +
        ": Unknown opcode '" + opcode + "'"
    );
}
// op1 = rs    op2 = rd
uint16_t encodeInstruction(const OpcodeFormat& op, optional<int> op1, optional<int> op2) {
    uint16_t inst = 0x0000;
    if (op.field0)
        inst |= (static_cast<uint16_t>(*op.field0 & FMask) <<Field0Off);

    if (op1.has_value())
        inst |= (static_cast<uint16_t>(op1.value() & FMask) << Field1Off);
    else{
        if (op.field1)
        inst |= (static_cast<uint16_t>(*op.field1 & FMask) << Field1Off);
    }
    if (op2.has_value())
        if (op.RS == RSMode::imm8)        
            inst |= (static_cast<uint16_t>(op2.value() & FMask8) << Field2Off);
        else
            inst |= (static_cast<uint16_t>(op2.value() & FMask) << Field2Off);
    else{
        if (op.field2)
        inst |= (static_cast<uint16_t>(*op.field2 & FMask) << Field2Off);
    }
    if (op.RS != RSMode::imm8)
        if (op.field3)
            inst |= (static_cast<uint16_t>(*op.field3 & FMask) << Field3Off);

    return inst;
}


optional<int> parseOperand(const string& operand, int lineNumber) {
    if (operand.empty())
        return nullopt;

    // REGISTER: R<number>
    if (operand[0] == 'R' && operand.size() > 1) {
        for (size_t i = 1; i < operand.size(); i++) {
            if (!isdigit(operand[i])) {
                throw runtime_error(
                    "Line " + to_string(lineNumber) +
                    ": Invalid register '" + operand + "'"
                );
            }
        }
        return stoi(operand.substr(1));
    }

    // IMMEDIATE: number
    size_t pos = 0;
    try {
        int value = stoi(operand, &pos);
        if (pos != operand.size())
            throw runtime_error("");
        return value;
    } catch (...) {
        throw runtime_error(
            "Line " + to_string(lineNumber) +
            ": Invalid operand '" + operand +
            "' (not register or immediate)"
        );
    }
}

void parseLine(
    const string& line,
    int lineNumber,
    string& opcode,
    optional<int>& op1,
    optional<int>& op2
) {
    string cleaned = line;
    for (char& c : cleaned)
        if (c == ',') c = ' ';

    string rawOpcode, t1, t2;
    stringstream ss(cleaned);

    ss >> rawOpcode >> t1 >> t2;

    if (rawOpcode.empty()) {
        throw runtime_error(
            "Line " + to_string(lineNumber) + ": Missing opcode"
        );
    }

    opcode = toUpper(rawOpcode);
    op1 = parseOperand(toUpper(t1), lineNumber);
    op2 = parseOperand(toUpper(t2), lineNumber);
}

// ---------- MAIN ----------
int main(int argc, char* argv[]) {
    bool debug = false;
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input_file> [output_file]\n";
        return 1;
    }
    if (!isRightExtention(argv[1])) {
        cerr << "ERROR: Input file must have .asm or .s extension\n";
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-d") {
            debug = true;
        }
    }
    ifstream input(argv[1]);
    if (!input) {
        cerr << "Failed to open input file\n";
        return 1;
    }

    string outputFile = (argc >= 3) ? argv[2] : changeExtensionToBin(argv[1]);
    ofstream output(outputFile);

    string line;
    int lineNumber = 1;

    try {
        while (getline(input, line)) {
            if (line.empty()) {
                lineNumber++;
                continue;
            }

            string opcode;
            optional<int> op1, op2;

            parseLine(line, lineNumber, opcode, op1, op2);
            const OpcodeFormat& fmt = findOpcodeFormat(opcode, lineNumber);
            
            uint16_t instruction = encodeInstruction(fmt, op1, op2);
            if(debug == true)
                cout << opcode << " " << bitset<16>(instruction) << " " << hex << instruction << endl;
            output.write(reinterpret_cast<const char*>(&instruction), sizeof(instruction));

            lineNumber++;
        }
    } catch (const runtime_error& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }

    cout << "Parsed output written to: " << outputFile << endl;
    return 0;
}
#endif