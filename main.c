#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "symtab.h"
#include "analyze.h"

int lineno = 0;

FILE* source;
FILE* listing;
FILE* code;

int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;

int Error = FALSE;

int main(int argc, char * argv[]) {
  char pgm[20]; /* source code file name */
  TreeNode* syntaxTree;

  if (argc !=2) {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  strcpy(pgm, argv[1]);
  source = fopen(pgm,"r");
  if (source == NULL) {
    fprintf(stderr, "File %s not found\n", pgm);
    exit(1);
  }
  listing = stdout;
  //while(getToken()!=ENDFILE);
  syntaxTree = parse();
  if (syntaxTree == NULL) fprintf(listing,"syntaxTree is Null\n");
  
  if (TraceParse) {
    fprintf(listing,"\nSyntax tree:\n");
    printTree(syntaxTree);
  }
  
  if(!Error){
    /* 
     * fprintf(listing,"\nBuilding symbol table...\n");
     * buildSymtab(syntaxTree);
     */
    fprintf(listing,"\nChecking type...\n");
    typeCheck(syntaxTree);
    fprintf(listing,"\nChecking type finished...\n");
  }  

  return 1;
}
