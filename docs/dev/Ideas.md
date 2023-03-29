# Development Ideas

## Lince Engine

### Scene system

A scene is a collection of entities and logic.
How does one manage multiple scenes with their own logic?

You could define each scene as a different type with custom data and a set of functions and logic to modify it. These could be related in a generic way with a `Scene` struct:
```c
struct Scene {
    void *data;
    void (*on_enter)(Scene* s);
    void (*on_update)(Scene* s, float dt);
    void (*on_draw)(Scene* s); // collect sprite components and draw, also tilemap
    void (*on_exit)(Scene* s);
    LinceEntityRegistry* reg;
}
```

Different scenes would be managed with a `SceneStack`, where scenes are loaded on top of one another, and only the topmost one is updated and rendered.
The state of other scenes in the stack would be saved in memory, though. This allows quick switching back to them.
This assumes a hierarchial structure, where some scenes are 'on top' of others.
For instance, the first scene would be the main menu, and the world and so on would be loaded on top. The topmost one could be a pause menu. 
```c
struct SceneStack {
    array_t<Scene> scenes;
    Scene* top;
}

void PushScene(SceneStack* stack, Scene* scene) {
    array_push(stack, scene);
}
void PopScene(SceneStack* stack){
    stack->top->on_exit(stack->top);
    array_pop(stack);
    stack->top = array_back(stack->scenes);
}
```

An example of a custom scene integrated with this scheme would be:

```c
struct TownScene {
    Tilemap *town_map;
    array_t<uint32_t> static_sprites; // entity IDs: trees, etc
    array_t<uint32_t> townsfolk; // people - sprites that move
}

void TownEnter(Scene* scene) {
    TownScene town_data = {
        .tilemap = LinceCreateTilemap("tilemap file"),
        .static_sprites = { }
        .townsfolk = { }
    };
    scene->data = LinceNewCopy(&town_data, sizeof(TownScene));
}

void TownUpdate(Scene* scene) {
    // Move townsfolk
    // Update camera entity, etc
}

void TownDraw(Scene* scene) {
    // Collect and draw entities
    // Draw tilemap
    // FUTURE: If player enters e.g. house, fetch its scene and push it to top of stack.
}

void TownExit(Scene* scene) {
    // Deallocate scene->data and set to NULL
}

void Init(){
    Scene town_scene = {
        .on_enter = TownEnter,
        .on_exit = TownExit,
        .on_update = TownUpdate,
        .on_draw = TownDraw
    }
    PushScene(&scene)
}

void Update(){
    Scene* top_scene = scene_stack->top;
    top_scene->on_update(top_scene);
    top_scene->on_draw(top_scene);
}
```

How do scenes communicate with one another? How do you push a new scene from another scene?

How do we make this compatible with serialisation?
We would have to follow a similar approach to that of components:
Write serialise `on_save` and deserialise `on_load` callbacks for each scene,
and call each in turn.


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

