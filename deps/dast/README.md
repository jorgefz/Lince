# dast

DAta STructures (DAST).  A collection of some commonly used data structures written in C.

* Dynamic array (`array_t`): a resizeable contiguous array containing items of the same type.
* String (`string_t`): a thin wrapper for a character array plus a length.
* Hashmap (`hashmap_t`): a hash table mapping one one data type to another.

Features:

* Fast
* Simple
* Lightweight
* Written in C99
* Fully tested with CMocka (https://cmocka.org/)
* Premake5 build scripts included (https://premake.github.io/)
* Works on both 32bit and 64bit architectures
* Works without the C standard library
* Support for user-defined memory allocation functions

## Installation / Compilation

A Premake5 file is included to compile DAST into a static library.
To use it,
1. Run `premake5 gmake2` on Linux or `premake5 vs2022` on Windows (with Visual Studio 2022 installed).
2. Compile the code with `make dast` on Linux or by opening the `dast.sln` solution file on VS2022 on Windows.
    * There are four possible configurations: 64bit with std lib (`arch64`), 64bit with no std lib (`arch64-nostd`), 32bit with std lib (`arch32`) and 32bit with no std lib (`arch32-nostd`).
3. The output static libraries for each configuration will be found in the `bin` folder.

To compile manually, include all files in the `include` folder and compile all source files in the `src` folder.

Once compiled, link your project against the produced static library and, in your code, include the `dast.h `header.
```c
#include <dast.h>
```

To run the tests,
1. Install CMocka following the documentation at https://cmocka.org/.
2. Compile all files inside the `test` folder, linking against `dast` and `cmocka`.
    * With the included premake5 script, simply run `premake5 gmake2` (or `premake5 vs2022` on Windows), and compile the project `test` with either Make (`make test`) or VStudio 2022. These projects can also generate executables for the tests for both 64bit and 32bit architectures as well as including/excluding the C standard library.
3. Execute the resulting binaries.

## Code examples

### array_t

* Includes many functions to add/remove elements, mirroring `std::vector` in C++ (i.e. push_front, push_back, pop_front, pop_back, insert), as well as forwards and reverse iterators.
* Can store any data type.
* Bounds checking.
* Supports user-defined allocation functions. 
* No macros whatsoever.

```c
array_t data;
array_init(&data, sizeof(float));

float value = 1.0f;
array_push_back(&data, &value);

float ret = *(float*)array_get(&data, 0);
assert(ret == value);
assert(data.size == 1);

array_uninit(&data);
```

### string_t

* Thin wrapper around a pointer to a char array and a length. Only 16 bytes on 64bit architectures (8 bytes on 32bit).
* Can create string_t objects from string literals, character arrays, and formatted strings. The underlying character array also null-terminated, so use like any other C char array but know its length at all times.
* Support for scoped strings, which are automatically freed at the end of the current scope.
* Support for user-defined allocation functions (malloc and free) with no impact on the size of the string object itself.

```c
// Allocated on the heap, must be freed
string_t s1 = string_from_literal("Allocated string");

printf("%s\n", s1.str);
printf("length %d\n", (int)s.len);

string_free(&s1);

// Allocated on the stack, automatically freed at the end of the current scope
string_t s2 = string_scoped_lit("Local string");

// Formatted using variadic arguments
string_t s3 = string_from_fmt("%f %d %s", 10.0f, -1, "hello");
string_free(&s3);

// Using custom allocation functions
dast_allocator_t my_alloc = {.alloc = my_malloc, .realloc = my_realloc, .free = my_free};
string_t s4 = string_from_literal_custom("Custom allocated string", my_alloc); // will use `my_alloc`
string_free(&s4); // will use 'my_free'
```

### hashmap_t

* Stores any data type, including different types for different keys.
* Any data type can be used as a keys, with support for used-defined key comparison and hashing functions.
* Provide the intial capacity of the hashmap on initialisation, avoiding the time cost of having to resize constantly when adding many elements.
* Key-value pairs of keys with colliding hashes are stored using a linked list.
* Additional functions that take string_t objects as keys.
* Default hashing function is 64bit FNV-1A. 

```c
hashmap_t map;
hashmap_init(&map, 10); // Starting capacity will be next prime number after given value

// Use string_t as key
string_t key = string_scoped_lit("key");
float value = 100.0f;
hashmap_set(&map, key, &value);

float ret = *(float*)hashmap_get(&map, key);
assert(ret == value);

// Use anything as a key
uint64_t custom_key = 100;
size_t custom_key_len = sizeof(uint64_t);
float value2 = 200.0f;
hashmap_setb(&map, &custom_key, custom_key_len, value2);

float ret2 = *(float*)hashmap_getb(&map, &custom_key, custom_key_len);
assert(ret2 == value2);
hashmap_uninit(&map);

// Custom allocation functions
dast_allocator_t my_alloc = {.alloc = my_malloc, .realloc = my_realloc, .free = my_free};
hashmap_t map2;
hashmap_init_custom(&map, 10, my_alloc, dast_null, dast_null);
hashmap_uninit(&map2);

// Custom hashing and key comparison functions (for custom keys)
dast_bool my_cmp(const void* a, const void* b, dast_sz len) { return *(uint64_t*)a == *(uint64_t*)b; }
dast_u64  my_hash(const void* key, dast_sz key_len) { ... }

hashmap_t map3;
hashmap_init_custom(&map3, 10, my_alloc, my_hash, my_cmp);
hashmap_uninit(&map3);
```

