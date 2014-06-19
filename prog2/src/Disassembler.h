/*  On my honor, I have neither given nor received unauthorized aid on this assignment */
//
//  Mapper.h
//  MIPSsim
//
//  Created by Yuxing Han on 14-3-21.
//  Copyright (c) 2014 Yuxing Han. All rights reserved.
//

#pragma once
#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

using std::map;
using std::vector;

class Disassembler{
private:
	map<char*, char*, cstr_compare> category1;
	map<char*, char*, cstr_compare> category2;
    
public:
//    vector<char*> result;
    map<MEMORY, char*> result;
    
public:
	Disassembler();
    ~Disassembler();
    
	void disassemble(int type, int address,const char* line,char* out);
    
    // UnSignedImmediateTransform -> strtol
    
	int SignedImmediateTransform(const char* number,int length);
    
};


#endif
