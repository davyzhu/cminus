/**************************/
/* File: scan.c           */
/**************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
  {START, INLTEQ, INMTEQ, INEQ, INNEQ, 
   INCOMMENT1, INCOMMENT2, OUTCOMMENT1, OUTCOMMENT2,
   INNUM, INID, DONE}
  StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

/* BUFLEN = length of the input buffer for 
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /*holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */

void getbtp(BacktrackPoint* btp, TokenType token) {
  strncpy(btp->lineBuf, lineBuf, BUFLEN);
  btp->linepos = linepos;
  btp->bufsize = bufsize;

  btp->token = token;
  strncpy(btp->tokenString, tokenString, MAXTOKENLEN);
  
  fgetpos(source, &btp->pos);
}

TokenType setbtp(BacktrackPoint* btp) {
  strncpy(lineBuf, btp->lineBuf, BUFLEN);
  linepos = btp->linepos;
  bufsize = btp->bufsize;

  strncpy(tokenString, btp->tokenString, MAXTOKENLEN);
  fsetpos(source, &btp->pos);

  return btp->token;

}


/* getNextChar fetches the next non-blank character 
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static char getNextChar() {
  if (linepos < bufsize) 
    return lineBuf[linepos++];
  else {
    lineno++;
    if (fgets(lineBuf, BUFLEN-1, source)) {
      if (EchoSource) 
        fprintf(listing, "%4d: %s", lineno, lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else return EOF;
  } 
} 
  
/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar() {
  linepos--;
}

/* lookup table of reserved words */
static struct
{ char* str;
  TokenType tok;
} reservedWords[MAXRESERVED]
= {{"if", IF}, {"else", ELSE}, {"int", INT},
   {"return", RETURN}, {"void", VOID}, {"while", WHILE}};

/* lookup an identifier to see if it is a reserved word
   uses linear search */
static TokenType reservedLoopup (char* s) {
  int i;
  for (i=0; i<MAXRESERVED; i++)
    if (0 == strcmp(s, reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/* function getToken returns the 
   next token in source file */
TokenType getToken() {
  /* index for storing into tokenString */
  int tokenStringIndex = 0;
  /* holds current token to be returned */
  TokenType currentToken;
  /* current state - begins at START */
  StateType state = START;
  /* flag to indicate save to tokenString */
  int save;
  while (state != DONE ){
    char c = getNextChar();
    save = TRUE;
    switch (state) {
    case START:
      /* multi-character first */
      if (isdigit(c)) 
        state = INNUM;
      else if (isalpha(c)) 
        state = INID;
      else if (c == '<') 
        state = INLTEQ;
      else if (c == '>') 
        state = INMTEQ;
      else if (c == '=') 
        state = INEQ;
      else if (c == '!') 
        state = INNEQ;
      else if (c == '/') 
        state = INCOMMENT1;
      else if ((c == ' ') || (c == '\t') || (c == '\n'))
        save = FALSE;
      /* 
       * else if (c == '{') {
       *   state = INCOMMENT;
       *   save = FALSE;
       * }
       */
      /* single-character */
      else {
        state = DONE;
        switch (c) {
        case EOF:
          save = FALSE;
          currentToken = ENDFILE;
          break;
        /* 
         * case '=':
         *   currentToken = EQ;
         *   break;
         * case '<':
         *   currentToken = LT;
         *   break;
         */
        case '+':
          currentToken = PLUS;
          break;
        case '-':
          currentToken = MINUS;
          break;
        case '*':
          currentToken = TIMES;
          break;
        /* 
         * case '/':
         *   currentToken = OVER;
         *   break;
         */
        case '(':
          currentToken = LPAREN;
          break;
        case ')':
          currentToken = RPAREN;
          break;
        case '[':
          currentToken = LBRACK;
          break;
        case ']':
          currentToken = RBRACK;
          break;
        case '{':
          currentToken = LBRACE;
          break;
        case '}':
          currentToken = RBRACE;
          break;
        case ';':
          currentToken = SEMI;
          break;
        case ',':
          currentToken = COMMA;
          break;
        default:
          currentToken = ERROR;
          break;
        }  
      }
      break;
    case INLTEQ:
      state = DONE;
      if (c == '=') currentToken = LTEQ;
      else {
        /* backup in the input */
        ungetNextChar();
        save = FALSE;
        currentToken = LT;
      }
      break;
    case INMTEQ:
      state = DONE;
      if (c == '=') currentToken = MTEQ;
      else {
        /* backup in the input */
        ungetNextChar();
        save = FALSE;
        currentToken = MT;
      }
      break;
    case INEQ:
      state = DONE;
      if (c == '=') currentToken = EQ;
      else {
        /* backup in the input */
        ungetNextChar();
        save = FALSE;
        currentToken = ASSIGN;
      }
      break;
    case INNEQ:
      state = DONE;
      if (c == '=') 
        currentToken = NEQ;
      break;
    case INCOMMENT1:
      if (c == '*') {
        /*clear the first '/'*/
        tokenString[0] = '\0';
        tokenStringIndex = 0;
        state = INCOMMENT2;
        save = FALSE;
      }
      else {
        save = FALSE;
        linepos--;
        state = DONE;
        currentToken = OVER;
      }
      break;
    case INCOMMENT2:
      save = FALSE;
      if (c == '*') state = OUTCOMMENT1;
      break;
    case OUTCOMMENT1:
      save = FALSE;
      if (c == '/') state = START;
      else state = OUTCOMMENT1;
      break;
    /* 
     * case INASSIGN:
     *   state = DONE;
     *   if (c == '=')
     *     currentToken = ASSIGN;
     *   else {
     *     /\* backup in the input *\/
     *     ungetNextChar();
     *     save = FALSE;
     *     currentToken = ERROR;
     *   }
     *   break;
     */
    case INNUM:
      if (!isdigit(c)) {
        /* backup in the input */
        ungetNextChar();
        save = FALSE;
        state = DONE;
        currentToken = NUM;
      }
      break;
    case INID:
      if (!isalpha(c)) {
        /* backup in the input */
        ungetNextChar();
        save = FALSE;
        state = DONE;
        currentToken = ID;
      }
      break;
    case DONE:
    default: /*should never happen*/
      fprintf(listing, "Scanner Bug: state= %d\n", state);
      state = DONE;
      currentToken = ERROR;
      break;
    }
    if (save && (tokenStringIndex <= MAXTOKENLEN))
      tokenString[tokenStringIndex++] = c;
    if (state == DONE) {
      tokenString[tokenStringIndex] = '\0';
      if (currentToken == ID)
        currentToken = reservedLoopup(tokenString);
    }
  }
  if (TraceScan) {
    fprintf(listing, "\t%d: ", lineno);
    printToken(currentToken, tokenString);
  }
  return currentToken;
}
 
