#!/bin/zsh

cmake . --list-presets  
cmake . -B build --preset=Release_x64-macos_clang