
/*

===== tuple.h =====

A tuple is ...

The types of the elements must be known beforehand in order to retrieve their values.



--- EXAMPLE ---

int main(){

	struct tuple* t = tuple_init();
	if(!t) exit(1);

	tuple_pushb(t, sizeof(double));
	
	double y = 12.345;
	tuple_setp(t, 0, &y);

	printf("%g\n", *(double*)tuple_getp(t, 0))

	tuple_free(t);

	return 0;
}

*/


#ifndef TUPLE_H
#define TUPLE_H

#include <stdlib.h>

struct tuple {
    void* data;       /* stores the tuple elements as raw bytes */
    size_t* offsets;  /* list of cumulative byte offsets between elements */
    size_t elems;     /* number of elements stored in the tuple */
    size_t size;      /* total stored memory in bytes */
};

/*
Initialises a tuple with zero elements and returns a pointer to it.
Parameters: none
Returns:
	tpl:	(struct tuple*) Newly created tuple
Example:
	struct tuple* my_tuple = tuple__init();
*/
struct tuple* tuple_init();

/*
Deallocates a tuple.
Parameters:
	tpl:	(struct tuple*) Tuple to delete
Returns: none
Example:
	tuple_free(my_tuple);
*/
void tuple_free(struct tuple* tpl);

/*
Extends the tuple by one element of a given size in bytes
Parameters:
	tpl:		(struct tuple*) Tuple object
	type_size	(size_t) Size in bytes of the new element
Returns:
	tpl:		(struct tuple*) Tuple object
Example:
	tuple__push(my_tuple, sizeof(int));
*/
struct tuple* tuple_pushb(struct tuple* tpl, size_t type_size);

/*
Provides the value for an element in a tuple via a pointer.
Parameters:
	tpl:	(struct tuple*) Tuple object
	idx:	(size_t) Index of the element to set
	value:	(void*) Pointer to where the value is stored.
Returns:
	tpl:	(struct tuple*) Tuple object
Example:
	float y = 3.14f;
	tuple__set(my_tuple, 0, &y);
*/
struct tuple* tuple_setp(struct tuple* tpl, size_t idx, void* value);

/*
Returns a pointer to the location of an element.
If the index is out of range, it returns NULL.
Parameters:
	tpl:	(struct tuple*) Tuple object
	idx:	(size_t) Index of the element to obtain
Returns:
	ptr:	(void*) Pointer to where the tuple element is stored
Example:
	double z = *(double*)tuple__get(my_tuple, 2);
*/
void* tuple_getp(struct tuple* tpl, size_t idx);

/*
Removes the last element of a tuple.
If the tuple has no elements, it returns NULL.
Parameters:
	tpl:	(struct tuple*) Tuple object
Returns:
	tpl:	(struct tuple*) Tuple object
Example:
	tuple__pop(my_tuple);
*/
struct tuple* tuple_pop(struct tuple* tpl);


/* MACROS */

/*
Initialise a tuple of up to 9 elements by providing it with the types
Example:
	struct tuple* t = tuple_4(int, float, double, size_t);
*/
#define tuple_0() tuple_init()
#define tuple_1(t1) tuple_pushb(tuple_init(), sizeof(t1))
#define tuple_2(t1, t2) tuple_pushb(tuple_1(t1), sizeof(t2))
#define tuple_3(t1, t2, t3) tuple_pushb(tuple_2(t1,t2), sizeof(t3))
#define tuple_4(t1, t2, t3, t4) tuple_pushb(tuple_3(t1,t2,t3), sizeof(t4))
#define tuple_5(t1, t2, t3, t4, t5) tuple_pushb(tuple_4(t1,t2,t3,t4), sizeof(t5))
#define tuple_6(t1, t2, t3, t4, t5, t6) tuple_pushb(tuple_5(t1,t2,t3,t4,t5), sizeof(t6))
#define tuple_7(t1, t2, t3, t4, t5, t6, t7) tuple_pushb(tuple_6(t1,t2,t3,t4,t5, t6), sizeof(t7))
#define tuple_8(t1, t2, t3, t4, t5, t6, t7, t8) tuple_pushb(tuple_7(t1,t2,t3,t4,t5,t6,t7), sizeof(t8))
#define tuple_9(t1, t2, t3, t4, t5, t6, t7, t8, t9) tuple_pushb(tuple_8(t1,t2,t3,t4,t5,t6,t7,t8), sizeof(t9))

#define tuple(N, ...) tuple_##N(__VA_ARGS__)


// #define tuple(N, t1, ...) tuple__push(tuple(N-1, __VA_ARGS__), sizeof(t1));

/*
Provides a value to a tuple element.
If the index is invalid, nothing is done.
Parameters:
	tpl:	(struct tuple*) Tuple object
	idx:	(size_t) index of the tuple element to modify
	type:	([type identifier]) type of the value to set
	value:  ([literal or variable]) value to which to set the tuple element
Example:
	`tuple_set(my_tuple, 0, int, 42);`
*/
#define tuple_set(tpl, idx, type, value) if((size_t)idx < tpl->elems) *(type*)(tpl->data + tpl->offsets[idx]) = value

/*
Returns a tuple element directly.
If the index is invalid, an invalid read will occur.
Use tuple_get_s for a safer alternative where a default value must be provided.
Parameters:
	tpl:	(struct tuple*) Tuple object
	idx:	(size_t) index of the tuple element to obtain
	type:	([type identifier]) type of the value to return
Returns:
	value:  ([variable]) Stack-allocated tuple element
Example:
	`int x = tuple_get(my_tuple, 0, int)`
*/
#define tuple_get(tpl, idx, type) *(type*)(tuple_getp(tpl, idx))

/*
Returns a tuple element directly.
If the index is invalid, the default value is returned.
Parameters:
	tpl:	(struct tuple*) Tuple object
	idx:	(size_t) index of the tuple element to obtain
	type:	([type identifier]) type of the value to return
	def:	([literal or variable]) default value to return if the index is invalid
Returns:
	value:  ([variable]) Stack-allocated tuple element
Example:
	int x = tuple_get(my_tuple, 0, int, -1)
*/
#define tuple_get_s(tpl, idx, type, def) ((size_t)idx < tpl->elems) ? tuple_get(tpl, idx, type) : def


/*
Extends a tuple by one element with the provided value.
Parameters:
	tpl:	(struct tuple*) Tuple object
	type:	([type identifier]) type of the element to append
	value:  ([literal or variable]) Value of the new tuple element
Returns:
	value:  ([variable]) Stack-allocated tuple element
Example:
	`tuple_push(my_tuple, 0, int)`
*/
#define tuple_push(tpl, type, value) do{     	\
    tuple_pushb(tpl, sizeof(type));      		\
    tuple_set(tpl, tpl->elems-1, type, value); 	\
    } while(0)                      			\


#endif /* TUPLE_H */


/* 	IMPLEMENTATION */

#define TUPLE_IMPL

#include <stdio.h>
#include <string.h>


// allocates a new empty tuple
struct tuple* tuple_init(){
    struct tuple* t = NULL;
    t = calloc(1, sizeof(struct tuple));
    if(t == NULL) return NULL;

    // initialise first offset to zero
    t->offsets = calloc(1, sizeof(size_t));
    if(t->offsets == NULL) return NULL;

    return t;
}

// deallocates a tuple
void tuple_free(struct tuple* tpl){
    if(tpl == NULL) return;
    if(tpl->offsets) free(tpl->offsets);
    if(tpl->data) free(tpl->data);
    free(tpl);
}

// appends an element of given type to the tuple
struct tuple* tuple_pushb(struct tuple* tpl, size_t type_size){
    tpl->elems++;
    tpl->size += type_size;

    // extend memory pool
    void* data = realloc(tpl->data, tpl->size);
    if(data == NULL) return NULL;
    tpl->data = data;

    // extend offsets
    size_t* offs = realloc(tpl->offsets, (tpl->elems+1) * sizeof(size_t));
    if(offs == NULL) return NULL;
    offs[tpl->elems] = type_size + offs[tpl->elems-1];
    tpl->offsets = offs;

    return tpl;
}


struct tuple* tuple_setp(struct tuple* tpl, size_t idx, void* value){
    if(idx >= tpl->elems) return NULL;
    memmove(tpl->data + tpl->offsets[idx], value, tpl->offsets[idx+1]);
    return tpl;
}


void* tuple_getp(struct tuple* tpl, size_t idx){
    if(idx >= tpl->elems) return NULL;
    return tpl->data + tpl->offsets[idx];
}


struct tuple* tuple_pop(struct tuple* tpl){
    if(!tpl || tpl->elems == 0) return NULL;
	
    tpl->size -= tpl->offsets[tpl->elems] - tpl->offsets[tpl->elems-1];
    tpl->elems--;

    size_t *mem = realloc(tpl->offsets, (tpl->elems+1) * sizeof(size_t));
    if(!mem) return NULL;
    tpl->offsets = mem;

	if(tpl->size == 0){
		if (tpl->data) free(tpl->data);
		tpl->data = NULL;
		return tpl;
	}

	void* data = realloc(tpl->data, tpl->size);
	if(!data) return NULL;
	tpl->data = data;

	return tpl;
}




