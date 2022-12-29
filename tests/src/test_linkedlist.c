#include "lince/containers/linkedlist.h"

#include "test.h"
#include "targets.h"

void scan_list(listnode_t* node){

    node = list_head(node);

    while(node){
        if(node->data){
            printf("%d", *(int*)(node->data));
        } else {
            printf("(null)");
        }

        if(node->next){
            if(node->next->prev){
                printf(" <-> ");
            } else {
                printf(" -> ");
            }
        }
        node = node->next;
    }
    printf("\n");
}


int test_linkedlist_performance(){

    int first_value = -1;
	listnode_t* head = list_create(&first_value);

	TEST_CLOCK_START(time);
    int end = 10000;
	for(int i = 0; i != end; ++i){
        list_push_back(head, NULL);
	}
	long int n_op = end;
	TEST_CLOCK_END(time, n_op);
	
    list_destroy(head);

	return TEST_PASS;
}

int test_linkedlist(){
    
    // list_create(empty)
    listnode_t* node = list_create(NULL);
    TEST_ASSERT(node && !node->prev && !node->next && !node->data,
        "Failed to initialise empty node");
    list_destroy(node);

    // list_create(data)
    int data = 123;
    node = list_create(&data);
    TEST_ASSERT(node && !node->prev && !node->next && *(int*)(node->data)==data,
        "Failed to initialise node with data");

    // list_head, list_tail
    listnode_t* head = list_head(node);
    listnode_t* tail = list_tail(node);
    TEST_ASSERT(node == head, "Failed to retrieve list head");
    TEST_ASSERT(node == tail, "Failed to retrieve list tail");

    // list_push_front
    int data_front = 1;
    head = list_push_front(node, &data_front);
    TEST_ASSERT(
        head && *(int*)(head->data) == data_front
        && head->next && head->next == node && *(int*)(head->next->data) == data
        && !head->prev && node->prev == head && !node->next,
        "Failed to add new node at front"
    );

    // list_push_back
    int data_back = 3;
    tail = list_push_back(node, &data_back);
    TEST_ASSERT(
        tail && tail->prev == node && tail->prev->next == tail
        && list_size(tail) == 3 && *(int*)(tail->data) == data_back,
        "Failed to add new node at back"
    );

    // list_insert
    int data_insert = -256;
    listnode_t* ret = list_insert(tail, &data_insert);
    TEST_ASSERT(
        ret && ret->prev == node && ret->next == tail
        && *(int*)(ret->data) == data_insert && list_size(head) == 4,
        "Failed to insert node"
    );

    // list_node_at
    listnode_t* node_at = list_node_at(head, 1);
    TEST_ASSERT(node_at && node_at == node, "Failed to retrieve node at index");
    
    // list_insert_at
    int data_insert_at = 0x0FFFFFFF;
    listnode_t* insert_at = list_insert_at(head, 1, &data_insert_at);
    TEST_ASSERT(
        insert_at && insert_at->prev == head && insert_at->next == node_at
        && *(int*)(insert_at->data) == data_insert_at && list_size(head) == 5,
        "Failed to insert node at index");
    
    // list_remove
    list_remove(head);
    head = insert_at;
    TEST_ASSERT(list_size(head) == 4 && insert_at->prev == NULL,
        "Failed to remove head");

    list_remove(tail);
    tail = ret;
    TEST_ASSERT(list_size(head) == 3 && ret->next == NULL,
        "Failed to remove tail");
    
    list_remove(insert_at);
    head = node;
    TEST_ASSERT(list_size(head) == 2 && head->next == tail && tail->prev == head,
        "Failed to remove tail");

    list_destroy(head);

    return TEST_PASS;
}
