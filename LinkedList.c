#include <stdio.h>
#include <stdlib.h>

struct ListNode {
    int value;
    struct ListNode* next;
};

void insert(struct ListNode* list, int val) {
	if (list == NULL) {
		list = malloc(sizeof(struct ListNode));
		list->value = val;
		list->next = NULL;
		printf("list value is %d\n", list->value);
	} else {
		while (list->next != NULL) {
			list = list->next;
		}
		list->next = malloc(sizeof(struct ListNode));
		list->next->value = val;
		list->next->next = NULL;
	}
}

void delete(struct ListNode* list) {
    struct ListNode* newHead = list->next;
    free(list);
    list = newHead;
}
