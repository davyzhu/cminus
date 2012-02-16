/**************************/
/* File: parse.c          */
/**************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token;  /* current token */

/* function prototypes for recursive calls, with rule number in BNF grammer */
static TreeNode* declaration_list(void); //2
static TreeNode* declaration(void); //3
static TreeNode* var_declaration(void); //4
//static TreeNode* fun_declaration(void); //6
static TreeNode* params(void); //7
static TreeNode* param_list(void); //8
static TreeNode* param(void); //9
static TreeNode* compound_stmt(void); //10
static TreeNode* local_declarations(void); //11
static TreeNode* statement_list(void); //12
static TreeNode* statement(void); //13
static TreeNode* expression_stmt(void); //14
static TreeNode* if_stmt(void); //15
static TreeNode* while_stmt(void); //16
static TreeNode* return_stmt(void); //17
static TreeNode* exp(void); //18
static TreeNode* var(void); //19
static TreeNode* simple_exp(void);//20
static TreeNode* additive_exp(void);//22
static TreeNode* term(void);//24
static TreeNode* factor(void);//26
static TreeNode* args(void);//28

static void syntaxError(char* message) {
  fprintf(listing, "\n>>> ");
  fprintf(listing, "Syntax error at line %d: %s", lineno, message);
  //Error = TRUE;
}

static void match(TokenType expected) {
  if (token == expected)
    token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token, tokenString);
    fprintf(listing,"       ");
  }
}

TreeNode* declaration_list(void) { //2
  TreeNode* t = declaration();
  TreeNode* p = t;
  while(token != ENDFILE) { // shall I list follow set
    TreeNode* q;
    q = declaration();
    if (q!=NULL){
      if(t==NULL) t=p=q;
      else {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode* declaration(void) { //3
  TreeNode* t;
  ExpType type_specifier;
  char* id;
  int num;
  
  if (token==INT) 
    type_specifier = Integer;
  else if (token==VOID)
    type_specifier = Void;
  match(token);
  if (token==ID) 
    id = copyString(tokenString);
  match(ID);
  
  t = newDecl(type_specifier, id);
  switch(token) {
  case SEMI: 
    t->kind.decl = VarK;
    t->arraynum = 0;
    match(SEMI);
    break;
  case LBRACK: 
    match(LBRACK);
    t->kind.decl = VarK;
    if (token==NUM)
      t->arraynum = atoi(tokenString);
    match(NUM);
    match(RBRACK);
    match(SEMI);
    break;
  case LPAREN:
    match(LPAREN);
    t->kind.decl = FunK;
    t->child[0] = params();
    match(RPAREN);
    t->child[1] = compound_stmt();
  }
  return t;
}

TreeNode* var_declaration(void) { //4
  TreeNode* t;
  ExpType type_specifier;
  char* id;

  if (token==INT) 
    type_specifier = Integer;
  else if (token==VOID)
    type_specifier = Void;
  else {
    syntaxError("unexpected token -> ");
    printToken(token, tokenString);
    fprintf(listing,"       ");
  }
  match(token);
  if (token==ID) 
    id = copyString(tokenString);
  match(ID);
  t = newDecl(type_specifier, id);
  if (token==LBRACK) {
    match(LBRACK);
    t->arraynum = atoi(tokenString);
    match(NUM);
    match(RBRACK);
  }
  match(SEMI);
  return t;
}


TreeNode* params(void) { //7
  TreeNode* t = NULL;
  if (token==VOID) {
    match(VOID);
    t = newParam(Void,"void");
  } else
    t = param_list();
  return t;
}

TreeNode* param_list(void) { //8
  TreeNode* t = param();
  TreeNode* p = t;
  while(token == COMMA) {
    TreeNode* q;
    match(COMMA);
    q = param();
    if (q!=NULL) {
      if (t==NULL) t=p=q;
      else {/* now p cannot be NULL either */
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode* param() { //9
  TreeNode* t ;
  ExpType et;
  unsigned int arraynum = 0;
  switch(token) {
  case VOID: 
    et = Void;
    match(VOID);
    break;
  case INT: 
    et = Integer;
    match(INT);
    break;
  default: 
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    token=getToken();
    break;
  }
  
  t = newParam(et, copyString(tokenString));
  match(ID);
  if (token == LBRACK){
    match(LBRACK);
    match(RBRACK);
    arraynum = 1;
  }
  t->arraynum = arraynum;
  return t;
}

TreeNode* compound_stmt(void) { //10
  TreeNode* t = newCompStmtNode();
  if(token == LBRACE) {
    match(LBRACE);
  }
  
  if(token==RBRACE) {
    t->child[0] = NULL;
    t->child[1] = NULL;
    match(RBRACE);
    return t;
  }
  else if((token == VOID) || (token == INT))
    t->child[0] = local_declarations();

  if(token==RBRACE) {
    t->child[1] = NULL;
    match(RBRACE);
    return t;
  }
  t->child[1] = statement_list();
  match(RBRACE);
  return t;
}

TreeNode* local_declarations(void) { //11
  TreeNode* t=NULL;
  TreeNode* p = t;
  while((token==INT) || (token==VOID)) {
    TreeNode* q;
    q = var_declaration();
    if (q!=NULL) {
      if (t==NULL) t=p=q;
      else {/* now p cannot be NULL either */
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}


TreeNode* statement_list(void) { //12
  TreeNode* t = statement();
  TreeNode* p = t;
  while((token==LBRACE) || (token==IF)
        || (token==WHILE) || (token==RETURN)
        || (token==ID) || (token==NUM)
        || (token==LPAREN) || (token==SEMI)){
    TreeNode* q;
    q = statement();
    if (q!=NULL) {
      if (t==NULL) t=p=q;
      else {/* now p cannot be NULL either */
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode* statement(void) { //13
  TreeNode* t = NULL;
  switch(token) {
  case LBRACE: t=compound_stmt(); break;
  case IF: t=if_stmt(); break;
  case WHILE: t=while_stmt(); break;
  case RETURN: t=return_stmt(); break;
  case ID: 
  case NUM:
  case LPAREN: 
  case SEMI: t=expression_stmt(); break;
  default: 
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    token=getToken();
    break;
  }
  return t;
}

TreeNode* expression_stmt(void) { //14
  TreeNode* t = NULL;
  if(token==SEMI) match(SEMI);
  else {
    t = exp();
    match(SEMI);
  }
  return t;
}

TreeNode* if_stmt(void) { //15
  TreeNode* t = newStmtNode(IfK);
  match(IF);
  match(LPAREN);
  if(t!=NULL) t->child[0] = exp();
  match(RPAREN);
  if(t!=NULL) t->child[1] = statement();
  if(token==ELSE) {
    match(ELSE);
    if(t!=NULL) t->child[2] = statement();
  }
  return t;
}

TreeNode* while_stmt(void) { //16
  TreeNode* t = newStmtNode(WhileK);
  match(WHILE);
  match(LPAREN);
  if(t!=NULL) t->child[0] = exp();
  match(RPAREN);
  if(t!=NULL) t->child[1] = statement();
  return t;
}

TreeNode* return_stmt(void) { //17
  TreeNode* t = newStmtNode(ReturnK);
  match(RETURN);
  if (token!=SEMI) {
    t->child[0] = exp();
  }
  match(SEMI);
  return t;
}
 
static TreeNode* exp(void){ //18
  TreeNode* t;
  TreeNode* tc;
  BacktrackPoint btp;
  getbtp(&btp, token); /* get backtrack point */
  if (token==ID) {
    tc = var();
  }
  if (token==ASSIGN) {
    match(ASSIGN);
    t = newStmtNode(AssignK);
    t->child[0] = tc;
    t->child[1] = exp();
  } else {
    /* backtrack to entrance of exp() */
    token = setbtp(&btp);
    t = simple_exp();
  }
  return t;
}

static TreeNode* var(void) { //19
  TreeNode* t = newExpNode(IdK);
  if (token==ID) {
    t->attr.name = copyString(tokenString);
    t->arraynum = 0;
    match(ID);
  }
  if (token==LBRACK) {
    match(LBRACK);
    t->arraynum = 1;
    t->child[0] = exp();
    match(RBRACK);
  }
  return t;
}

TreeNode* simple_exp(void) { //20
  TreeNode* t = additive_exp();
  if((token==NEQ) || (token==EQ) ||
     (token==LT) || (token==LTEQ) ||
     (token==MT) || (token==MTEQ)) {
    TreeNode* p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if(t!=NULL)
      t->child[1] = additive_exp();
  }
  return t;
}

TreeNode* additive_exp(void) { //22
  TreeNode* t = term();
  while ((token==PLUS) || (token==MINUS)) {
    TreeNode* p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode* term(void) { //24
  TreeNode* t = factor();
  while((token==TIMES) || (token==OVER)) {
    TreeNode*p = newExpNode(OpK);
    if(p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode* factor(void) { //26
  TreeNode* t = NULL;
  char* s = NULL;
  switch(token) {
  case NUM:
    t = newExpNode(ConstK);
    if ((t!=NULL) && (token==NUM))
      t->attr.val = atoi(tokenString);
    match(NUM);
    break;
  case ID:
    //t = newExpNode(IdK);
    if (token==ID) {
      s = copyString(tokenString);
      match(ID);
    }
    if (token==LPAREN) {
      t = newExpNode(CallK);
      match(LPAREN);
      if (token != RPAREN) 
        t->child[0] = args();
      match(RPAREN);
    } else if (token==LBRACK) {
      t = newExpNode(IdK);
      match(LBRACK);
      if (token != RPAREN) 
        t->child[0] = exp();
      match(RBRACK);
    } else {
      t = newExpNode(IdK);
    }
    t->attr.name = s;

    break;
  case LPAREN:
    match(LPAREN);
    t = exp();
    match(RPAREN);
    break;
  default:
    syntaxError("unexpected token -> ");
    printToken(token, tokenString);
    token = getToken();
    break;
  }
  return t;
}

TreeNode* args(void) { //28
  TreeNode* t = exp();
  TreeNode* p = t;
  while(token==COMMA) {
    TreeNode* q;
    match(COMMA);
    q = exp();
    if (q!=NULL) {
      if (t==NULL) t=p=q;
      else {/* now p cannot be NULL either */
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}


TreeNode* parse() {
  TreeNode* t;
  token = getToken();
  t = declaration_list();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}


