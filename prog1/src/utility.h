/*  On my honor, I have neither given nor received unauthorized aid on this assignment */
//
//  utility.h
//  MIPSsim
//
//  Created by Yuxing Han on 14-3-22.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#ifndef MIPSsim_utility_h
#define MIPSsim_utility_h

#include <assert.h>

//register label
#define REGISTER unsigned int

#define MEMORY int

#define REG_NUM 32
#define MEM_SIZE 256 //  1024/4

#define INSTRUCTION_LENGTH 32
#define INITIAL_COM_ADDR 256
#define FLAG_COM_ADDR -1

#define BREAK_CODE "01010100000000000000000000001101"

// for c_str

class cstr_compare {
public:
	bool operator()(const char *l, const char * r) {
		if (strcmp(l, r) < 0)
			return true;
		else
			return false;
	}
};

#endif
