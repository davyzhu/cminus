/* see handwriting diary in 2012, Feb 13 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* multiplier in hash function(power of 2) */
#define SHIFT 4

/* hash function */
static int hash(char* key) {
  int t = 0;
  int i = 0;
  while(key[i] != '\0') {
    t = ((t<<SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return t;
}


void st_new(unsigned int scope) {
  static unsigned int cnt = 0;
  SymbolTable st = (SymbolTable)malloc(sizeof(struct symbolTable));
  int i;
  for (i=0;i<SIZE;i++) {
    st->hashTable[i] = NULL;
  }
  st->scope = scope;
  if (st == NULL) 
    exit(EXIT_FAILURE);
  st->up = NULL;

  if (cnt == 0) {
    st_root = st_cur = st;
    st_prev = NULL;
  } else {
    st_prev = st_cur;
    st_cur = st;
    st_cur->up = st_prev;
  }
  
  cnt++;
}

void st_delete() {
  SymbolTable t;
  if (st_cur->up) {
    t = st_cur;
    st_cur = st_cur->up;
    free(t);
  } else {
    /* top layer */
    free(st_cur);
  }
}

/* 
 * insert line numbers and memory locations into symbol table
 * loc=memory locatoin is inserted only the first time, 
 * otherwise ignored
 */
BucketList st_insert(char* name, int lineno, int loc, SType stype, SType frtype) {
  int h=hash(name);
  BucketList l = st_cur->hashTable[h];
  while(l && (strcmp(name, l->name) != 0)) 
    l = l->next;
  if (l==NULL){ /* variable not yet in table */
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines=(LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->stype = stype;
    l->frtype = (stype == SFunc) ? frtype: SNone; 
    { int i;
      for (i=0; i<6; i++) {
        l->fptype[i] = SNone;
      }
    }
    l->lines->next = NULL;
    l->next = st_cur->hashTable[h];
    st_cur->hashTable[h] = l;
  } else { /* found in table, just add lineno */
    LineList t = l->lines;
    while(t->next) t=t->next; /* find last element */
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
  return l;
}

/* return memory location of a variable or -1 if not found */
int st_lookup(char* name, unsigned int depth) {
  /* depth=0, search all scope, depth=1 search current scope 
     depth=2, search current and up scope*/
  int h = hash(name);
  unsigned char search_all = (depth==0) ? 1 : 0;
  SymbolTable st=NULL;
  do {
    BucketList l = st_cur->hashTable[h];
    while(l && (strcmp(name, l->name) != 0))
      l = l->next;
    if(l) return l->memloc;
    else st = st_cur->up;
  } while (search_all || (st && --depth));
  return -1;
}

/* return pointer of a function/variable or NULL if not found */
BucketList st_lookup2(char* name, unsigned int depth) {
  /* depth=0, search all scope, 
     depth=1 search current scope, 
     depth=2, search current and up scope*/
  unsigned int scope = cur_scope;
  int h = hash(name);
  unsigned char search_all = (depth==0) ? 1 : 0;
  SymbolTable st=st_cur;
  do {
    BucketList l = st->hashTable[h];
    while(l && (strcmp(name, l->name) != 0))
      l = l->next;
    if(l) return l;
    else if (scope == 0 || depth == 1) return NULL;
    else st = st->up;
  } while (scope-- && (search_all || --depth));
  return NULL;
}


/* print symble table */
void printSymTab(FILE* listing) {
  int i;
  BucketList l=NULL;
  LineList t=NULL;
  SymbolTable st=st_cur;
  fprintf(listing, "sType:symbol, frType:function return, fpType: function paramter(at most 6)\n");
  fprintf(listing, "Variable Name  sType frType fpType Scope Location  Line Numbers\n");
  fprintf(listing, "-------------  ----- ------ ------ ----- --------  ------------\n");
  if (st) {
    do {
      for (i=0; i<SIZE; ++i) {
        if(st->hashTable[i]) {
          l = st->hashTable[i];
          while(l) {
            t=l->lines;
            fprintf(listing,"%-14s ", l->name);

            switch(l->stype) {
            case SVoid: fprintf(listing,"%-5s ", "V"); break;
            case SInt: fprintf(listing,"%-5s ", "I"); break;
            case SArray: fprintf(listing,"%-5s ", "A"); break;
            case SFunc: fprintf(listing,"%-5s ", "F"); break;
            default: break;   
            }
            
            /* print function return type */
            if (l->stype == SFunc) {
              switch(l->frtype) {
              case SVoid: fprintf(listing,"%-6s ", "V"); break;
              case SInt: fprintf(listing,"%-6s ", "I"); break;
              case SArray: fprintf(listing,"%-6s ", "A"); break;
              default: break;   
              }
            } else {
              fprintf(listing,"%-6s ", "");
            }
            
            /* print function parameter type */
            { 
              unsigned char i;
              for (i=0; i<6; i++) {
                switch(l->fptype[i]) {
                case SNone: fprintf(listing, " "); break;
                case SInt: fprintf(listing, "I"); break;
                case SArray: fprintf(listing, "A"); break;
                default: break;
                }
              }
              fprintf(listing, " ");
            }

            fprintf(listing,"%-5d ", st->scope);
            fprintf(listing,"%-8d ", l->memloc);
            while(t) {
              fprintf(listing,"%4d ", t->lineno);
              t=t->next;
            }
            fprintf(listing,"\n");
            l=l->next;
          }
        }
      }
    } while (st = st->up);
  }
  fprintf(listing,"\n");
}
