#include <stdlib.h>
#include <stdio.h>

#define SPACE 32

struct ll_node {
	int val;
	struct ll_node *next, *prev;
};

typedef struct ll_node node;

void insert (node **head, node *elem) {
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

void printout(node * head, int level) {
	node *index = head;
   	printf("Node(%d) %p -> %p; %d\n", level, index, index->next, index->val);
   	printf("Node(%d) %p -> %p; %d\n", level++, index, index->prev, index->val);
   	index = index->next;
   	while (index != head) {
   		printf("Node(%d) %p -> %p; %d\n", level, index, index->next, index->val);
   		printf("Node(%d) %p -> %p; %d\n", level++, index, index->prev, index->val);
   		index = index->next;
	}
}

int main (int argc, char *argv[]) {
	node *curr, *head;
	void *spacer;
	int i;
	char dummy;

	head = NULL;

	for (i=1; i<=20; i++) {
		spacer = (void*)malloc(SPACE);
		curr = (node*)malloc(sizeof(node));

		curr->val = rand() % 100;
		insert(&head, curr);
	}

	printout(head, 0);

	printf("Press enter to exit\n");
	scanf("%c", &dummy);

	return 0;
}
