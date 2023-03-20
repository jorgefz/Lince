
/** @file linkedlist.h
* Implementation of a doubly-linked list.
*/

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <inttypes.h>

/** @struct listnode_t
* Entry of a linked list.
*/
typedef struct listnode_container {
    void* data;                       ///< User-data held by the node
    struct listnode_container* next;  ///< Next node in the list
    struct listnode_container* prev;  ///< Previous node in the list
} listnode_t;


/** @brief Initialises and allocates a new unlinked node
* The node data is not copied, only the pointer is stored.
* Ensure your data is freed by iterating the list before destroying the nodes. 
*/
listnode_t* list_create(void* data);

/** @brief Deletes all the nodes in a list */
void list_destroy(listnode_t* head);

/** @brief Returns the number of elements in the list */
uint32_t list_size(listnode_t* node);

/** @brief Returns the node at the given index from the given node */
listnode_t* list_node_at(listnode_t* node, uint32_t index);

/** @brief Returns the first node (head) of a list */
listnode_t* list_head(listnode_t* node);

/** @brief Returns the last node of a list */
listnode_t* list_tail(listnode_t* node);

/** @brief Places a new node before the given node */
listnode_t* list_insert(listnode_t* node, void* data);

/** @brief Places a new node before the node at the given index */
listnode_t* list_insert_at(listnode_t* head, uint32_t index, void* data);

/** @brief Places a node at the front of a list.
* Note that the new head is returned.
*/
listnode_t* list_push_front(listnode_t* node, void* data);

/** @brief Places a node at the end of a list */
listnode_t* list_push_back(listnode_t* node, void* data);

/** @brief Discards a node from the list */
void list_remove(listnode_t* node);


#endif /* LINKED_LIST_H */