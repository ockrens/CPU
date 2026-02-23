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


#define MemorySize 524288

#endif