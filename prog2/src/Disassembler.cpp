/*  On my honor, I have neither given nor received unauthorized aid on this assignment *///
//  Mapper.cpp
//  MIPSsim
//
//  Created by Yuxing Han on 14-3-21.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#include "Disassembler.h"


Disassembler::Disassembler(){
	category1["0000"]="J";
	category1["0001"]="JR";
	category1["0010"]="BEQ";
	category1["0011"]="BLTZ";
	category1["0100"]="BGTZ";
	category1["0101"]="BREAK";
	category1["0110"]="SW";
	category1["0111"]="LW";
	category1["1000"]="SLL";
	category1["1001"]="SRL";
	category1["1010"]="SRA";
	category1["1011"]="NOP";
    
	category2["0000"]="ADD";
	category2["0001"]="SUB";
	category2["0010"]="MUL";
	category2["0011"]="AND";
	category2["0100"]="OR";
	category2["0101"]="XOR";
	category2["0110"]="NOR";
	category2["0111"]="SLT";
	category2["1000"]="ADDI";
	category2["1001"]="ANDI";
	category2["1010"]="ORI";
	category2["1011"]="XORI";
}

Disassembler::~Disassembler(){
    map<MEMORY, char*>::iterator it;
    for(it=result.begin();it!=result.end();it++)
    {
        free(it->second);
    }
}

void Disassembler::disassemble(int type, int address,const char* line, char* out){
	char opcode[5],opcode_mapper[10];
	unsigned int instr,sa;
    REGISTER rs,rt,rd,base;
    int offset,immediate;
    char* command = (char*)malloc(sizeof(char)*30);
    
    memcpy(opcode, line+sizeof(char)*2, sizeof(char)*4);opcode[4]='\0';
    
	if(type == 1){
        strcpy(opcode_mapper, category1[opcode]);
        
		if(strcmp(opcode_mapper,"J")==0){
			char instr_index_str[27];
			memcpy(instr_index_str, line+sizeof(char)*6, sizeof(char)*27);
			instr = strtol(instr_index_str,NULL,2) << 2;
            
			sprintf(out,"%s\t%d\t%s #%u",line, address, opcode_mapper,instr);
            sprintf(command, "%s #%u",opcode_mapper,instr);
            result[address] = command;
            return;
		}
        
        if (strcmp(opcode_mapper, "JR")==0) {
//            REGISTER rs;
            char rs_str[6];
            memcpy(rs_str, line+sizeof(char)*6, sizeof(char)*5);rs_str[5]='\0';
            
            rs=strtol(rs_str, NULL, 2);
            
            sprintf(out, "%s\t%d\t%s R%u",line,address,opcode_mapper,rs);
            sprintf(command, "%s R%u",opcode_mapper,rs);
            result[address] = command;
            return;
        }
        
        if (strcmp(opcode_mapper, "BEQ")==0) {
//            REGISTER rs,rt;
            char rs_str[6],rt_str[6], offset_str[17];
            memcpy(rs_str, line+sizeof(char)*6,sizeof(char)*5);rs_str[5]='\0';
            memcpy(rt_str, line+sizeof(char)*11, sizeof(char)*5);rt_str[5]='\0';
            memcpy(offset_str, line+sizeof(char)*16, sizeof(char)*17);
            
            rs = strtol(rs_str, NULL, 2);
            rt = strtol(rt_str, NULL, 2);
            offset = strtol(offset_str, NULL,2)<<2;
            
            sprintf(out, "%s\t%d\t%s R%u, R%u, #%d",line,address,opcode_mapper,rs,rt,offset);
            sprintf(command, "%s R%u, R%u, #%d",opcode_mapper,rs,rt,offset);
            result[address] = command;
            
            return;
        }
        
        if (strcmp(opcode_mapper, "BLTZ")==0 || strcmp(opcode_mapper, "BGTZ")==0) {
//            REGISTER rs;
            char rs_str[6],offset_str[17];
            memcpy(rs_str, line+sizeof(char)*6, sizeof(char)*5);rs_str[5]='\0';
            memcpy(offset_str, line+sizeof(char)*16, sizeof(char)*17);
            
            rs = strtol(rs_str, NULL, 2);
            offset = strtol(offset_str, NULL, 2)<<2;
            
            sprintf(out, "%s\t%d\t%s R%u, #%d", line,address,opcode_mapper,rs,offset);
            sprintf(command, "%s R%u, #%d",opcode_mapper,rs,offset);
            result[address] = command;
            return;
        }
        
        if (strcmp(opcode_mapper, "BREAK") == 0 || strcmp(opcode_mapper, "NOP") == 0) {
            sprintf(out, "%s\t%d\t%s",line,address,opcode_mapper);
            sprintf(command, "%s",opcode_mapper);
            result[address] = command;
            return;
        }
        
        if (strcmp(opcode_mapper, "SW") == 0 || strcmp(opcode_mapper, "LW") == 0 ) {
//            REGISTER base,rt;
            char base_str[6],rt_str[6],offset_str[17];
            memcpy(base_str, line+sizeof(char)*6, sizeof(char)*5); base_str[5]='\0';
            memcpy(rt_str, line+sizeof(char)*11, sizeof(char)*5); rt_str[5]='\0';
            memcpy(offset_str, line+sizeof(char)*16, sizeof(char)*17);
            
            base = strtol(base_str, NULL, 2);
            rt = strtol(rt_str, NULL, 2);
            offset = strtol(offset_str, NULL, 2);
            
            sprintf(out, "%s\t%d\t%s R%u, %d(R%u)",line,address,opcode_mapper,rt, offset,base);
            sprintf(command, "%s R%u, %d(R%u)",opcode_mapper,rt,offset,base);
            result[address] = command;
            return;
        }
        
        if (strcmp(opcode_mapper, "SLL")==0 || strcmp(opcode_mapper, "SRL")==0 ||
            strcmp(opcode_mapper, "SRA")==0) {
//            REGISTER rt,rd;
            char rt_str[6],rd_str[6],sa_str[6];
            memcpy(rt_str, line+sizeof(char)*11, sizeof(char)*5);rt_str[5]='\0';
            memcpy(rd_str, line+sizeof(char)*16, sizeof(char)*5);rd_str[5]='\0';
            memcpy(sa_str, line+sizeof(char)*21, sizeof(char)*5);sa_str[5]='\0';
            
            rt = strtol(rt_str, NULL, 2);
            rd = strtol(rd_str, NULL, 2);
            sa = strtol(sa_str, NULL, 2);
            
            sprintf(out, "%s\t%d\t%s R%u, R%u, #%u", line,address,opcode_mapper,rd,rt,sa);
            sprintf(command, "%s R%u, R%u, #%u",opcode_mapper,rd,rt,sa);
            result[address] = command;
            return;
        }
        
        
	}
    
	else //type == 2
	{
        strcpy(opcode_mapper, category2[opcode]);
        
		if(strcmp(opcode_mapper, "ADD")==0 || strcmp(opcode_mapper, "SUB")==0 ||
           strcmp(opcode_mapper, "MUL")==0 || strcmp(opcode_mapper, "AND")==0 ||
           strcmp(opcode_mapper, "OR")==0 || strcmp(opcode_mapper, "XOR")==0 ||
           strcmp(opcode_mapper, "NOR")==0 || strcmp(opcode_mapper, "SLT")==0){
			
//			REGISTER rs,rt,rd;
			char rs_str[6],rt_str[6],rd_str[6];
			memcpy(rs_str, line+sizeof(char)*6,sizeof(char)*5);rs_str[5]='\0';
			memcpy(rt_str, line+sizeof(char)*11,sizeof(char)*5);rt_str[5]='\0';
			memcpy(rd_str, line+sizeof(char)*16,sizeof(char)*5);rd_str[5]='\0';
            
			rs=strtol(rs_str,NULL,2);
			rt=strtol(rt_str,NULL,2);
			rd=strtol(rd_str,NULL,2);
            
			sprintf(out, "%s\t%d\t%s R%u, R%u, R%u",line,address,opcode_mapper,rd,rs,rt);
            sprintf(command, "%s R%u, R%u, R%u",opcode_mapper,rd,rs,rt);
            result[address] = command;
            return;
		}
        
		if(strcmp(opcode_mapper, "ADDI")==0 || strcmp(opcode_mapper, "ANDI")==0||
           strcmp(opcode_mapper, "ORI")==0 || strcmp(opcode_mapper, "XORI")==0){
//			REGISTER rs,rt;
			char number_str[17];
			char rs_str[6],rt_str[6];
			memcpy(rs_str, line+sizeof(char)*6, sizeof(char)*5);rs_str[5]='\0';
			memcpy(rt_str, line+sizeof(char)*11, sizeof(char)*5);rt_str[5]='\0';
            
			rs=strtol(rs_str,NULL,2);
			rt=strtol(rt_str,NULL,2);
            
			memcpy(number_str, line+sizeof(char)*16, sizeof(char)*17);
			
			immediate = SignedImmediateTransform(number_str,17-1);
            
			sprintf(out, "%s\t%d\t%s R%u, R%u, #%d",line,address,opcode_mapper,rt,rs,immediate);
            sprintf(command, "%s R%u, R%u, #%d",opcode_mapper,rt,rs,immediate);
            result[address] = command;
            return;
		}
	}
    
}

int Disassembler::SignedImmediateTransform(const char* number,int length){
	if(number[0]=='0')
		return strtol(number,NULL,2);
	else{
		char* left =(char*)malloc(sizeof(char)*length);
		memcpy(left, number+sizeof(char)*1, sizeof(char)*length);
		int imm = strtol(left,NULL,2) - (1<<31);
        
		free(left);
        
		return imm;
	} 
}
