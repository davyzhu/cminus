CC = gcc
LEX = flex
YACC = bision
CFLAGS = -Wall -gdwarf-2 -g3
#CFLAGS = -c

all: cminusc

cminusc: util.c scan.c symtab.c analyze.c parse.c main.c  
	${CC} ${CFLAGS} -o cminusc $^
