#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif
 
#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8

typedef enum {
  /* book-keeping tokens */
  ENDFILE, ERROR,
  /* reserved words */
  IF, THEN, ELSE, END, REPEAT,UNTIL, READ, WRITE,
  /* multicharacter tokens */
  ID, NUM,
  /* special symbols */
  ASSIGN, EQ, LT, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI
} TokenType;

extern FILE* source; /* source code file */
extern FILE* listing; /* listing output file*/
extern FILE* code; /* code file for TM simulator */

extern int lineno;
extern int EchoSource;
extern int TraceScan;

#endif
