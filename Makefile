CC = gcc
LEX = flex
YACC = bision
CFLAGS = -gdwarf-2 -g3
#CFLAGS = -c

all: tinyc

tinyc: util.c scan.c main.c
	${CC} ${CFLAGS} -o tinyc $^
