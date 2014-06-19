/*  On my honor, I have neither given nor received unauthorized aid on this assignment */
//
//  pipeline_Console.h
//  MIPSsim
//
//  Created by Yuxing Han on 14-4-19.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#ifndef __MIPSsim__pipeline_Console__
#define __MIPSsim__pipeline_Console__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <queue>
#include "utility.h"
#include "Disassembler.h"
#include "Instr_Info.h"
using std::vector;
using std::queue;


class pipeline_Console {
private:
    int PC;
    int reg[REG_NUM];
    int mem[MEM_SIZE];
    MEMORY start_mem;
    MEMORY end_mem;
    FILE *out;
    pipeline_Console(){};
    void IF_step(Disassembler& disassembler);
    void issue_step();
    void exe_step();
    void mem_step();
    void WB_step();
    bool SW_issue_check(int index);
    
    enum FuncUnit{
        None=-1, MEM, ALU1, ALU2
    };
    
public:
    //instruction label
    int label;
    // process terminate flag
    bool isFinished;
    //IF stage stall flag
    bool IF_isStalled;
    // clock cycle count
    int cycle_count;
    
    //check at the end of last cycle, how many slots pre-issue queue has?
    int left_slots;
    //store original instruction information
    vector<char*> Instructions;
    //store parsed instruction information
    vector<InstrParseInfo> Instr_Infos;
    //stalled branch instruction in IF step; print for IF's waiting instruction
    int stalled_instr;
    //print for IF's executed instruction
    int executed_str;
    // size : 4
    int pre_issue_queue[PRE_ISSUE_SIZE];
    // size : 2
    queue<int> pre_alu1_queue;
    // size : 2
    queue<int> pre_alu2_queue;
    
    //index[0] for instr_label, index[1] for dst register, index[2] for result
    int post_alu2_buffer[3];
    int pre_mem_buffer[3];
    int post_mem_buffer[3];
    
    //handle the problem that any FUs cannot obtain the new register
    //values written by WB in the same cycle
    int written_reg_counts[REG_NUM];
    int finished_instr, finished_LW_instr;
    
    // Indicate each register will be written by which functional unit.
    FuncUnit RegStatus[REG_NUM];
    
    
public:
    pipeline_Console(int m_pc, int m_start_mem, int m_end_start,const char* fname);
    ~pipeline_Console();
    
    void process(Disassembler& disassembler);
    void simu_print();
    
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
    }
    
    
    //Harzard Check ***********************************************************
    bool RAW_hazard_check(REGISTER rs, int instr_label){
        //return true if there exists a RAW Hazard
        for (int i=0; i<instr_label; i++) {
            if (Instr_Infos[i].done==false && Instr_Infos[i].dst == rs) {
                return true;
            }
        }
        return false;
    }
    bool WAW_hazard_check(REGISTER rs, int instr_label){
        //return true if there exists a WAW Hazard
        assert(instr_label< Instr_Infos.size());
        for (int i=0; i<instr_label; i++) {
            if ( Instr_Infos[i].done == false && Instr_Infos[i].dst == rs) {
                return true;
            }
        }
        return false;
    }
    bool WAR_hazard_check(REGISTER rs, int instr_label){
        //return true if there exists a WAR Hazard
        assert(instr_label < Instr_Infos.size());
        for (int i=0; i<instr_label; i++) {
            if(Instr_Infos[i].done == false &&
               (Instr_Infos[i].source1 == rs || Instr_Infos[i].source2 ==rs))
                return true;
            }
        return false;
    }
    //===Harzard Check ***********************************************************
    
    // Tools for pre_issue_queue ********************************
    bool is_pre_issue_queue_full(){
        return pre_issue_queue[PRE_ISSUE_SIZE-1] != -1;
    }
    
    bool is_pre_issue_queue_empty(){
        return pre_issue_queue[0] == -1;
    }
    
    void pre_issue_queue_add(int instr_label){
        for (int i=0; i < PRE_ISSUE_SIZE; i++) {
            if (pre_issue_queue[i] == -1) {
                pre_issue_queue[i]=instr_label;
                break;
            }
        }
    }
    
    void pre_issue_queue_delete(int index){
        assert(index > -1  && index < PRE_ISSUE_SIZE);
        assert(pre_issue_queue[index] != -1);
        pre_issue_queue[index] = -1;
    }
    
    void pre_issue_queue_reorganize(){
        int temp_copy[PRE_ISSUE_SIZE];
        memcpy(temp_copy, pre_issue_queue, sizeof(int)*PRE_ISSUE_SIZE);
        //test
        for (int i=0; i < PRE_ISSUE_SIZE; i++) {
            assert(pre_issue_queue[i]==temp_copy[i]);
        }
        //==test
        
        int current_index=0;
        for (int i=0; i < PRE_ISSUE_SIZE; i++) {
            if (temp_copy[i] != -1) {
                pre_issue_queue[current_index] = temp_copy[i];
                current_index++;
            }
        }
        
        for (int i=current_index; i<PRE_ISSUE_SIZE; i++) {
            pre_issue_queue[i] = -1;
        }
    }
    
    int pre_issue_queue_size(){
        int size=0;
        for (int i=0; i<PRE_ISSUE_SIZE; i++) {
            if (pre_issue_queue[i] != -1) {
                size++;
            }
        }
        return size;
    }
    
    // ==Tools for pre_issue_queue ********************************
    
    
    
    
};

#endif /* defined(__MIPSsim__pipeline_Console__) */
