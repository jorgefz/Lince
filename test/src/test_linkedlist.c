
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/containers/linkedlist.h"

void test_linkedlist(void **state){
    (void)state;
    
    // Create empty node
    listnode_t* node = list_create(NULL);
    assert_non_null(node);
    assert_null(node->prev);
    assert_null(node->next);
    assert_null(node->data);
    list_destroy(node);

    // Create node
    int data = 123;
    node = list_create(&data);

    assert_non_null(node);
    assert_null(node->prev);
    assert_null(node->next);
    assert_true((int*)node->data == &data);

    // Retrieve head and tail
    listnode_t* head = list_head(node);
    listnode_t* tail = list_tail(node);
    assert_true(node == head);
    assert_true(node == tail);
    
    // Push node to front
    int data_front = 1;
    head = list_push_front(node, &data_front);

    assert_non_null(head);
    assert_true(head->next == node);
    assert_null(head->prev);
    assert_null(node->next);
    assert_true(node->prev == head);
    assert_true((int*)head->data == &data_front);
    assert_true((int*)head->next->data == &data);

    // Push node to back
    int data_back = 3;
    tail = list_push_back(node, &data_back);

    assert_non_null(tail);
    assert_true(tail->prev == node);
    assert_true(node->next == tail);
    assert_true(list_size(tail) == 3);
    assert_true((int*)tail->data == &data_back);

    // Insert node
    int data_insert = -256;
    listnode_t* ret = list_insert(tail, &data_insert);

    assert_non_null(ret);
    assert_true(ret->prev == node);
    assert_true(ret->next == tail);
    assert_true((int*)ret->data == &data_insert);
    assert_true(list_size(head) == 4);

    // Get node at index
    listnode_t* node_at = list_node_at(head, 1);
    assert_non_null(node_at);
    assert_true(node_at == node);
    
    // Insert node at index
    int data_insert_at = 0x0FFFFFFF;
    listnode_t* insert_at = list_insert_at(head, 1, &data_insert_at);

    assert_non_null(insert_at);
    assert_true(insert_at->prev == head);
    assert_true(insert_at->next == node_at);
    assert_true((int*)insert_at->data == &data_insert_at);
    assert_true(list_size(head) == 5);

    // Remove node
    list_remove(head);
    assert_true(list_size(node));
    assert_true(list_head(node) != head);

    list_remove(tail);
    assert_true(list_size(node) == 3);
    assert_true(list_tail(node) != tail);
    
    list_remove(insert_at);
    assert_true(list_size(node) == 2);
    assert_true(list_head(node)->next == list_tail(node));
    assert_true(list_tail(node)->prev == list_head(node));

    list_destroy(node);
}
