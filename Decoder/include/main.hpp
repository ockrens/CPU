/**
 * @file main.h
 * @brief header file of the main.cpp file.
 * 
 * @author R.J. Ockhuijsen
 * @date 2026-02-23
 * @version 0.1
 * 
 * @details
 * 
 */


 /* 
    read the opcodes from a fils so no recompile?

    these proposed functions are there to make changing how te encoding works easier in the long run.

    to write the right databits to the given adresses.
    function(start adress, adress count, opcode via struct for control signals).

    defines what cotrol singals need to be set for each instruction.
    function(opcode, control signals * n). 
        - control signal examples RD mode, RS mode and functional unit. all ig
 */


#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;

#define MemorySize 65536


struct Block
{
    string name;
    string type;              

    // block type
    int startAddress{};
    int endAddress{};

    // address type (dynamic)
    vector<int> addresses;

    // bittype
    string bitMode{};

    // control signals
    string RDMode{};
    string RSMode{};
    string FuncUnit{};
    uint8_t FuncData{};
    string FlagMode{};
    string PCMode{};
};

#endif