# Code Style Guide

## Lince API
All the symbols, functions, and types in the Lince public API should start with "Lince".

## Functions & Structs
Functions as well as structs and typedefs follow *PascalCase*. Functions follow the naming convetion "LinceVerb(Noun)", e.g.: `void LinceGetGameData()`. Structs and custom types follow the style "LinceNoun", e.g.: `struct LinceApp`.

## Variables
Variables, function arguments, and struct members are written using *snake\_case*, e.g.: `int enemy_count = 0` or `app.layer_stack = NULL`.

## Enums
Enums should be named with the name of the enum typedef followed by the name of the constant, both in PascalCase but separated by an underscore: "LinceEnum\_Constant", e.g.: `LinceEventType_WindowResize`. This tells you which category the constant belongs to.

## Macros
Macros are written in uppercase *SNAKE\_CASE*, e.g.: `LINCE_WINDOWS`.

## Exceptions

The only exceptions, apart from third party libraries, are the data structures in `lince/containers`, such as `array_t`, `listnode_t`, and `hashmap_t`. These use the snake\_case convention for both typedefs and functions. 


