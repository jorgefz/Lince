#include <toml.h>
#include "util.h"


size_t strlen_or(char* const buf, long max){
    char* p = &(buf[0]); // Avoid compiler warning on const
    while(p != '\0'){
        p++;
        if( (p - buf) > max){
            return max;
        }
    }
    return (size_t)(p - buf);
}


LinceTileset* LinceLoadTilesetToml(LinceAssetCache* assets, string_t fname){
    char errbuf[200];

    string_t content = LinceReadFile(fname);
    if(!content.str){
        printf("Could not read toml file '%s'\n", fname.str);
        return NULL;
    }

    toml_table_t* table = toml_parse(content.str, errbuf, sizeof(errbuf));
    string_free(&content);

    if(!table){
        printf("Failed to parse toml file '%s'\n", fname.str);
        printf("%s\n", errbuf);
        return NULL;
    }

    toml_datum_t field_texname = toml_string_in(table, "texture");
    toml_datum_t field_width_px     = toml_int_in(table, "width_px");
    toml_datum_t field_height_px    = toml_int_in(table, "height_px");

    string_t texname = string_scoped(field_texname.u.s, strlen_or(field_texname.u.s, LINCE_PATH_MAX));
    size_t width_px  = field_width_px.u.i;
    size_t height_px = field_height_px.u.i;
    toml_free(table);

    LinceTexture* tex = LinceAssetCacheGet(assets, texname, string_scoped_lit("texture"));
    if(!tex){
        printf("Could not find texture '%s'\n", texname.str);
        string_free(&texname);
        return NULL;
    }
    string_free(&texname);

    LinceTileset* tset = LinceAlloc(sizeof(LinceTileset));
    LinceTilesetInit(tset, tex, (uint32_t)width_px, (uint32_t)height_px);
    return tset;
}


LinceTilemap* LinceLoadTilemapToml(LinceAssetCache* assets, string_t fname){
    char errbuf[200];

    string_t content = LinceReadFile(fname);
    if(!content.str){
        printf("Could not read toml file '%s'\n", fname.str);
        return NULL;
    }

    toml_table_t* table = toml_parse(content.str, errbuf, sizeof(errbuf));

    toml_datum_t field_tsetname = toml_string_in(table, "texture");
    toml_datum_t field_width    =    toml_int_in(table, "width");
    toml_datum_t field_height   =    toml_int_in(table, "height");
    toml_array_t* field_grid    =  toml_array_in(table, "grid");

    if(!field_width.ok || !field_height.ok || !field_grid){
        if(field_tsetname.ok)  LinceFree(field_tsetname.u.s);
        string_free(&content);
        return NULL;
    }

    string_t tsetname = string_scoped(field_tsetname.u.s, strlen_or(field_tsetname.u.s, LINCE_PATH_MAX));
    size_t width  = field_width.u.i;
    size_t height = field_height.u.i;

    size_t ntiles = (size_t)toml_array_nelem(field_grid);
    if(ntiles != width * height){
        printf("Corrupted tilemap toml file '%s'\n", fname.str);
        printf("Expected %llux%llu = %llu tiles but got %llu\n", width, height, width*height, ntiles);
        string_free(&tsetname);
    }

    array_t grid;
    array_init(&grid, (uint32_t)sizeof(uint32_t));
    array_resize(&grid, (uint32_t)ntiles);
    for(size_t i = 0; i != ntiles; ++i){
        toml_datum_t dat = toml_int_at(field_grid, (int)i);
        uint32_t value = (uint32_t)dat.u.i;
        array_set(&grid, &value, i);
    }

    LinceTileset* tset = LinceAssetCacheGet(assets, tsetname, string_scoped_lit("tileset"));

    LinceTilemap* tm = LinceCalloc(sizeof(LinceTilemap));
    tm->width = (uint32_t)width;
    tm->height = (uint32_t)height;
    LinceTilemapInit(tm, grid.data);
    LinceTilemapUseTileset(tm, tset);

    toml_free(table);
    array_uninit(&grid);

    return tm;
}