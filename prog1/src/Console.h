/*  On my honor, I have neither given nor received unauthorized aid on this assignment */
//
//  console.h
//  MIPSsim
//
//  Created by Yuxing Han on 14-3-22.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#ifndef MIPSsim_console_h
#define MIPSsim_console_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include "utility.h"
using std::map;



class Console{
private:
    int reg[REG_NUM];
    int mem[MEM_SIZE];
    MEMORY start_mem;
    MEMORY end_mem;
    int cycle_count;
    FILE *out;
    Console(){};
public:
    int PC;
    Console(int m_pc, int m_start_mem, int m_end_start,const char* fname);
    ~Console();
    
    void execute(const char* command);
    
    inline void memory_set(MEMORY addr, int value)
    {
        mem[addr] = value;
    }
    
    
    
    void internal_show()
    {
        printf("REG SHOW\n");
        for (int i=0; i<REG_NUM; i++) {
            printf("R[%d]=%d ",i, reg[i]);
        }
        printf("\n");
//        printf("\nMEM SHOW\n");
//        for (int i=start_mem; i<end_mem; i++) {
//            printf("MEM[%d]=%d\n", i*4, mem[i]);
//        }
    }
    
    void simu_print(const char *command);
    
    
};

#endif
