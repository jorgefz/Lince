# Asset Cache

Assets are essentially files on disk which contain data used by the game.
These range from images and textures (e.g. PNG and JPG files), to shader files, to tilesets and maps.

The asset cache is an engine system that takes care of loading assets when needed and managing their lifetimes.

You can fetch the global asset cache from anywhere in your program with the function `LinceAppFetchAssetCache()`.

## Location of assets

You can tell Lince where to find your assets in two ways:

1. In config file with parameter `assets` (see [ProjectConfigFile](ProjectConfigFile.md)).
2. In code with the function `LinceAppPushAssetFolder(path)`.
    * You can call this function at any point, but it is recommended to do so during initialisation.
3. Doing nothing. Lince will assume the assets are located in a folder named `assets` in the current working directory.

Both must be relative to the project's root path.


## Asset names

Inside the code, the asset cache refers to each asset using a name that is unique to that asset, which we can use to find it later in our game.

Asset names in the asset cache, however, are not stored as strings. This is because comparing strings is slow, as you might have two compare one character at a time. Instead, the cache uses String IDs (see [StringIDs](StringIDs.md) for further details).
As such, the asset cache functions take `LinceSID` types for asset names and types.
You can easily convert a string literal into a String ID with the macro `LinceMakeSIDFromLit()`.

If you also store references to assets in your code, it is recommended you do so using string IDs (`LinceSID`), and use `LinceAssetCacheGet(cache, sid)` every time you need the asset.


## Asset types

Types are a way of clasifying assets that can be loaded in the same way. For instance, loading images from png or jpg files.

To add an asset type, you must give your type an unique name (in the form of a String ID), and tell the asset cache what functions to use to load and unload it.
This is done with the function `LinceAssetCacheAddType(cache, type_name, load_func, unload_func)`.

The load function must have the signature `void* my_load(LinceAssetCache*, string_t path, void* args)`, where `path` will be the location of the asset within the asset folder; the function must also return a pointer to the asset stored on the heap, which the function will be responsible for allocating.

The unload function must have the signature `void my_load(LinceAssetCache*, void* ptr)`, where `ptr` will be the pointer to the asset stored on the heap. The function will be responsible from deallocating it.


## Registering assets

In order to use an in your game, two steps must be carried out:

1. Register the asset with the cache. Name the asset with an unique string, specify its type (previously added with `LinceAssetCacheAddType`), and its location in the assets folder.

2. Load the asset into memory, to be used by your game.

You can register assets in two ways:

1. Using an Asset Index File.
2. Manually using Lince functions.

### Automatically with and Index File

Inside assets folder, you can place an 'index' file which will specify which assets to register. This is essentially a TOML file with a list of assets to register and their locations inside the assets folder.

To automatically register all assets defined in this file, use the function `LinceAssetCacheRegisterIndex(cache, path)`, which takes the path of the index file (relative to the assets folder).

An example of an index file defining two assets is as follows:
```
# assets.toml

[[assets]]
name = "outside_map"
type = "tilemap"
path = "tilemaps/outside.toml"

[[assets]]
name = "inside_map"
type = "tilemap"
path = "tilemaps/inside.toml"
```

Each asset is sepparated by the header `[[assets]]`, which in TOML adds a table element to a list.
In JSON, this is equivalent to:
```
"assets": [
    { "name" : "outside_map", "type" : "tilemap", "path" : "tilemaps/outside.toml" },
    { "name" : "inside_map",  "type" : "tilemap", "path" : "tilemaps/inside.toml" },
]
```

The parameters needed to define an asset are as follows:

* name: unique string identifier for the asset.
* type: string identifier of its type, tells Lince which load/unload functions to use.
* path: location of the asset within the assets folder.


### Programatically with Lince functions

You can also use the following function to register assets:

`LinceAssetCacheRegister(LinceAssetCache* cache, LinceSID name, LinceSID type, string_t path)`


## Using and managing assets

* `LinceAssetCacheLoad`. Manually load a registered asset.
* `LinceAssetCacheUnload`. Manually unload a loaded asset.
* `LinceAssetCacheGet`. Fetch a registered asset. It is also loaded if it wasn't previously.
* `LinceAssetCacheAdd`. Add an existing unmanaged asset to the cache.