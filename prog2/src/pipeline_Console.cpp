/*  On my honor, I have neither given nor received unauthorized aid on this assignment */
//
//  pipeline_Console.cpp
//  MIPSsim
//
//  Created by Yuxing Han on 14-4-19.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#include "pipeline_Console.h"


pipeline_Console::pipeline_Console(int m_pc, int m_start_mem, int m_end_mem, const char* fname)
:PC(m_pc), start_mem(m_start_mem),end_mem(m_end_mem)
{
    cycle_count = 1;
    stalled_instr = -1;
    executed_str = -1;
    isFinished = false;
    IF_isStalled = false;
    label = 0;
    finished_instr = -1;
    finished_LW_instr = -1;
    
    left_slots = PRE_ISSUE_SIZE;
    memset(reg, 0, sizeof(REGISTER)*REG_NUM);
    memset(mem, 0, sizeof(MEMORY)*MEM_SIZE);
    memset(RegStatus, None, sizeof(int)*REG_NUM);
    memset(pre_issue_queue, -1, sizeof(int)*PRE_ISSUE_SIZE);
    memset(post_alu2_buffer, -1, sizeof(int)*3);
    memset(pre_mem_buffer, -1, sizeof(int)*3);
    memset(post_mem_buffer, -1, sizeof(int)*3);
    memset(written_reg_counts, 0 , sizeof(int)*REG_NUM);
    
    
    
    out = fopen(fname, "w");
}

pipeline_Console::~pipeline_Console()
{
    if (!out) {
        fclose(out);
    }
}


void pipeline_Console::process(Disassembler& disassembler){
    WB_step();
    mem_step();
    exe_step();
    issue_step();
    IF_step(disassembler);
}

//check whether the previous stores are issued
//to make sure load instrucion wait to be issued until all the previous stores are issued
//and the store instruction to be issued in order
bool pipeline_Console::SW_issue_check(int index){
    for (int prev=0; prev<index; prev++) {
        if (strcmp(Instr_Infos[pre_issue_queue[prev]].oper, "SW") ==0) {
            return true;
        }
    }
    return false;
}

void pipeline_Console::IF_step(Disassembler& disassembler)
{
    int current_IF_num = 0;
    
    // for instruction parsing
    unsigned int instr;
    REGISTER rs,rt;
    int offset;
    char oper[10];
    char* command = NULL;
    
    
    if (!isFinished){
        
        if (stalled_instr != -1) {
            assert(IF_isStalled == true);
            command = Instructions[stalled_instr];
            sscanf(command, "%s", oper);
            if (strcmp(oper, "JR") ==0) {
                sscanf(command, "%s R%u", oper, &rs);
                if (RegStatus[rs] == None) {
                    PC = reg[rs];
                    IF_isStalled = false;
                    executed_str = stalled_instr;
                    stalled_instr = -1;
                }
            }// "JR"
            else if (strcmp(oper, "BEQ")==0) {
                sscanf(command, "%s R%u, R%u, #%d", oper, &rs,&rt,&offset);
                if (RegStatus[rs] == None && RegStatus[rt] == None) {
                    PC = (reg[rs]==reg[rt]) ? PC+4+offset : PC + 4;
                    IF_isStalled = false;
                    executed_str = stalled_instr;
                    stalled_instr = -1;
                }
            }// "BEQ"
            else if (strcmp(oper, "BLTZ")==0){
                sscanf(command, "%s R%u, #%d", oper, &rs,&offset);
                if(RegStatus[rs] == None){
                    PC = (reg[rs] < 0)? PC+4+offset: PC + 4;
                    IF_isStalled = false;
                    executed_str = stalled_instr;
                    stalled_instr = -1;
                }
            }// "BLTZ"
            else if(strcmp(oper, "BGTZ")==0){
                sscanf(command, "%s R%u, #%d", oper, &rs,&offset);
                if(RegStatus[rs] == None){
                    PC = (reg[rs] > 0) ? PC+4+offset : PC + 4;
                    IF_isStalled = false;
                    executed_str = stalled_instr;
                    stalled_instr = -1;
                }
            }// "BGTZ"
        }
        // (IF) step is not stalled
        else{
            executed_str = -1;
            while (current_IF_num < left_slots && current_IF_num < IF_SIZE) {
                command = disassembler.result[PC];
                sscanf(command, "%s", oper);
                
                Instructions.push_back(command);
                InstrParseInfo instr_parse_info;
                instr_parse_info.parse_instr(command);
                Instr_Infos.push_back(instr_parse_info);
                
                if (strcmp(oper, "J") == 0) {
                    sscanf(command, "%s #%u", oper, &instr);
                    PC = instr;
                    executed_str = label;
                    label++;
                    break;
                }// "J"
                else if(strcmp(oper, "JR") == 0){
                    sscanf(command, "%s R%u", oper, &rs);
                    if (RegStatus[rs] != None) {
                        IF_isStalled = true;
                        stalled_instr = label;
                    }else{
                        PC = reg[rs];
                        executed_str = label;
                    }
                    label++;
                    break;
                }// "JR"
                else if (strcmp(oper, "BEQ") == 0){
                    sscanf(command, "%s R%u, R%u, #%d", oper, &rs,&rt,&offset);
                    if (RegStatus[rs] != None || RegStatus[rt] != None) {
                        IF_isStalled = true;
                        stalled_instr = label;
                    } else{
                        PC = (reg[rs]==reg[rt]) ? PC+4+offset : PC + 4;
                        executed_str = label;
                    }
                    label++;
                    break;
                }// "BEQ"
                else if (strcmp(oper, "BLTZ") == 0){
                    sscanf(command, "%s R%u, #%d", oper, &rs,&offset);
                    if(RegStatus[rs] != -1){
                        IF_isStalled = true;
                        stalled_instr = label;
                    } else{
                        PC = (reg[rs] < 0)? PC+4+offset: PC + 4;
                        executed_str = label;
                    }
                    label++;
                    break;
                }// "BLTZ"
                else if (strcmp(oper, "BGTZ") == 0){
                    sscanf(command, "%s R%u, #%d", oper, &rs,&offset);
                    if(RegStatus[rs] != -1){
                        IF_isStalled = true;
                        stalled_instr = label;
                    } else{
                        PC = (reg[rs] > 0) ? PC+4+offset : PC + 4;
                        executed_str = label;
                    }
                    label++;
                    break;
                }// "BGTZ"
                else if (strcmp(oper, "NOP") == 0){
                    executed_str = label;
                    PC += 4;
                    label++;
                }// "NOP"
                else if (strcmp(oper, "BREAK") == 0){
                    executed_str = label;
                    label++;
                    isFinished = true;
                    break;
                }// "BREAK"
                else{
                    PC += 4;
                    pre_issue_queue_add(label);
                    label++;
                }// other instructions
                
                current_IF_num++;
            }
            
        }
        
        // finished operations in WB operation, because in this implementation of pipeline
        // the order of processing instructions is reverse
        for (int i=0; i<REG_NUM; i++) {
            if (written_reg_counts[i]==1) {
                RegStatus[i] = None;
                written_reg_counts[i]=0;
                assert(finished_instr!=-1 || finished_LW_instr!=-1);
                if (finished_instr!=-1) {
                    Instr_Infos[finished_instr].done=true;
                }
                if (finished_LW_instr!=-1) {
                    Instr_Infos[finished_LW_instr].done=true;
                }
                
            }
        }
        
        left_slots = PRE_ISSUE_SIZE - pre_issue_queue_size();
    }
    
    
}

void pipeline_Console::issue_step(){
    char* command = NULL;
    char oper[10];
    int size = pre_issue_queue_size();
    bool is_mem_issued = false;
    bool is_alu_issued = false;
    
    for (int i=0;i<size;i++){
        int instr_label = pre_issue_queue[i];
        command = Instructions[instr_label];
        sscanf(command, "%s", oper);
        
        if (!is_mem_issued && strcmp(oper, "LW")==0) {
            if (pre_alu1_queue.size() < PRE_ALU1_SIZE) {
                int dst = Instr_Infos[instr_label].dst;
                int read_reg = Instr_Infos[instr_label].source1;
                if (!RAW_hazard_check(read_reg,instr_label) &&
                    !WAW_hazard_check(dst, instr_label) &&
                    !WAR_hazard_check(dst, instr_label) && !SW_issue_check(i)) {
                    pre_issue_queue_delete(i);
                    pre_alu1_queue.push(instr_label);
                    is_mem_issued=true;
                }
                
            }
            continue;
        }
        
        if (!is_mem_issued && strcmp(oper, "SW") == 0) {
            if (pre_alu1_queue.size() < PRE_ALU1_SIZE) {
                int read_reg1 = Instr_Infos[instr_label].source1;
                int read_reg2 = Instr_Infos[instr_label].source2;
                if (!RAW_hazard_check(read_reg1, instr_label) &&
                    !RAW_hazard_check(read_reg2, instr_label) && !SW_issue_check(i)) {
                    pre_issue_queue_delete(i);
                    pre_alu1_queue.push(instr_label);
                    is_mem_issued=true;
                }
            }
            continue;
        }
        
        if(!is_alu_issued &&
           (strcmp(oper, "ADD")==0 || strcmp(oper, "SUB")==0 ||
            strcmp(oper, "MUL")==0 || strcmp(oper, "AND")==0 ||
            strcmp(oper, "OR")==0 || strcmp(oper, "XOR")==0 ||
            strcmp(oper, "NOR")==0 || strcmp(oper, "SLT")==0)){
               if (pre_alu2_queue.size() < PRE_ALU2_SIZE) {
                   int dst = Instr_Infos[instr_label].dst;
                   int read_reg1 = Instr_Infos[instr_label].source1;
                   int read_reg2 = Instr_Infos[instr_label].source2;
                   assert(dst !=-1 && read_reg1 != -1 && read_reg2 != -1);
                   
                   if (!RAW_hazard_check(read_reg1,instr_label) && !RAW_hazard_check(read_reg2, instr_label) &&
                       !WAW_hazard_check(dst, instr_label) && !WAR_hazard_check(dst, instr_label)) {
                       //issue this instruction****************
                       RegStatus[dst] = ALU2;
                       pre_issue_queue_delete(i);
                       pre_alu2_queue.push(instr_label);
                       is_alu_issued=true;
                       //==issue this instruction***************
                   }
               }
               
               continue;
           }
        
        if(!is_alu_issued &&
           (strcmp(oper, "SLL")==0 || strcmp(oper, "SRL")==0 || strcmp(oper, "SRA")==0 ||
            strcmp(oper, "ADDI")==0 || strcmp(oper, "ANDI")==0||
            strcmp(oper, "ORI")==0 || strcmp(oper, "XORI")==0)){
               if (pre_alu2_queue.size() < PRE_ALU2_SIZE) {
                   int dst = Instr_Infos[instr_label].dst;
                   int read_reg = Instr_Infos[instr_label].source1;
                   assert(dst != -1 && read_reg != -1);
                   
                   if (!RAW_hazard_check(read_reg, instr_label) &&
                       !WAW_hazard_check(dst, instr_label) && !WAR_hazard_check(dst, instr_label)) {
                       RegStatus[dst]=ALU2;
                       pre_issue_queue_delete(i);
                       pre_alu2_queue.push(instr_label);
                       is_alu_issued=true;
                   }
               }
               continue;
           }
        
    }
    
    
    pre_issue_queue_reorganize();
}

void pipeline_Console::exe_step(){
    //ALU1 execution****************************************************
    if (!pre_alu1_queue.empty()) {
        int instr_label = pre_alu1_queue.front(); pre_alu1_queue.pop();
        pre_mem_buffer[0] = instr_label;
        char *oper = Instr_Infos[instr_label].oper;
        
        
        if (strcmp(oper, "LW") == 0) {
            pre_mem_buffer[1] = Instr_Infos[instr_label].dst;
            //result is the memory address
            pre_mem_buffer[2] = (reg[Instr_Infos[instr_label].source1] + Instr_Infos[instr_label].imm)/4;
            goto ALU2;
        }
        if (strcmp(oper, "SW") == 0) {
            //pre_mem_buffer[1] is the reference registr
            pre_mem_buffer[1] = Instr_Infos[instr_label].source2;
            //result is the memory address
            pre_mem_buffer[2] = (reg[Instr_Infos[instr_label].source1] + Instr_Infos[instr_label].imm)/4;
            //special case for "SW", because already read the register
            Instr_Infos[instr_label].done=true;
            goto ALU2;
        }
    }
    //==ALU1 execution****************************************************
    
    
    //ALU2 execution****************************************************
ALU2:
    if (!pre_alu2_queue.empty()) {
        
        int instr_label = pre_alu2_queue.front(); pre_alu2_queue.pop();
        post_alu2_buffer[0]=instr_label;
        post_alu2_buffer[1]=Instr_Infos[instr_label].dst;
        int read_reg1,read_reg2,imm;
        char *oper = Instr_Infos[instr_label].oper;
        assert(oper!=NULL);
        if (strcmp(oper, "ADD")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            read_reg2 = Instr_Infos[instr_label].source2;
            post_alu2_buffer[2]=reg[read_reg1]+reg[read_reg2];
            return;
        }
        if (strcmp(oper, "SUB")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            read_reg2 = Instr_Infos[instr_label].source2;
            post_alu2_buffer[2]=reg[read_reg1]-reg[read_reg2];
            return;
        }
        if (strcmp(oper, "MUL")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            read_reg2 = Instr_Infos[instr_label].source2;
            post_alu2_buffer[2]=reg[read_reg1]*reg[read_reg2];
            return;
        }
        if (strcmp(oper, "AND")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            read_reg2 = Instr_Infos[instr_label].source2;
            post_alu2_buffer[2]=reg[read_reg1]&reg[read_reg2];
            return;
        }
        if (strcmp(oper, "OR")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            read_reg2 = Instr_Infos[instr_label].source2;
            post_alu2_buffer[2]=reg[read_reg1]|reg[read_reg2];
            return;
        }
        if (strcmp(oper, "XOR")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            read_reg2 = Instr_Infos[instr_label].source2;
            post_alu2_buffer[2]=reg[read_reg1]^reg[read_reg2];
            return;
        }
        if (strcmp(oper, "NOR")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            read_reg2 = Instr_Infos[instr_label].source2;
            post_alu2_buffer[2]=~(reg[read_reg1]|reg[read_reg2]);
            return;
        }
        if (strcmp(oper, "SLL")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            imm = Instr_Infos[instr_label].imm;
            post_alu2_buffer[2]=reg[read_reg1] << imm;
            return;
        }
        if (strcmp(oper, "SRL")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            imm = Instr_Infos[instr_label].imm;
            post_alu2_buffer[2]=(unsigned)reg[read_reg1] >> imm;
            return;
        }
        if (strcmp(oper, "SRA")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            imm = Instr_Infos[instr_label].imm;
            post_alu2_buffer[2]=reg[read_reg1] >> imm;
            return;
        }
        if (strcmp(oper, "ADDI")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            imm = Instr_Infos[instr_label].imm;
            post_alu2_buffer[2]=reg[read_reg1] + imm;
            return;
        }
        if (strcmp(oper, "ANDI")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            imm = Instr_Infos[instr_label].imm;
            post_alu2_buffer[2]=reg[read_reg1] & imm;
            return;
        }
        if (strcmp(oper, "ORI")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            imm = Instr_Infos[instr_label].imm;
            post_alu2_buffer[2]=reg[read_reg1] | imm;
            return;
        }
        if (strcmp(oper, "XORI")==0) {
            read_reg1 = Instr_Infos[instr_label].source1;
            imm = Instr_Infos[instr_label].imm;
            post_alu2_buffer[2]=reg[read_reg1] ^ imm;
            return;
        }
    }
    //==ALU2 execution****************************************************
}

void pipeline_Console::mem_step(){
    if (pre_mem_buffer[0]!=-1) {
        int instr_label = pre_mem_buffer[0];
        if (strcmp(Instr_Infos[instr_label].oper,"LW") == 0) {
            post_mem_buffer[0]=pre_mem_buffer[0];
            post_mem_buffer[1]=pre_mem_buffer[1];
            post_mem_buffer[2]=mem[pre_mem_buffer[2]];
        }
        else{//  "SW" instruction
            assert(strcmp(Instr_Infos[instr_label].oper,"SW") == 0);
            mem[pre_mem_buffer[2]] = reg[pre_mem_buffer[1]];
            Instr_Infos[instr_label].done = true;
        }
        
        pre_mem_buffer[0] = -1;
    }
}

void pipeline_Console::WB_step(){
    
    if (post_mem_buffer[0] != -1) {
        int instr_label = post_mem_buffer[0];
        int dst = post_mem_buffer[1];
        
        reg[dst] = post_mem_buffer[2];
        
        assert(written_reg_counts[dst] == 0);//otherwise a WAW harzard
        written_reg_counts[dst] = 1;
        finished_LW_instr = instr_label;
        
        post_mem_buffer[0] = -1;
    }
    
    if (post_alu2_buffer[0]!=-1) {
        int instr_label = post_alu2_buffer[0];
        int dst = post_alu2_buffer[1];
        
        reg[dst] = post_alu2_buffer[2];
        
        assert(written_reg_counts[dst] == 0);//otherwise a WAW harzard
        written_reg_counts[dst] = 1;
        finished_instr = instr_label;
        
        post_alu2_buffer[0]=-1;
    }
    
}

void pipeline_Console::simu_print()
{
    fprintf(out,HYPHENS);fprintf(out,"\n");
    fprintf(out,"Cycle:%d\n\n", cycle_count);
    fprintf(out,"IF Unit:\n");
    fprintf(out,"\tWaiting Instruction:");
    if (stalled_instr != -1) {
        fprintf(out," [%s]\n", Instructions[stalled_instr]);
    }else{fprintf(out,"\n");}
    
    fprintf(out,"\tExecuted Instruction:");
    if (executed_str != -1) {
        fprintf(out," [%s]\n", Instructions[executed_str]);
    }else{fprintf(out,"\n");}
    
    // print Info of Pre-Issue Queue
    fprintf(out,"Pre-Issue Queue:\n");
    int size = pre_issue_queue_size();
    for (int i=0; i<size; i++) {
        fprintf(out,"\tEntry %d: [%s]\n",i, Instructions[pre_issue_queue[i]]);
    }
    for (int i=size; i<PRE_ISSUE_SIZE; i++) {
        fprintf(out,"\tEntry %d:\n", i);
    }
    // ====== print Info of Pre-Issue Queue
    int i=0;
    
    queue<int> temp_pre_alu1_queue(pre_alu1_queue);
    fprintf(out,"Pre-ALU1 Queue:\n");
    while (!temp_pre_alu1_queue.empty()) {
        fprintf(out,"\tEntry %d: [%s]\n",i, Instructions[temp_pre_alu1_queue.front()]);
        temp_pre_alu1_queue.pop();
        i++;
    }
    for (int j=i; j<PRE_ALU1_SIZE; j++) {
        fprintf(out,"\tEntry %d:\n", j);
    }
    
    i=0;
    fprintf(out,"Pre-MEM Queue:");
    if (pre_mem_buffer[0] != -1) {
        fprintf(out," [%s]\n", Instructions[pre_mem_buffer[0]]);
    }else{fprintf(out,"\n");}
    
    
    fprintf(out,"Post-MEM Queue:");
    if (post_mem_buffer[0] != -1) {
        fprintf(out," [%s]\n", Instructions[post_mem_buffer[0]]);
    }else{fprintf(out,"\n");}
    
    queue<int> temp_pre_alu2_queue(pre_alu2_queue);
    fprintf(out,"Pre-ALU2 Queue:\n");
    
    while (!temp_pre_alu2_queue.empty()) {
        
        fprintf(out,"\tEntry %d: [%s]\n",i, Instructions[temp_pre_alu2_queue.front()]);
        temp_pre_alu2_queue.pop();
        i++;
    }
    for (int j=i; j<PRE_ALU2_SIZE; j++) {
        fprintf(out,"\tEntry %d:\n", j);
    }
    
    fprintf(out,"Post-ALU2 Queue:");
    if (post_alu2_buffer[0]!=-1) {
        fprintf(out," [%s]\n\n", Instructions[post_alu2_buffer[0]]);
    }else{fprintf(out,"\n\n");}
    
    fprintf(out,"Registers\n");
    fprintf(out,"R00:\t");
    fprintf(out, CON_PRINT_FORMAT(reg, 0) );
    fprintf(out,"R08:\t");
    fprintf(out, CON_PRINT_FORMAT(reg, 8) );
    fprintf(out,"R16:\t");
    fprintf(out,CON_PRINT_FORMAT(reg, 16) );
    fprintf(out,"R24:\t");
    fprintf(out,CON_PRINT_FORMAT(reg, 24) );fprintf(out,"\n");
    fprintf(out,"Data\n");
    int tmp_mem = end_mem - (end_mem-start_mem+1)%8;
    
    for(int i=start_mem; i<tmp_mem;i=i+8)
    {
        // maybe need to modify
        fprintf(out,"%d:\t",i*4);
        fprintf(out,CON_PRINT_FORMAT(mem, i) );
    }
    
    
    if (tmp_mem != end_mem) {
        fprintf(out,"%d:\t", (tmp_mem+1)*4);
        for (int i=tmp_mem+1; i<=end_mem; i++) {
            fprintf(out,"%d\t", mem[i]);
        }
    }
    //    fprintf(out,"\n");
    
}