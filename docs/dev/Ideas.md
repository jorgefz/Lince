# Development Ideas

## The Big RPG game

### Language
The game would have several starting regions or kingdoms. Each region would have its own language.

The player could choose a region in which to start. The language of this region would be the player's native region, and thus they would be able to understand it. In this case, their native language would simply appear as English.

During the game, text in other languages would not appear in English.
As the player learns these other languages in an organic way, the words that they learn would be replaced with their English counterparts. One such example is by associating signposts with the building where they are located, e.g. reading a signpost that says "xyz" in front of an inn, would mark the word "xyz" as having the meaning "inn". In further texts, the word "xyz" would be replaced by "inn" and this could be signalled with a change in the words colour.

These languages could be created by generating random words and associating each with its English counterpart using a hashmap. This would be done only once at the start of the game (if the words are different on each playthrough) or during development (if the languages are the same across all playthroughs).

Using a purely random word generator would simply produce gibberish, which is hard to pronounce. A more complex system could be devised to make the words appear more organic. For instance, ensuring only certain combination of vowel and consonant clusters appear.

Moreover, one could add a distinct _feel_ to eahc language by defining letter clusters and syllables that would appear more commonly when generating words for a language, e.g. "ij" and "aa" for a Dutch-like tongue, or restring words to single consonant and vowel combinations for a Spanish or Japanese-like tongue.

