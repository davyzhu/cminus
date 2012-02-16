#include "globals.h"
#include "util.h"

void printToken(TokenType token, const char* tokenString) {
  switch(token) {
  case IF:
  case ELSE:
  case INT:
  case RETURN:
  case VOID:
  case WHILE:
    fprintf(listing, 
            "reserved word: %s\n", tokenString);
    break;
  case ASSIGN: fprintf(listing, "=\n"); break;
  case LT: fprintf(listing, "<\n"); break;
  case LTEQ: fprintf(listing, "<=\n"); break;
  case MT: fprintf(listing, ">\n"); break;
  case MTEQ: fprintf(listing, ">=\n"); break;

  case EQ: fprintf(listing, "=\n"); break;
  case NEQ: fprintf(listing, "!=\n"); break;

  case LPAREN: fprintf(listing, "(\n"); break;
  case RPAREN: fprintf(listing, ")\n"); break;
  case LBRACK: fprintf(listing, "[\n"); break;
  case RBRACK: fprintf(listing, "]\n"); break;
  case LBRACE: fprintf(listing, "{\n"); break;
  case RBRACE: fprintf(listing, "}\n"); break;

  case SEMI: fprintf(listing, ";\n"); break;
  case COMMA: fprintf(listing, ",\n"); break;
  case PLUS: fprintf(listing, "+\n"); break;
  case MINUS: fprintf(listing, "-\n"); break;
  case TIMES: fprintf(listing, "*\n"); break;
  case OVER: fprintf(listing, "/\n"); break;
  case ENDFILE: fprintf(listing, "EOF\n"); break;
  case NUM:
    fprintf(listing,
            "NUM, val= %s\n", tokenString);
    break;
  case ID:
    fprintf(listing,
            "ID, name= %s\n", tokenString);
    break;
  case ERROR:
    fprintf(listing,
            "ERROR: %s\n", tokenString);
    break;
  default: /* should never happen */
    fprintf(listing, "Unknown token: %d\n", token);
  }
}

TreeNode* newDecl(ExpType et, char* id) {
  TreeNode* t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else {
    for (i=0; i<MAXCHILDREN;i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DeclK;
    t->lineno = lineno;
    t->attr.name = id;
    t->type = et;
  }
  return t;
}

TreeNode* newParam(ExpType et, char* id) {
  TreeNode* t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else {
    for (i=0; i<MAXCHILDREN;i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ParamK;
    t->lineno = lineno;
    t->attr.name = id;
    t->type = et;
  }
  return t;
}

TreeNode* newCompStmtNode(void) {
  TreeNode* t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else {
    for (i=0; i<MAXCHILDREN;i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = CompStmtK;
    t->lineno = lineno;
  }
  return t;
}


TreeNode* newStmtNode(StmtKind kind) {
  TreeNode* t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else {
    for (i=0; i<MAXCHILDREN;i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode* newExpNode(ExpKind kind) {
  TreeNode* t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else {
    for (i=0; i<MAXCHILDREN;i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

char* copyString(char* s) {
  char* t;
  if (s==NULL) return NULL;
  if ((t = malloc(strlen(s) + 1)) == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else strcpy(t,s);
  return t;
}

static int indentno = 0;

#define INDENT indentno+=2
#define UNINDENT indentno-=2

static void printSpaces(void) {
  int i;
  for(i=0; i<indentno;i++)
    fprintf(listing, " ");
}

static char s_int[] = "Int";
static char s_void[] = "Void";
static char s_wrong[] = "Wrong Type";

void printTree(TreeNode* tree) {
  int i;
  //ExpType et;
  char* s;
  INDENT;
  if (tree == NULL) fprintf(listing,"tree is Null\n");
    
  while(tree != NULL) {
    printSpaces();
    switch(tree->nodekind) {
    case DeclK: 
      if(tree->type==Void) s=s_void;
      else if(tree->type==Integer) s=s_int;
      else s=s_wrong;

      switch(tree->kind.decl) {
      case VarK:
        fprintf(listing,"Decl Var: %s, %s, %d \n",
                s, tree->attr.name, tree->arraynum);
        break;
      case FunK:
        fprintf(listing,"Decl Fun: %s, %s, ",
                s, tree->attr.name);
        fprintf(listing,"\n");
        break;
      default:
        fprintf(listing,"Unknown DeclNode kind\n");
        break;
      }
      break;
    case ParamK:
      fprintf(listing,"Params:");
      if ((tree->type==Void) && !strncmp(tree->attr.name,"void",4))
        fprintf(listing,"void");
      else {
        if (tree->type==Integer)
          fprintf(listing,"Int, ");
        else if (tree->type==Void)
          fprintf(listing,"Void, ");
        fprintf(listing,"%s ",tree->attr.name);
        if (tree->arraynum==1)
          fprintf(listing,"[]");
      }
      fprintf(listing,"\n");
      break;
    case CompStmtK:
      fprintf(listing,"Comp Stmt:\n");
      break;
    case StmtK: 
      switch(tree->kind.stmt) {
      case IfK:
        fprintf(listing,"If\n");
        break;
      case WhileK:
        fprintf(listing,"While\n");
        break;
      case ReturnK:
        fprintf(listing,"Return\n");
        break;
      case AssignK:
        fprintf(listing,"Assign\n");
        break;
      default:
        fprintf(listing,"Unknown StmtNode kind\n");
        break;
      }
      break;
     
    case ExpK: 
      switch (tree->kind.exp) {
      case OpK:
        fprintf(listing, "Op: ");
        printToken(tree->attr.op,"\0");
        break;
      case ConstK:
        fprintf(listing, "Const: %d\n", tree->attr.val);
        break;
      case IdK:
        fprintf(listing, "Id: %s\n", tree->attr.name);
        break;
      case CallK:
        fprintf(listing, "Call: %s\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
      break;
    
    default: 
      fprintf(listing, "Unknown node kind\n");
      break;
    }
    for (i=0; i<MAXCHILDREN; i++)
      if (tree->child[i])
        printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
