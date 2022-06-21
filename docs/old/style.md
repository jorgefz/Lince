# Code Style Guide

## Naming conventions

### Lince API
All the symbols, functions, and types in the Lince public API should start with "Lince".

### Functions & Structs
Functions as well as structs and other types are written in *CamelCase*. Functions follow the style "Lince"+Action, e.g.: `void LinceGetGameData()` Structs and custom types follow the style "Lince"+Noun, e.g.: `struct LinceApp`

### Variables
Variables, function arguments, and struct members are written using *snake\_case*, e.g.: `int enemy_count = 0` or `app.layer_stack = NULL`.

### Enums
Enums follow the convention "LinceEnumName\_ValueName", e.g.: `LinceEventType_WindowResize`.

### Macros
Macros are written in uppercase *SNAKE\_CASE*, e.g.: `LINCE_WINDOWS`.

## Formatting

### Line length
Lines must not be longer than 80 characters

