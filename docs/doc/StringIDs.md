# String IDs

Asset names in the asset cache, however, are not stored as strings. This is because comparing strings is slow, as you might have two compare one character at a time.

Instead, the asset cache uses String IDs, which are essentially integers. Integers are much faster to compare, as they take up only one (or sometimes two) CPU instructions. A String ID is essentially the result of hashing a character string.

In Lince, String IDs are represented by the type `LinceSID`, which is esssentially an alias for a 64-bit integer.

You can convert a character string (`string_t` object) into a String ID using the macro `LinceMakeSID(str)`.
There is another helper macro which takes a string literal too: `LinceMakeSIDFromLit("string_literal")`.

For reference, Lince uses the 64-bit FNV1a hashing algorithm for converting character strings into String IDs. 

We can convert character strings into String IDs easily. But given an String ID, how do we know what string it came from?
Given a String ID, you can get the original string using the function `string_t LinceGetSIDName(LinceSID sid)`.

This is possible because Lince stores a global hash table mapping String IDs into their corresponding character strings, which is updated every time `LinceMakeSID` is called. This global cache is created and deleted with the functions `LinceInitSIDCache()` and `LinceUninitSIDCache()`, and it can be directly obtained with the function `hashmap_t* LinceGetSIDCache()`.

