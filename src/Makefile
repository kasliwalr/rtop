# Makefile for JustForFun Files

# A few variables

CC=g++
CCFLAGS= -std=c++11 -g -Wall -pedantic -O0 -DBOOST_LOG_DYN_LINK 
 
LIBS=-lncursesw -lpanelw -lmenuw -lpthread -lboost_log -lboost_thread -lboost_system -lboost_log_setup


SRC_DIR=.
EXE_DIR=../bin
INCLUDE_DIR=../include

EXES = \
    ${EXE_DIR}/rtop_v0_1\
 
${EXE_DIR}/%: %.o
	${CC} ${CCFLAGS} -o $@ $< ${LIBS}

%.o: ${SRC_DIR}/%.cpp  ${INCLUDE_DIR}/*.h
	${CC} ${CCFLAGS} -I/usr/include/ncursesw -I../include/ -I../libs/pugixml-1.9/src/ -o $@ -c $<

all:    ${EXES}

clean:
	@rm -f ${EXES}


