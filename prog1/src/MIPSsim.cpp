/*  On my honor, I have neither given nor received unauthorized aid on this assignment */
//
//  main.cpp
//  MIPSsim
//
//  Created by Yuxing Han on 14-3-21.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>


#include "Disassembler.h"
#include "Console.h"

#define INPUTFILE "sample.txt"
#define OUTPUT_DIS "output_disassembly.txt"
#define OUTPUT_SIM "output_simulation.txt"


int main(int argc, char** argv)
{
    if (argc != 2 ) {
        fprintf(stderr, "Invalid input to [MIPSsim]\n");
        printf("Usage: MIPSsim [input file name]\n");
        return 1;
    }
    
//	FILE *fp = fopen(INPUTFILE, "r");
    FILE *fp =fopen(argv[1], "r");
    
	if(!fp){
        fprintf(stderr, "Can't open input file");
        return 1;
	}
    
    FILE *out_dis = fopen(OUTPUT_DIS, "w");
    
	char line[INSTRUCTION_LENGTH + 1];
	bool is_instruction = true;
	int address = INITIAL_COM_ADDR;
    char out[100];
	Disassembler disassembler;
    map<MEMORY,int> inital_mem;
    MEMORY data_addr_start = -1;
    MEMORY data_addr_end;
	
    
	while(fscanf(fp,"%s\n",&line) != -1){
        
		assert(strlen(line) == INSTRUCTION_LENGTH);
        
		if(is_instruction){
			char head[3] = {line[0],line[1]};
            
			if(strcmp(head, "01") == 0){
                disassembler.disassemble(1, address, line, out);

                fprintf(out_dis, "%s\n",out);
                //printf("%s\n", out);
				if(strcmp(line, BREAK_CODE) == 0)
					is_instruction = false;
			}
			else if (strcmp(head, "11") == 0)
            {
				disassembler.disassemble(2, address, line, out);
                //printf("%s\n", out);
                fprintf(out_dis, "%s\n",out);
            }
		}
		else {
            //printf("%s\t%d\t%d\n",line,address,disassembler.ImmediateTransform(line,INSTRUCTION_LENGTH));
            int num = disassembler.SignedImmediateTransform(line,INSTRUCTION_LENGTH);
			fprintf(out_dis,"%s\t%d\t%d\n",line,address, num);
            inital_mem[address] = num;
            
            if (data_addr_start == -1) {
                data_addr_start = address;
            }
		}
        
		address += 4;
	}
    
    data_addr_end = address - 4;

    
//    for (map<MEMORY, char*>::iterator it = disassembler.result.begin(); it != disassembler.result.end(); it++) {
//        printf("%d:%s\n", it->first, it->second);
//    }
    
	fclose(fp);
    fclose(out_dis);
    
    //simulation
    Console console(INITIAL_COM_ADDR, data_addr_start/4,data_addr_end/4, OUTPUT_SIM);
    
    map<MEMORY, int>::iterator it;
    for (it = inital_mem.begin(); it != inital_mem.end(); it++) {
        console.memory_set(it->first/4, it->second);
    }
    
    while (console.PC != FLAG_COM_ADDR) {
        console.execute(disassembler.result[console.PC]);
//        printf("%d\n", console.PC);
    }
}

