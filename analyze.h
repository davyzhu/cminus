#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/* construct symbol table by preorder traversal of syntax tree */
void buildSymtab(TreeNode*);

/* check type by a postorder syntax tree traversal */
void typeCheck(TreeNode*);

#endif
