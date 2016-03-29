
#include<stdlib.h>
#include<stdio.h>

#define SPACE 0

struct tree_el {
   int val;
   struct tree_el * right, * left;
};
typedef struct tree_el tree_node;

struct dll_node {
	int val;
	struct dll_node *next, *prev;
	struct dll_node *other;
};
typedef struct dll_node dll_node;

struct ll_node {
	int val;
	struct ll_node *next, *prev;
};
typedef struct ll_node ll_node;

void insert_tree(tree_node ** tree, tree_node * item) {
   if(!(*tree)) {
      *tree = item;
      return;
   }
   if(item->val<(*tree)->val)
      insert_tree(&(*tree)->left, item);
   else if(item->val>(*tree)->val)
      insert_tree(&(*tree)->right, item);
}

void insert_dll (dll_node **head, dll_node *elem) {
	if (!(*head)) {
		*head = elem;
		elem->next = elem;
		elem->prev = elem;
	}
	else {
		(*head)->prev->next = elem;
		elem->prev = (*head)->prev;
		(*head)->prev = elem;
		elem->next = *head;
		*head = elem; //optional for insert at front/back
	}
}	

void printout_dll(dll_node * head, int level) {
	dll_node *index = head;
   	printf("Node(%d) %p -> %p; %d\n", level, index, index->next, index->val);
   	printf("Node(%d) %p -> %p; %d\n", level++, index, index->prev, index->val);
   	index = index->next;
   	while (index != head) {
   		printf("Node(%d) %p -> %p; %d\n", level, index, index->next, index->val);
   		printf("Node(%d) %p -> %p; %d\n", level++, index, index->prev, index->val);
   		index = index->next;
	}
}

void printout_tree(tree_node * tree, tree_node* parent, int level) {
   printf("Node(%d) %p -> %p; %d %x\n", level, parent, tree, tree->val, tree->val);
   if(tree->left) printout_tree(tree->left, tree, level+1);
   if(tree->right) printout_tree(tree->right, tree, level+1);
}

void insert_ll (ll_node **head, ll_node *elem) {
	if (!(*head)) {
		*head = elem;
		elem->next = elem;
		elem->prev = elem;
	}
	else {
		(*head)->prev->next = elem;
		elem->prev = (*head)->prev;
		(*head)->prev = elem;
		elem->next = *head;
		*head = elem; //optional for insert at front/back
	}
}	

void printout_ll(ll_node * head, int level) {
	ll_node *index = head;
   	printf("Node(%d) %p -> %p; %d\n", level, index, index->next, index->val);
   	printf("Node(%d) %p -> %p; %d\n", level++, index, index->prev, index->val);
   	index = index->next;
   	while (index != head) {
   		printf("Node(%d) %p -> %p; %d\n", level, index, index->next, index->val);
   		printf("Node(%d) %p -> %p; %d\n", level++, index, index->prev, index->val);
   		index = index->next;
	}
}

void maketree()
{
   tree_node * curr, * root;
   void* spacer;
   int i;

   root = NULL;

   for(i=1;i<=20;i++) {
      spacer =  (void*) malloc(SPACE);
      curr = (tree_node *)malloc(sizeof(tree_node));

      curr->left = curr->right = NULL;
      curr->val = rand() % 10000;
      insert_tree(&root, curr);
   }

   printout_tree(root, root, 0);
}

void make_ll()
{
	ll_node *curr, *head;
	void *spacer;
	int i;
	char dummy;

	head = NULL;

	for (i=1; i<=20; i++) {
		spacer = (void*)malloc(SPACE);
		curr = (ll_node*)malloc(sizeof(ll_node));

		curr->val = rand() % 100;
		insert_ll(&head, curr);
	}

	printout_ll(head, 0);
}

void make_dll()
{
	dll_node *curr, *otherdll_node, *head, *otherhead;
	void *spacer;
	int i;
	char dummy;

	head = NULL;
	otherhead = NULL;

	for (i=1; i<=20; i++) {
		spacer = (void*)malloc(SPACE);
		curr = (dll_node*)malloc(sizeof(dll_node));

		curr->val = rand() % 100;
		if (curr->val > 50) {
			spacer = (void*)malloc(SPACE);
			otherdll_node = (dll_node*)malloc(sizeof(dll_node));
			curr->other = otherdll_node;
			insert_dll(&otherhead, otherdll_node);
		} else {
			curr->other = NULL;
		}
		insert_dll(&head, curr);
	}

	printout_dll(head, 0);
}


int main(int c, char* argv[]) {
   char dummy;

   maketree();
   make_dll();
   make_ll();

   printf("Press enter to exit\n");
   scanf("%c", &dummy);
}
