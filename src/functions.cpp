#include "../include/parser.h"
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std; // Use standard namespace

bool isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}



static Opcode stringToOpcode(const string& opcodeStr) {
    string op = opcodeStr;
    transform(op.begin(), op.end(), op.begin(), ::toupper);

    if (op == "ADD") return Opcode::ADD;
    if (op == "SUB") return Opcode::SUB;
    if (op == "MOV") return Opcode::MOV;
    if (op == "LOAD") return Opcode::LOAD;
    if (op == "STORE") return Opcode::STORE;
    if (op == "JMP") return Opcode::JMP;
    if (op == "CMP") return Opcode::CMP;
    if (op == "NOP") return Opcode::NOP;
    return Opcode::INVALID;
}



static Register stringToRegister(const string& regStr) {
    string reg = regStr;
    transform(reg.begin(), reg.end(), reg.begin(), ::toupper);

    if (reg == "R0") return Register::R0;
    if (reg == "R1") return Register::R1;
    if (reg == "R2") return Register::R2;
    if (reg == "R3") return Register::R3;
    if (reg == "R4") return Register::R4;
    if (reg == "R5") return Register::R5;
    if (reg == "R6") return Register::R6;
    return Register::INVALID;
}

bool isALUOpcode (const string& opcodeStr){
    string op = opcodeStr;
    transform(op.begin(), op.end(), op.begin(), ::toupper);

    if (
    op == "ADD" ||
    op == "SUB" ||
    op == "SHL"
    ) return true;
    return false;

}



Instruction Parser::parseInstruction(const string& line)const {
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



DecodedInstruction Parser::decodeInstruction(const Instruction& inst) const{
    DecodedInstruction decodeparts;
    // make it so it this funtion spilts every possible parts, and gives the specific values to each part
    // make the values already bitshifted to the right place when decoding
    // and add some error handling
    /*
    */
   // opcode conversion
    decodeparts.opcodeName = inst.opcode;
    Opcode op = stringToOpcode(inst.opcode);

   // destination register check
    Register reg1 = stringToRegister(inst.operand1);
    if (reg1 == Register::INVALID)
        throw invalid_argument("Error: Unknown register '" + inst.operand1 + "'");
    decodeparts.regDest = static_cast<uint8_t>(reg1);

    // immediate / source register check
    Register reg2 = stringToRegister(inst.operand2);
    bool immIsNumber = isNumber(inst.operand2);
    
    if(immIsNumber == true && reg2 == Register::INVALID){

        decodeparts.isImmediate = true;
        int immedateValue = stoi(inst.operand2);

        if (immedateValue < 0 || immedateValue > 255){
            throw invalid_argument("Error: Immediate value out of range (0-255): " + inst.operand2);
        }
        else{
            if (immedateValue < 0 || immedateValue > 15) decodeparts.fitInImm4 = true;
            decodeparts.immediate_8 = (static_cast<uint8_t>(stoi(inst.operand2))) << 8;
            decodeparts.immediate_8 = (static_cast<uint8_t>(stoi(inst.operand2))) << 12;
        }
        
    }

    else if (reg2 == Register::INVALID)
        throw invalid_argument("Error: Unknown register '" + inst.operand2 + "'");

    else
        decodeparts.regSrc = static_cast<uint8_t>(reg2);

    return decodeparts;
    
}
// assemble the different parts into binary code
// has some random test code, missing some error handling and the rest of the opcodes
int Parser::encodeBinary(const DecodedInstruction& decoded) const{
 /*         done(*) working on(!) not started(#)
-> mov #
-> jmp #
-> nop #
-> hlt #
-> ldi #
-> jmpr #
-> add !
-> addc #
-> sub #
-> subc #
-> shl #
-> shr #
-> shlc #
-> shrc #
-> shra #
-> not #
-> neg #
-> and #
-> or #
-> xor #
-> fmov #
 
 */
    switch (stringToOpcode(decoded.opcodeName)) {
        case Opcode::ADD: {
            cout << decoded.opcode << endl;
            cout << decoded.regDest << endl;

            if (decoded.isImmediate)
                cout << decoded.immediate_8 << endl;
                        
            else {
                cout << decoded.regSrc << endl;
            }

            break;
        }


        case Opcode::SUB:   
        break;

        case Opcode::MOV:   
        break;

        default: 
            throw invalid_argument("Error: Unknown opcode '" + decoded.opcodeName + "'");
        break;
    }
    return 0;
}
