# Project Config File

When you set up a project with Lince, it is very useful to create a config file alongside it.
The config file tells the application where to find the folder with its assets, as well as the start-up properties of the window (e.g. dimensions, vsync, etc).

Without it, the application will assume some default values for e.g. paths, which may not actually exist, crashing the program on start-up.

The configuration file is a TOML file which accepts the following parameters:

| Parameter            | Type | Description                                           | Default       |
| -------------------- | ---- | ----------------------------------------------------- | ------------- |
| `root`               | str  | Absolute path to the root directory of the project.   | `"./"`        |
| `assets`             | str  | Path to the app assets, relative to root.             | `"assets/"`   |
| `engine`             | str  | Path to the engine assets, relative to root.          | `"lince/"`    |
| `logfile`            | str  | Path to log file, relative to root.                   | `"log.txt"`   |
| `logging`            | bool | Whether to produce log messages to a log file.        | `"false"`     |
| `window.title`       | str  | Title displayed at the top of the window.             | `"Lince App"` |
| `window.width`       | int  | Window width  in pixels (if fullscreen is false).     | `1080`        |
| `window.height`      | int  | Window height in pixels (if fullscreen is false).     | `720`         |
| `window.fullscreen`  | bool | Enable fullscreen mode.                               | `false`       |
| `window.resizable`   | bool | Allow window to be resized by the user.               | `true`        |
| `window.vsync`       | bool | Enable VSync.                                         | `false`       |

The default values are adopted if the config file cannot be found or if their values are not provided in the config file.
