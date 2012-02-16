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
  ASSIGN, 
  EQ, NEQ, LT, LTEQ, MT, MTEQ, 
  PLUS, MINUS, TIMES, OVER, 
  LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE, SEMI, COMMA

} TokenType;

extern FILE* source; /* source code file */
extern FILE* listing; /* listing output file*/
extern FILE* code; /* code file for TM simulator */

extern int lineno;
extern int EchoSource;
extern int TraceScan;
extern int TraceAnalyze;
extern int Error;

/* syntax tree for parsing */
typedef enum {DeclK, ParamK, CompStmtK, StmtK, ExpK} NodeKind;
typedef enum {VarK, FunK} DeclKind;
typedef enum {IfK, WhileK, ReturnK, AssignK} StmtKind;
typedef enum {OpK, ConstK, IdK, CallK} ExpKind;


/* ExpType is used for type checking */
typedef enum {Void, Integer, Boolean, Array} ExpType;

#define MAXCHILDREN 3
typedef struct treeNode {
  struct treeNode * child[MAXCHILDREN];
  struct treeNode *sibling;
  unsigned int lineno;
  NodeKind nodekind;
  union {DeclKind decl; StmtKind stmt; ExpKind exp;} kind;
  union {TokenType op; int val; char* name;} attr;
  unsigned int arraynum; /* 1: is array, 0: is not array, >1 array number */ 
  ExpType type; /* for type checking of exps */
} TreeNode;

#define BUFLEN 256
#define MAXTOKENLEN 40

typedef struct backtrackPoint {
  /* line buffer */
  char lineBuf[BUFLEN];
  int linepos;
  int bufsize;
  /* current token */
  TokenType token;
  char tokenString[MAXTOKENLEN+1];
  /* source file location */
  fpos_t pos;
} BacktrackPoint;


#endif
