#ifndef _UTIL_H_
#define _UTIL_H_

void printToken( TokenType, const char* );
TreeNode* newDecl(ExpType, char*);
TreeNode* newParam(ExpType, char*);
TreeNode* newCompStmtNode();
TreeNode* newStmtNode(StmtKind);
TreeNode* newExpNode(ExpKind);
char* copyString(char* );
void printTree(TreeNode*);

//void printToken( char , const char* );

#endif
