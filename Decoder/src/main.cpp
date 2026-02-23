/**
 * @file main.cpp
 * @brief main file for the decoder program.
 * 
 * @author R.J. Ockhuijsen
 * @date 2026-02-23
 * @version 0.1
 * 
 * @details
 * 
 */

#include "../include/main.h"


#ifndef MAIN_CPP
#define MAIN_CPP

using namespace std;

uint8_t rom1Arr[MemorySize]{0};
uint8_t rom2Arr[MemorySize]{0};

int main(int argc, char const *argv[])  
{
    ofstream rom1File("rom1.bin", ios::out | ios::binary);
    ofstream rom2File("rom2.bin", ios::out | ios::binary);

    if (!rom1File || !rom2File) {
        cerr << "Failed to open input file(s)\n";
        return 1;
    }

    rom1File.close();
    rom2File.close();
    return 0;
}

#endif