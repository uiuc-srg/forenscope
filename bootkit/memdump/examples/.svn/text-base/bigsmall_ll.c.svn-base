#include <stdlib.h>
#include <stdio.h>

#define SPACE 32

struct small_node {
	int val;
	struct small_node *next, *prev;
};

struct big_node {
	struct big_node *next, *prev;
	int val;
	int val2;
	char name[16];
	struct small_node *other;
};

typedef struct small_node small_node;
typedef struct big_node big_node;

void insert_big (big_node **head, big_node *elem) {
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

void insert_small (small_node **head, small_node *elem) {
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

void printout_big(big_node * head, int level) {
	big_node *index = head;
   	printf("Node(%d) %p -> %p; %d\n", level, index, index->next, index->val);
   	printf("Node(%d) %p -> %p; %d\n", level++, index, index->prev, index->val);
   	index = index->next;
   	while (index != head) {
   		printf("Node(%d) %p -> %p; %d\n", level, index, index->next, index->val);
   		printf("Node(%d) %p -> %p; %d\n", level++, index, index->prev, index->val);
   		index = index->next;
	}
}

void printout_small(small_node * head, int level) {
	small_node *index = head;
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
	big_node *curr, *head;
	small_node *othernode, *otherhead;
	void *spacer;
	int i;
	char dummy;

	head = NULL;
	otherhead = NULL;

	for (i=1; i<=20; i++) {
		spacer = (void*)malloc(SPACE);
		curr = (big_node*)malloc(sizeof(big_node));

		curr->val = rand() % 100;
		if (curr->val > 50) {
			spacer = (void*)malloc(SPACE);
			othernode = (small_node*)malloc(sizeof(small_node));
			othernode->val = (int)0xdeadbeef;
			curr->other = othernode;
			insert_small(&otherhead, othernode);
		} else {
			curr->other = NULL;
		}
		insert_big(&head, curr);
	}

	printout_big(head, 0);
	printout_small(otherhead, 0);

	printf("Press enter to exit\n");
	scanf("%c", &dummy);

	return 0;
}

