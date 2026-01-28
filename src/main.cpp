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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
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



// ---------- OPERAND PARSER ----------
int parseOperand(const string& operand, int lineNumber) {
    if (operand.empty())
        return 0;

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

// ---------- LINE PARSER ----------
void parseLine(
    const string& line,
    int lineNumber,
    string& opcode,
    int& op1,
    int& op2
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

    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input_file> [output_file]\n";
        return 1;
    }
    if (!isRightExtention(argv[1])) {
    cerr << "ERROR: Input file must have .asm or .s extension\n";
    return 1;
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
            int op1, op2;

            parseLine(line, lineNumber, opcode, op1, op2);

            // UPPERCASE opcode + numeric operands only
            output << opcode << " "
                   << op1 << " "
                   << op2 << endl;

            lineNumber++;
        }
    } catch (const runtime_error& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }

    cout << "Parsed output written to: " << outputFile << endl;
    return 0;
}
