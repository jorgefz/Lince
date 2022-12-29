
/*

Implementation of a doubly-linked list.

*/


#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <inttypes.h>

typedef struct listnode_container {
    void* data;
    struct listnode_container* next;
    struct listnode_container* prev;
} listnode_t;


/* Initialises and allocates a new unlinked node */
listnode_t* list_create(void* data);

/* Deletes all the nodes in a list */
void list_destroy(listnode_t* head);

/* Returns the number of elements in the list */
uint32_t list_size(listnode_t* node);

/* Returns the node at the given index from the given node */
listnode_t* list_node_at(listnode_t* node, uint32_t index);

/* Returns the first node (head) of a list */
listnode_t* list_head(listnode_t* node);

/* Returns the last node of a list */
listnode_t* list_tail(listnode_t* node);

/* Places a new node before the given node */
listnode_t* list_insert(listnode_t* node, void* data);

/* Places a new node before the node at the given index */
listnode_t* list_insert_at(listnode_t* head, uint32_t index, void* data);

/*
Places a node at the front of a list.
Note that the new head is returned.
*/
listnode_t* list_push_front(listnode_t* node, void* data);

/* Places a node at the end of a list */
listnode_t* list_push_back(listnode_t* node, void* data);

/* Discards a node from the list */
void list_remove(listnode_t* node);


#endif /* LINKED_LIST_H */