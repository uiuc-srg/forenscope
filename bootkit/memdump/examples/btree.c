#include<stdlib.h>
#include<stdio.h>

#define SPACE 4

struct tree_el {
   int val;
   struct tree_el * right, * left;
};

typedef struct tree_el node;

void insert(node ** tree, node * item) {
   if(!(*tree)) {
      *tree = item;
      return;
   }
   if(item->val<(*tree)->val)
      insert(&(*tree)->left, item);
   else if(item->val>(*tree)->val)
      insert(&(*tree)->right, item);
}

void printout(node * tree, node* parent, int level) {
   printf("Node(%d) %p -> %p; %d %x\n", level, parent, tree, tree->val, tree->val);
   if(tree->left) printout(tree->left, tree, level+1);
   if(tree->right) printout(tree->right, tree, level+1);
}

int main(int c, char* argv[]) {
   node * curr, * root;
   void* spacer;
   int i;
   char dummy;

   root = NULL;

   for(i=1;i<=20;i++) {
      spacer =  (void*) malloc(SPACE);
      curr = (node *)malloc(sizeof(node));

      curr->left = curr->right = NULL;
      curr->val = rand() % 10000;
      insert(&root, curr);
   }

   printout(root, root, 0);

   printf("Press enter to exit\n");
   scanf("%c", &dummy);
}
