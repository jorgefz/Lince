
#include "containers/linkedlist.h"

#include <stdlib.h>

/* Initialises and allocates a new unlinked node */
listnode_t* list_create(void* data){
    listnode_t* node = calloc(1, sizeof(listnode_t));
    if(!node) return NULL;
    node->data = data;
    return node;
}

/* Deletes all the nodes in a list */
void list_destroy(listnode_t* head){
    if(!head) return;
    if(head->prev){
        head = list_head(head);
    }
    listnode_t* node = head, *next;
    while(node){
        next = node->next;
        free(node);
        node = next;
    }
}

/* Returns the node at the given index from the given node */
listnode_t* list_node_at(listnode_t* node, uint32_t index){
    if(!node) return NULL;

    while(index-- != 0 && node->next){
        node = node->next;
    }
    if(index != 0) return NULL;
    return node;
}

/* Returns the first node (head) of a list */
listnode_t* list_head(listnode_t* node){
    if(!node) return NULL;
    while(node->prev){
        node = node->prev;
    }
    return node;
}

/* Returns the last node of a list */
listnode_t* list_tail(listnode_t* node){
    if(!node) return NULL;
    while(node->next){
        node = node->next;
    }
    return node;
}

/* Places a new node before the given node */
listnode_t* list_insert(listnode_t* node, void* data){
    listnode_t* new_node = list_create(data);
    if(!new_node) return NULL;

    new_node->next = node;
    new_node->prev = node->prev;

    if(node->prev){
        node->prev->next = new_node;
    }
    node->prev = new_node;

    return new_node;
}

/* Places a new node before the node at the given index */
listnode_t* list_insert_at(listnode_t* head, uint32_t index, void* data){
    listnode_t* node = list_node_at(head, index);
    return list_insert(node, data);
}

/*
Places a node at the front of a list.
Note that the new head is returned.
*/
listnode_t* list_push_front(listnode_t* node, void* data){
    if(!node) return NULL;
    if(node->prev) node = list_head(node);
    listnode_t* head = list_create(data);
    if(!head) return NULL;
    head->next = node;
    node->prev = head;
    return head;
}

/* Places a node at the end of a list */
listnode_t* list_push_back(listnode_t* node, void* data){
    if(!node) return NULL;
    if(node->next) node = list_tail(node);
    node->next = list_create(data);
    return node->next;
}

/* Discards a node from the list */
listnode_t* list_remove(listnode_t* node){
    if(!node) return NULL;
    listnode_t *prev, *next;
    prev = node->prev;
    next = node->next;

    if(prev) prev->next = next;
    if(next) next->prev = prev;
    free(node);

    return prev;
}
