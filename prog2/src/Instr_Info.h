/*  On my honor, I have neither given nor received unauthorized aid on this assignment */
//
//  Instr_Info.h
//  MIPSsim
//
//  Created by Yuxing Han on 14-4-22.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#ifndef __MIPSsim__Instr_Info__
#define __MIPSsim__Instr_Info__

#include <stdio.h>
#include "utility.h"

class InstrParseInfo{
public:
    char* oper;
    REGISTER dst;
    REGISTER source1,source2;
    int imm;
    bool done; //is dst written? there exists a special case for "SW"
    
public:
    InstrParseInfo(){
        oper=NULL;
        dst=-1;
        source1=-1;
        source2=-1;
        imm=-1;
        done=false;
    }
    ~InstrParseInfo(){
        if(!oper)
            free(oper);
    }
    
    void parse_instr (const char* command){
        char* operation = (char*)malloc(sizeof(char)*10);
        REGISTER rs,rt,rd,base;
        unsigned int sa;
        int immediate,offset;
        sscanf(command, "%s", operation);
        
        oper = operation;
        done = false;
        if (strcmp(oper,"J") == 0 || strcmp(oper, "JR")==0 ||strcmp(oper, "BEQ")==0 ||
            strcmp(oper, "BLTZ")==0 || strcmp(oper, "BGTZ")==0 ||
            strcmp(oper, "NOP")==0 || strcmp(oper, "BREAK")==0) {
            return;
        }
        if (strcmp(oper, "LW") == 0) {
            sscanf(command, "%s R%u, %d(R%u)", oper, &rt, &offset, &base);
            dst = rt;
            source1 = base;
            imm = offset;
            return;
        }
        if (strcmp(oper, "SW") == 0) {
            sscanf(command, "%s R%u, %d(R%u)", oper, &rt, &offset, &base);
            source1 = base;
            source2 = rt;
            imm=offset;
            return;
        }
        if (strcmp(oper, "SLL")==0 || strcmp(oper, "SRL")==0 || strcmp(oper, "SRA")==0) {
            sscanf(command, "%s R%u, R%u, #%u", oper, &rd, &rt, &sa);
            dst = rd;
            source1 = rt;
            imm = sa;
            return;
        }
        if(strcmp(oper, "ADD")==0 || strcmp(oper, "SUB")==0 ||
           strcmp(oper, "MUL")==0 || strcmp(oper, "AND")==0 ||
           strcmp(oper, "OR")==0 || strcmp(oper, "XOR")==0 ||
           strcmp(oper, "NOR")==0 || strcmp(oper, "SLT")==0){
            sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
            dst = rd;
            source1 = rs;
            source2 = rt;
            return;
        }
        if(strcmp(oper, "ADDI")==0 || strcmp(oper, "ANDI")==0||
           strcmp(oper, "ORI")==0 || strcmp(oper, "XORI")==0){
            sscanf(command, "%s R%u, R%u, #%d", oper,&rt,&rs, &immediate);
            dst = rt;
            source1 = rs;
            imm = immediate;
            return;
        }
    }
    
};

#endif /* defined(__MIPSsim__Instr_Info__) */
