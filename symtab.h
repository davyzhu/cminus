#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* 
 * list of line numbers of source code in which a 
 * variable is referenced
 */

/* size of hash table */
#define SIZE 211
typedef enum {SNone, SVoid, SInt, SArray, SFunc} SType;

typedef struct LineListRec {
  int lineno;
  struct LineListRec* next;
}* LineList;

/* record in hash table */
typedef struct BucketListRec {
  char* name;
  SType stype; /* symbol type */
  SType frtype; /* function return type */
  SType fptype[6]; /* function parameter type (at most 6 parameters) */
  LineList lines;
  int memloc; 
  struct BucketListRec* next;
}* BucketList;

/* symbol table with scope*/
typedef struct symbolTable {
  BucketList hashTable[SIZE];
  struct symbolTable* up; /* to up level */
  unsigned int scope; /* top level is zero */
  /* 
   * struct symbolTable* next; /\* to next table on same level *\/
   * struct symbolTable* down; /\* to down level *\/
   */
}* SymbolTable;

SymbolTable st_root, st_cur, st_prev/*, st_parent, st_sibling */;

unsigned int cur_scope;

void st_new(unsigned int scope);
BucketList st_insert(char* name, int lineno, int loc, SType stype, SType frtype);
int st_lookup(char* name, unsigned int depth);
BucketList st_lookup2(char* name, unsigned int depth);
void st_delete();
void printSymTab(FILE* listing);

#endif
