######################################################################
# Generic C++ Project Makefile v2.1
# Copyright (c) 1998-2001 Vokimon/KKEPerian UNLTD
# Copyright (c) 1998-2001 Helly/Patata Productions
######################################################################

# Change the following options as suited to your project

# The name of the executable file
EXEC=carbon

# The name of the profiler
PROF=PROF_$(EXEC)

# Library flags for the linker
LDLIBS=-llagniappe -lreadline -lpthread -lrt -lhl++ -lcryptopp
# Flags for the C compiler
CFLAGS=-Wall -O2 -I. -D_CS_MACHINES -D_GNU_SOURCE -I/projects/lasr/space5/lagniappe/include -I/p/include/boost-1_35 -I../lib/cryptopp -g
LDFLAGS= -L/projects/lasr/space5/lagniappe/lib -L../lib/cryptopp

# Change the following options as suited to your system

# The C compiler
CC=gcc
# The C++ compiler
CPPC=g++
# Command used to delete files
RM=rm -rf

CPPSOURCES:= Thread.cc NetRequestDevice.cc NetRequest.cc carbon_op_queue_primary.cc carbon_op_queue_repl_1.cc carbon_op_echoserver.cc
MAIN_SOURCE:=lc_main.cc
PROF_SOURCE:=
OBJSCPP:=$(CPPSOURCES:.cc=.o)
MAIN_OBJ:=$(MAIN_SOURCE:.cc=.o)
PROF_OBJ:=$(PROF_SOURCE:.cc=.o)

default: depend ${EXEC}

depend: .depend
	@echo --- Generating dependencies
	$(CPPC) $(CFLAGS) -MM $(CPPSOURCES) $(MAIN_SOURCE) $(PROF_SOURCE) -D_KKEP_BUSCANT_DEPENDENCIES > .depend

.depend:
	touch .depend

clean: 
	-$(RM) *.o
	-$(RM) ${EXEC} ${PROF}

deepclean: clean
	-$(RM) lc_*

include .depend

${EXEC}: $(OBJSCPP) $(MAIN_OBJ)
	@echo --- Linking $@
	${CPPC} $(LDFLAGS) $(OBJSCPP) $(MAIN_OBJ) ${LDLIBS} -o ${EXEC}

$(OBJSCPP) : %.o: %.cc
	@echo --- Compiling $<
	${CPPC} ${CFLAGS} -c $< -o $@
$(MAIN_OBJ) : %.o: %.cc
	@echo --- Compiling $<
	${CPPC} ${CFLAGS} -c $< -o $@
$(PROF_OBJ) : %.o: %.cc
	@echo --- Compiling $<
	${CPPC} ${CFLAGS} -c $< -o $@

prof: depend ${PROF}

${PROF}: $(OBJSCPP) $(PROF_OBJ)
	@echo --- Linking $@
	${CPPC} $(LDFLAGS) $(OBJSCPP) $(PROF_OBJ) ${LDLIBS} -o ${PROF}



