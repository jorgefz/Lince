# Development Ideas

## Lince Engine

### Serialise scenes and entities
In order to save scenes (collections of entities) to file,
and them load them up again, we need to provide the engine
with a series of saving and loading callbacks for each component.

This could be enclosed in a _ComponentSerial_ structure, which contains these two functions:
```c
struct ComponentSerial {
    void (*on_save)(FILE* file, void* component_data);
    void* (*on_load)(FILE* file);
};
```
We could let each component define their own callbacks for serialising (saving) and deserialising (loading), and then call them in turn when saving and loading scenes.

These callbacks could be stored and accessed easily using the string representation of the components' names with a hashmap:
```c
hashmap_t<char*, ComponentSerial> serial_callbacks;
```

The exact format to encode the component data is yet to be decided, but for testing, JSON would be more than sufficient. In this format, each entity could be named via one of its components, a 'TagComponent', which would hold a string name unique to that entity. In JSON, this would be akin to:
```
// scene1.txt
"entities" : {
    "Entity1" : [
        "Component1" : {
            // custom data described by serial callback
        },
        "Component2" : { ... }
    ],
    "Entity2 : { ... }
}
```

One issue is how to serialise pointers and callbacks within components.
Maybe we can include references to global resource managers, or some other approach.


## The Big RPG game

### Language
The game would have several starting regions or kingdoms. Each region would have its own language.

The player could choose a region in which to start. The language of this region would be the player's native region, and thus they would be able to understand it. In this case, their native language would simply appear as English.

During the game, text in other languages would not appear in English.
As the player learns these other languages in an organic way, the words that they learn would be replaced with their English counterparts. One such example is by associating signposts with the building where they are located, e.g. reading a signpost that says "xyz" in front of an inn, would mark the word "xyz" as having the meaning "inn". In further texts, the word "xyz" would be replaced by "inn" and this could be signalled with a change in the words colour.

These languages could be created by generating random words and associating each with its English counterpart using a hashmap. This would be done only once at the start of the game (if the words are different on each playthrough) or during development (if the languages are the same across all playthroughs).

#### Word generator

Using a purely random word generator would simply produce gibberish, which is kind of hard to pronounce. A more complex system could be devised to make the words appear more organic. For instance, ensuring only certain combination of vowel and consonant clusters appear.

Moreover, one could add a distinct _feel_ to eahc language by defining letter clusters and syllables that would appear more commonly when generating words for a language, e.g. "ij" and "aa" for a Dutch-like tongue, or restring words to single consonant and vowel combinations for a Spanish or Japanese-like tongue.

This could be achieved by providing more detail about how words are composed for a given language. You could define settings on how individual syllables are generated, and then how these syllables combine to make words.

Some parameters I thought of:

* Minimum and maximum number of syllables in a word.
* Allowed consonant clusters and length at the beginning and end of a syllable
* Allowed vowel clusters and length within a syllable
* More complex conditional settings, e.g.: if this vowel is used, then you can't use this other consonant.

