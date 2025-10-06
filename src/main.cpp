/**
 * @file main.cpp
 * @brief Entry point for the CPU Assembler program.
 *
 * This program implements a simple assembler that translates
 * assembly instructions into machine code for a simulated CPU.
 * 
 * @details
 * The assembler reads input from a source file, parses the instructions,
 * validates syntax, and outputs the corresponding binary code.
 * 
 * @version 0.1.0
 * @date 2025-10-06
 * @author R.J. Ockhuijsen
 */
/**
 * @brief to do list:
 * make a dynamic way for opening files
 * make a way to use the parsed data an split in in the right opcodes and handle errors of its not a correct instruction 
 * make it return the binary code
 * enmums for all the different registers and alu opcodes
 * write a output file with the binary code
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../include/parser.h"
using namespace std;
/**
 * @brief Main entry point of the CPU Assembler.
 * 
 * @return int Exit status code.
 */


int main() {
      Parser parser;
    cout << "CPU Assembler v0.1.0 starting..." << endl;


string myText;


// Read from the text file
fstream InputFile("codetest.asm");
while(getline(InputFile, myText)){ // reads a line from the file
cout << myText << endl;

Instruction inst = parser.parseInstruction(myText);
    cout << "Mnemonic: " << inst.opcode << "\n";
    cout << "Operand 1: " << inst.operand1 << "\n";
    cout << "Operand 2: " << inst.operand2 << "\n";
}
InputFile.close();
return 0;
}