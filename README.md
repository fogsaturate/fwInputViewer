# fwInputViewer
fog's wacky input viewer, for Clone Hero, YARG, and more

GCC Arguments for VS Code
```json
"args": [
    "-fdiagnostics-color=always",
    "-g",
    "${fileDirname}/main.cpp",
    "${fileDirname}/input.cpp",
    "${fileDirname}/config.cpp",
    "${fileDirname}/createitem.cpp",
    "-o",
    "${fileDirname}/${fileBasenameNoExtension}",
    "-lraylib", "-lsfml-window", "-lsfml-system",
    "-lpthread", "-lm", "-ldl", "-lrt", "-lGL"
],
```
