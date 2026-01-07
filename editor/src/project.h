#include <lince.h>


typedef struct LinceProjectSettings {
    int placeholder;
} LinceProjectSettings;

typedef struct LinceProject {
    // Paths
    string_t name;     ///< Project name
    string_t rootdir;  ///< Absolute path of project's root directory
    string_t bindir;   ///< Absolute path of project's bin directory
    string_t resdir;   ///< Absolute path of project's resource directory
    string_t pmakedir; ///< Absolute path of project's premake script
    string_t tomldir;  ///< Absolute path of project's toml settings file
    string_t lincedir;  ///< Absolute path of project's toml settings file

    // Settings
    LinceProjectSettings settings;

} LinceProject;
