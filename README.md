# fwInputViewer
fog's wacky input viewer, for Clone Hero, YARG, and more

GCC Arguments for VS Code
```json
"args": [
    "-fdiagnostics-color=always",
    "-g",
    "${file}",
    "-o",
    "${fileDirname}/${fileBasenameNoExtension}",
    "-lraylib", "-lsfml-window", "-lsfml-system",
    "-lpthread", "-lm", "-ldl", "-lrt", "-lGL"
],
```
