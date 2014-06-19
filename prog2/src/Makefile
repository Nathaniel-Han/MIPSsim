## On my honor, I have neither given nor received unauthorized aid on this assignment
##
##  Makefile
##  MIPSsim
##
##  Created by Yuxing Han on 14-3-21.
##  Copyright (c) 2014 Yuxing Han. All rights reserved.
##

CC = g++ 

CFLAGS = -O2 -w -stdlib=libstdc++ ##For Mac OS X+ 

BINARIES = MIPSsim

OBJS = MIPSsim.o Disassembler.o pipeline_Console.o

all:$(BINARIES)

MIPSsim: $(OBJS)
	$(CC) $(CFLAGS) -o MIPSsim $(OBJS) 

MIPSsim.o: 
	$(CC) $(CFLAGS) -c MIPSsim.cpp

Disassembler.o:
	$(CC) $(CFLAGS) -c Disassembler.cpp

pipeline_Console.o:
	$(CC) $(CFLAGS) -c pipeline_Console.cpp

clean:
	rm -rf MIPSsim
	rm -rf *.o
	rm -rf output*.txt
