#include "../include/parser.h"
#include <sstream>
#include <algorithm>

using namespace std; // ✅ Use standard namespace

Instruction Parser::parseInstruction(const string& line) const {
    string cleaned = line;
    replace(cleaned.begin(), cleaned.end(), ',', ' '); // Replace commas with spaces

    istringstream iss(cleaned);
    vector<string> tokens;
    string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    Instruction inst;

    if (tokens.size() > 0) inst.opcode = tokens[0];
    if (tokens.size() > 1) inst.operand1 = tokens[1];
    if (tokens.size() > 2) inst.operand2 = tokens[2];

    return inst;
}
