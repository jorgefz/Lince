# dast
[![License](https://img.shields.io/badge/License-MIT-mediumorchid)](https://www.github.com/jorgefz/lince/blob/main/LICENSE)
[![Tests](https://img.shields.io/badge/Tests-passing-green)]()

DAST (DAta STructures) is C library that includes some commonly used data structures:

* Dynamic array (`array_t`): a resizeable contiguous array containing items of the same type.
* String (`string_t`): a thin wrapper around a character array plus a length.
* Hashmap (`hashmap_t`): a hash table mapping one one data type to another.

Features:

* Fast
* Simple
* Lightweight
* Written in C99
* Works without the C standard library
* Works on both 32bit and 64bit architectures
* Support for user-defined memory allocation functions
* Fully tested with CMocka (https://cmocka.org/)
* Premake5 build scripts included (https://premake.github.io/)


## Installation / Compilation

A Premake5 file is included to compile DAST into a static library.
To use it,
1. Run `premake5 gmake` on Linux or `premake5 vs2022` on Windows (with Visual Studio 2022 installed).
2. Compile the code with `make dast` on Linux or by opening the `dast.sln` solution file on VS2022 on Windows.
    * There are four possible configurations: 64bit with std lib (`arch64`), 64bit with no std lib (`arch64-nostd`), 32bit with std lib (`arch32`) and 32bit with no std lib (`arch32-nostd`).
    * To compile 32-bit version on 64-bit Linux, you might need to install the package `libc6-dev-i386`:
        * `sudo apt install libc6-dev-i386`
    * There is also a helper script called "winmake.bat" to compile the project on Windows without opening VStudio. 
3. The output static libraries for each configuration will be found in the `bin` folder.

To compile manually, include all files in the `include` folder and compile all source files in the `src` folder.

Once compiled, link your project against the produced static library and, in your code, include the `dast.h `header.
```c
#include <dast.h>
```

To run the tests,
1. Install CMocka following the documentation at https://cmocka.org/.
    * To enable CMocka testing support for 32-bit version of `dast` on 64-bit Linux, you might need to install the 32-bit version of CMocka on your system. For instance, for Ubuntu, you might need to run the following commands:
        * `sudo dpkg --add-architecture i386`
        * `sudo apt-get update`
        * `sudo apt install libcmocka-dev:i386`
2. Compile all files inside the `test` folder, linking against `dast` and `cmocka`.
    * With the included premake5 script, simply run `premake5 gmake` (or `premake5 vs2022` on Windows), and compile the project `test` with either Make (`make test`) or VStudio 2022. These projects can also generate executables for the tests for both 64bit and 32bit architectures as well as including/excluding the C standard library (e.g. `make test config=arch32-nostd`).
3. Execute the resulting binaries to run the tests.


## Documentation

The functions and macros in the header files have doxygen-style comments, so you can generate documentation for the library with Doxygen.

Simply run `doxygen` and HTML and LaTeX documentation will be generated to a folder named `doxygen`.


## Code examples

### array_t

* Includes many functions to add/remove elements, mirroring `std::vector` in C++ (i.e. push_front, push_back, pop_front, pop_back, insert), as well as forwards and reverse iterators.
* Can store any data type.
* Bounds checking.
* Supports per-object user-defined allocation functions. 
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
* Support for per-object user-defined allocation functions with no impact on the size of the string object itself.

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
* Additional familty of functions that take string_t objects as keys.
* Default hashing function is 64bit FNV-1A. 
* Supports per-object user-defined allocation functions. 

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

## Function reference

### array

* array_init()
* array_init_custom()
* array_uninit()
* array_copy()
* array_resize()
* array_set()
* array_get()
* array_front()
* array_back()
* array_end()
* array_insert()
* array_push_back()
* array_push_front()
* array_remove()
* array_pop_back()
* array_pop_front()
* array_clear()

### hashmap

* hashmap_FNV1a64_hash()
* hashmap_init()
* hashmap_init_custom()
* hashmap_uninit()
* hashmap_has_keyb()
* hashmap_has_key()
* hashmap_getb()
* hashmap_get()
* hashmap_setb()
* hashmap_set()
* hashmap_resize()
* hashmap_iterb()
* hashmap_iter()

### string

* string_ok()
* string_get_alloc()
* string_scoped_lit()
* string_scoped()
* string_from_literal_custom()
* string_from_literal()
* string_from_chars_custom()
* string_from_chars()
* string_from_len_custom()
* string_from_len()
* string_from_fmt_custom()
* string_from_fmt()
* string_copy_custom()
* string_copy()
* string_copy_scoped()
* string_free()

### allocator

* dast_get_alloc()
* dast_set_alloc()
* dast_memeq()
* dast_memset()
* dast_memcpy()
* dast_memmove()

