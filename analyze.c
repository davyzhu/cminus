#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;

static void traverse(TreeNode* t,
                     void(* preProc) (TreeNode*),
                     void(* postProc) (TreeNode*)) {
  if (t) {
    preProc(t);
    { int i;
      for(i=0;i<MAXCHILDREN;i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* do nothing */
static void nullProc(TreeNode* t) {
  if(t==NULL) return;
  else return;
}

static void exitScope(TreeNode* t) {
  if (t->nodekind==CompStmtK) {
    cur_scope--;
    st_delete();
    printSymTab(listing);
  }
}

/* insert identifiers stored in t into symbol tables */
static void insertNode(TreeNode* t) {
  static NodeKind prevnodekind = DeclK;
  static BucketList prevfundecl = NULL;
  /* initialize */
  if (st_root==NULL) {
    BucketList bl = NULL;
    st_new(0);
    /* insert function input() and output() */
    st_insert("input",0,location++,SFunc,SInt);
    bl = st_insert("output",0,location++,SFunc,SVoid);
    bl->fptype[0] = SInt;
  }
  switch(t->nodekind) {
  case ParamK:
    if (strcmp(t->attr.name, "void") != 0) {
      SType stype = SNone;
      if (t->arraynum == 0)
        stype = SInt;
      else if(t->arraynum == 1)
        stype = SArray;
      else
        fprintf(listing,"error: arraynum should be either 0 or 1\n");
      
      /* insert parameter to current scope */
      st_insert(t->attr.name, t->lineno, location++, stype, SNone);
      
      /* add parameter to previous function declaration's fptype */
      unsigned char i=0;
      while(prevfundecl->fptype[i] != SNone && i<5) i++;
      prevfundecl->fptype[i]=stype;
    }
    printSymTab(listing);
    break;
  case CompStmtK:
    if (prevnodekind != ParamK) {
      printSymTab(listing);
      st_new(++cur_scope);
    }
    break;
  case DeclK:
    switch(t->kind.decl) {
    case VarK:
      if(st_lookup(t->attr.name, 1)==-1) { 
        if (t->arraynum == 0)
          st_insert(t->attr.name, t->lineno, location++, SInt, SNone);
        else if(t->arraynum > 1) {
          st_insert(t->attr.name, t->lineno, location++, SArray,
                    SNone);
        }
      /* already in table, just add lineno */
      } else
        fprintf(listing,"error: variable %s already declared in the same scope\n",
                t->attr.name);
        /* 
         * if (t->arraynum == 0)
         *   st_insert(t->attr.name, t->lineno, 0, SInt, SNone);
         * else if(t->arraynum > 1)
         *   st_insert(t->attr.name, t->lineno, 0, SArray, SNone);
         */
      printSymTab(listing);
      break;
      
    case FunK:
      if (cur_scope==0) {
        /* not in table, define new */
        if(st_lookup(t->attr.name, 1)==-1)
          prevfundecl = st_insert(t->attr.name, t->lineno, location++,
                                  SFunc, (t->type==Void)?SVoid:SInt );
        /* already in table, just add lineno */
        else
          fprintf(listing,"error: function %s already declared in the same scope\n",
                  t->attr.name);
          
          /* 
           * st_insert(t->attr.name, t->lineno, 0, 
           *           SFunc, (t->type==Void)?SVoid:SInt);
           */
        /* add new symbol table for params and local variable */
        st_new(++cur_scope);
        
      } else {
        fprintf(listing,"error: function declaration should appear \
        in first level scope\n");
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  prevnodekind = t->nodekind;
}

/* construct symbol table by preorder traversal of syntax table */
void buildSymtab(TreeNode* syntaxTree) {
  st_root = st_cur = st_prev = NULL;
  cur_scope = 0;
  traverse(syntaxTree,insertNode,exitScope);
  if(TraceAnalyze) {
    fprintf(listing,"\nSymbol table: (V)oid, (I)nt, (A)rray, (N)one, (F)unction\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode* t, char* message) {
  fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
  Error=TRUE;
}

/* check type/symbol at a single tree node */
static void checkNode(TreeNode* t) {
  BucketList bl;
  switch(t->nodekind) {
  case ExpK:
    switch(t->kind.exp) {
    case OpK:
      if ((t->child[0]->type != Integer) ||
          (t->child[1]->type != Integer))
        typeError(t,"Op applied to non-integer");
      if ((t->attr.op == EQ) || (t->attr.op == NEQ) ||
          (t->attr.op == LT) || (t->attr.op == LTEQ) ||
          (t->attr.op == MT) || (t->attr.op == MTEQ))
        t->type = Boolean;
      else
        t->type = Integer;
      break;
    case ConstK:
      t->type = Integer;
      break;
    case IdK:
      if(bl = st_lookup2(t->attr.name, 0)) {
        if(bl->stype == SInt)
           t->type = Integer;
        else if(bl->stype == SArray) {
          if (t->child[0] && t->child[0]->type==Integer)
            t->type = Integer;
          else if (t->child[0]==NULL)
            t->type = Array;
          else
            typeError(t,"Array index error");
        }
      } else {
        fprintf(listing,"error: symbol %s at line %d not found\n",
                t->attr.name, t->lineno);
        
        printSymTab(listing);
      }
      break;
    case CallK:
      if(bl = st_lookup2(t->attr.name, 0)) {
        if(bl->stype != SFunc) fprintf(listing,"error: %s at line %d is not function",
                                       t->attr.name, t->lineno);
        else {
          /* check fptype */
          TreeNode* tn = NULL; 
          unsigned char i = 0;
          tn = t->child[0]; /* args */
          while(tn!=NULL) {
            if (tn->type == Integer && bl->fptype[i]==SInt)
              ;
            else if (tn->type == Array && bl->fptype[i]==SArray)
              ;
            else 
              typeError(tn,"args type error");              
            tn = tn->sibling; i++;
          }
          if (bl->fptype[i]!=SNone) typeError(t,"args number error");
          else {
            /* all requirements match, assign frtype */
            ExpType et;
            if (bl->frtype == SArray) et = Array;
            else if (bl->frtype == SInt) et = Integer;
            t->type = et;
          }
        }
      } else {
        fprintf(listing,"error: symbol %s at line %d not found\n",
                t->attr.name, t->lineno);
        printSymTab(listing);
       
      }

      break;
    default:
      break;
    }
    break;
  case StmtK:
    switch(t->kind.stmt) {
    case IfK:
      if((t->child[0]->type != Boolean) &&
         (t->child[0]->type != Integer))
        typeError(t->child[0],"if test is neither Boolean nor Integer");
      break;
    case AssignK:
      if(t->child[0]->type != t->child[1]->type) {
        typeError(t->child[0],"assign A=B, A and B have different type");
      }
      break;
    case ReturnK:
      if(t->child[0] && t->child[0]->type != Integer)
        typeError(t->child[0],"Return non-integer value");
      break;
    case WhileK:
      if((t->child[0]->type != Boolean) &&
         (t->child[0]->type != Integer))
        typeError(t->child[0],"while test is neither Boolean nor Integer");
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void checkNode2(TreeNode* t) {
  checkNode(t);
  exitScope(t);
}
      
/* check type by postorder syntax tree traversal */      
void typeCheck(TreeNode* syntaxTree) {
  location = 0;
  st_root = st_cur = st_prev = NULL;
  cur_scope = 0;
  traverse(syntaxTree, insertNode, checkNode2);
}


