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
#define MAXRESERVED 6

typedef enum {
  /* book-keeping tokens */
  ENDFILE, ERROR,
  /* reserved words */
  IF, ELSE, INT, RETURN, VOID, WHILE, 
  /* multicharacter tokens */
  ID, NUM,
  /* special symbols */
  ASSIGN, EQ, NEQ, LT, LTEQ, MT, MTEQ, PLUS, MINUS, TIMES, OVER, 
  LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE, SEMI, COMMA

} TokenType;

extern FILE* source; /* source code file */
extern FILE* listing; /* listing output file*/
extern FILE* code; /* code file for TM simulator */

extern int lineno;
extern int EchoSource;
extern int TraceScan;

#endif
