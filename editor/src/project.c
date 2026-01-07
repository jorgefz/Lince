#include "project.h"

#include <fileapi.h>

LinceBool LinceMakeDir(string_t path){
    int success = CreateDirectoryA(path.str, NULL);
    return !!success;
}


static LinceProject* LinceProjectInit(string_t name, string_t location, LinceProjectSettings* settings){

    LinceProject* proj = LinceAlloc(sizeof(LinceProject));
    LINCE_ASSERT_ALLOC(proj, sizeof(LinceProject));

    proj->name     = string_copy(name);
    proj->rootdir  = string_from_fmt("%s/%s", location.str, name.str);
    proj->bindir   = string_from_fmt("%s/bin", proj->rootdir.str); // binary files / executables
    proj->resdir   = string_from_fmt("%s/res", proj->rootdir.str); // resources/assets
    proj->tomldir  = string_from_fmt("%s/project.toml", proj->rootdir.str); // project info
    proj->pmakedir = string_from_fmt("%s/premake5.lua", proj->rootdir.str);
    proj->lincedir = string_from_fmt("%s/lince", proj->rootdir.str);

    if (settings) proj->settings = *settings;

    return proj;
}


static LinceBool LinceProjectCreateDirectoryStructure(LinceProject* proj){
    
    // Create directories
    if(
        !LinceMakeDir(proj->rootdir)  ||
        !LinceMakeDir(proj->bindir)   ||
        !LinceMakeDir(proj->resdir)   ||
        !LinceMakeDir(proj->lincedir)
    ){
        printf("Failed to create project directories\n");
        return LinceFalse;
    }

    // Create files
    // Save project settings to "project.toml"
    FILE* toml = fopen(proj->tomldir.str, "w");
    if(!toml){
        printf("Failed to create project settings file\n");
        return LinceFalse;
    }
    fprintf(toml, "name   = \"%s\"\n", proj->name.str);
    fprintf(toml, "root   = \"%s\"\n", proj->rootdir.str);
    fprintf(toml, "bin    = \"%s\"\n", proj->bindir.str);
    fprintf(toml, "res    = \"%s\"\n", proj->resdir.str);
    fprintf(toml, "pmake  = \"%s\"\n", proj->pmakedir.str);
    fprintf(toml, "lince  = \"%s\"\n", proj->lincedir.str);
    fclose(toml);

    // Generate premake script
    FILE* pmake = fopen(proj->pmakedir.str, "w");
    if(!pmake){
        printf("Failed to create project premake script\n");
        return LinceFalse;
    }
    const char pmake_template[] = 
        "project \"%s\"\n"
        "   kind \"ConsoleApp\"\n"
        "   language \"C\"\n"
        "   staticruntime \"on\"\n"
        "   location \"\%{wks.location}/build/\%{prj.name}\"\n"
        "   targetdir (\"\%{wks.location}/bin/\" .. LinceOutputDir .. \"/\%{prj.name}\")\n"
        "   objdir (\"\%{wks.location}/obj/\" .. LinceOutputDir .. \"/\%{prj.name}\")\n"
        "   files { \"src/**.c\", \"include/**.h\" }\n";
    fprintf(pmake, pmake_template, proj->name.str);
    fclose(pmake);

    return LinceTrue;
}

/**
 * Create an empty Lince project and its folder structure
 * at the specified location.
 * @param name Project name.
 * @param location Directory where to create project folder
 */
LinceProject* LinceEditorCreateProject(string_t name, string_t location){

    LinceProject* proj = LinceProjectInit(name, location, NULL);
    if(!proj) return NULL;

    LinceBool success = LinceProjectCreateDirectoryStructure(proj);
    if(!success){
        LinceProjectFree(proj);
        return NULL;
    }

    // Clone Lince
    // system("git clone https://github.com/jorgefz/lince '%s'", lincedir.str);

    return proj;
}

LinceProject* LinceEditorLoadProject(string_t rootdir){

    string_t tomldir = string_from_fmt("%s/project.toml", rootdir.str);
    if(!LinceIsFile(tomldir)){
        print("No project.toml found at '%s'\n", rootdir.str);
        string_free(&tomldir);
        return NULL;
    }

    string_t content = LinceReadFile(tomldir);
    char errbuf[100];
    toml_table_t* toml = toml_parse(content.str, errbuf, sizeof(errbuf));
    if(!toml){
        print("Failed to parse project.toml\n");
        print("%s\n", errbuf);
        return NULL;
    }

    toml_free(toml);
}

void LinceProjectFree(LinceProject* proj){
    string_free(&proj->name);
    string_free(&proj->rootdir);
    string_free(&proj->bindir);
    string_free(&proj->resdir);
    string_free(&proj->pmakedir);
    string_free(&proj->tomldir);
    string_free(&proj->lincedir);
    LinceFree(proj);
}