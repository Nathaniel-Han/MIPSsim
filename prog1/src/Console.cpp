/*  On my honor, I have neither given nor received unauthorized aid on this assignment */
//
//  Console.cpp
//  MIPSsim
//
//  Created by Yuxing Han on 14-3-22.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#include "Console.h"

#define HYPHENS "--------------------"

#define CON_PRINT_FORMAT(type,x) "%d\t%d\t%d \t%d \t%d\t%d\t%d\t%d\n",\
                type[x],type[x+1],type[x+2],type[x+3], type[x+4],type[x+5],type[x+6],type[x+7]


Console::Console(int m_pc, int m_start_mem, int m_end_mem, const char* fname)
:PC(m_pc), start_mem(m_start_mem),end_mem(m_end_mem)
{
    cycle_count = 1;
    memset(reg, 0, sizeof(REGISTER)*REG_NUM);
    memset(mem, 0, sizeof(MEMORY)*MEM_SIZE);
    
    out = fopen(fname, "w");
}

Console::~Console()
{
    if (!out) {
        fclose(out);
    }
}

void Console::execute(const char *command)
{
    char oper[10];
    REGISTER rs,rt,rd,base;
    unsigned int instr,sa;
    int immediate,offset;
    sscanf(command, "%s", oper);
    //printf("%s\n",oper);
    //printf("PC=%d\n",PC);
    
    if(strcmp(oper, "J") == 0)
    {
        sscanf(command, "%s #%u", oper, &instr);
        simu_print(command);
        PC = instr;
        goto end;
    }
    if(strcmp(oper, "JR") == 0)
    {
        sscanf(command, "%s R%u", oper, &rs);
        simu_print(command);
        PC = reg[rs];
        goto end;
    }
    if(strcmp(oper, "BEQ") == 0)
    {
        sscanf(command, "%s R%u, R%u, #%d", oper, &rs,&rt,&offset);
        simu_print(command);
        PC = (reg[rs]==reg[rt]) ? PC+4+offset : PC + 4;
        goto end;
    }
    if (strcmp(oper, "BLTZ") == 0) {
        sscanf(command, "%s R%u, #%d", oper, &rs,&offset);
        simu_print(command);
        PC = (reg[rs] < 0)? PC+4+offset: PC + 4;
        goto end;
    }
    if (strcmp(oper, "BGTZ") == 0) {
        sscanf(command, "%s R%u, #%d",oper,&rs,&offset);
        simu_print(command);
        PC = (reg[rs] > 0) ? PC+4+offset : PC + 4;
        goto end;
    }
    
    if (strcmp(oper, "BREAK") == 0) {
        simu_print(command);
        PC = FLAG_COM_ADDR;
        return;
    }
    
    if (strcmp(oper, "SW") == 0) {
        sscanf(command, "%s R%u, %d(R%u)", oper, &rt, &offset, &base);
        mem[(reg[base]+offset)/4] = reg[rt];
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "LW") == 0) {
        sscanf(command, "%s R%u, %d(R%u)", oper, &rt, &offset, &base);
        reg[rt] = mem[(reg[base]+offset)/4];
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "SLL") == 0) {
        sscanf(command, "%s R%u, R%u, #%u", oper, &rd, &rt, &sa);
        reg[rd] = reg[rt] << sa;
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "SRL") == 0) {
        sscanf(command, "%s R%u, R%u, #%u", oper, &rd, &rt, &sa);
        reg[rd] = (unsigned)reg[rt] >>  sa;
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "SRA") == 0) {
        sscanf(command, "%s R%u, R%u, #%u", oper, &rd, &rt, &sa);
        reg[rd] = reg[rt] >> sa;
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "NOP") == 0) {
        simu_print(command);
        PC += 4;
        goto end;
    }
    
    
    if (strcmp(oper, "ADD") == 0) {
        sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
        reg[rd] = reg[rs] + reg[rt];
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "SUB") == 0) {
        sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
        reg[rd] = reg[rs] - reg[rt];
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "MUL") == 0) {
        sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
        reg[rd] = reg[rs] * reg[rt];
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "AND") == 0) {
        sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
        reg[rd] = reg[rs] & reg[rt];
        simu_print(command);
        PC += 4;
        goto end;
    }

    if (strcmp(oper, "OR") == 0) {
        sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
        reg[rd] = reg[rs] | reg[rt];
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "XOR") == 0) {
        sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
        reg[rd] = reg[rs] ^ reg[rt];
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "NOR") == 0) {
        sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
        reg[rd] = ~(reg[rs] | reg[rt]);
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "SLT") == 0) {
        sscanf(command, "%s R%u, R%u, R%u", oper,&rd,&rs,&rt);
        reg[rd] = (reg[rs] < reg[rt]) ? 1 : 0;
        simu_print(command);
        PC += 4;
        goto end;
    }
    if (strcmp(oper, "ADDI") == 0) {
        sscanf(command, "%s R%u, R%u, #%d", oper,&rt,&rs, &immediate);
        reg[rt] = reg[rs] + immediate;
        simu_print(command);
        PC += 4;
        goto end;
    }
    
    if (strcmp(oper, "ANDI") == 0) {
        sscanf(command, "%s R%u, R%u, #%d", oper,&rt,&rs, &immediate);
        reg[rt] = reg[rs] & immediate;
        simu_print(command);
        PC += 4;
        goto end;
    }
    
    if (strcmp(oper, "ORI") == 0) {
        sscanf(command, "%s R%u, R%u, #%d", oper,&rt,&rs, &immediate);
        reg[rt] = reg[rs] | immediate;
        simu_print(command);
        PC += 4;
        goto end;
    }
    
    if (strcmp(oper, "XORI") == 0) {
        sscanf(command, "%s R%u, R%u, #%d", oper,&rt,&rs, &immediate);
        reg[rt] = reg[rs] ^ immediate;
        simu_print(command);
        PC += 4;
        goto end;
    }
    
    
end:
    cycle_count++;
    return;

}

void Console::simu_print(const char *command)
{
    fprintf(out,HYPHENS);fprintf(out,"\n");
    fprintf(out,"Cycle:%d %d %s\n\n", cycle_count, PC, command);
    fprintf(out,"Registers\n");
    fprintf(out,"R00:\t");
    fprintf(out, CON_PRINT_FORMAT(reg, 0) );
    fprintf(out,"R08:\t");
    fprintf(out, CON_PRINT_FORMAT(reg, 8) );
    fprintf(out,"R16:\t");
    fprintf(out, CON_PRINT_FORMAT(reg, 16) );
    fprintf(out,"R24:\t");
    fprintf(out, CON_PRINT_FORMAT(reg, 24) );
    fprintf(out,"\nData\n");
    
    int tmp_mem = end_mem - (end_mem-start_mem+1)%8;
    
    for(int i=start_mem; i<tmp_mem;i=i+8)
    {
        // maybe need to modify
       fprintf(out,"%d:\t",i*4);
       fprintf(out, CON_PRINT_FORMAT(mem, i) ); 
    }
    
    
    if (tmp_mem != end_mem) {
        fprintf(out, "%d:\t", (tmp_mem+1)*4);
        for (int i=tmp_mem+1; i<=end_mem; i++) {
            fprintf(out,"%d\t", mem[i]);
        }
    }
    
    fprintf(out,"\n");
    
    return;
}

