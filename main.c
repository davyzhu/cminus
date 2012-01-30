#include "globals.h"

#include "scan.h"

int lineno = 0;

FILE* source;
FILE* listing;
FILE* code;

int EchoSource = TRUE;
int TraceScan = TRUE;

int main(int argc, char * argv[]) {
  char pgm[20]; /* source code file name */
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
  while(getToken()!=ENDFILE);
  return 1;
}
