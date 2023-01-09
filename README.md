# vitOTT
[![Builds](https://github.com/edgjj/vitOTT/actions/workflows/builds.yml/badge.svg)](https://github.com/edgjj/vitOTT/actions/workflows/builds.yml)

Multiband compressor from Vital synthesizer.

## Requirements
SSE2 compliant CPU.

## Installing pre-built binaries
- Get release archive for necessary platform from [Builds](https://github.com/edgjj/vitOTT/actions/workflows/builds.yml)
- Copy .%format% directories according to following table

| Format            | Place to install                   |
| ----------------- | ---------------------------------- |
| VST3 (Windows)    | %COMMONPROGRAMFILES%/VST3          |
| VST3 (macOS)      | /Library/Audio/Plug-ins/VST3       |
| VST3 (Linux)      | ~/.vst3                            |
| LV2 (Linux)       | ~/.lv2                             |
| AudioUnit (macOS) | /Library/Audio/Plug-ins/Components |


## Building from sources
- Install dependencies (Linux only, demonstrated for apt)
```
    $ sudo apt-get update 
    $ sudo apt-get install -y libasound2-dev libgtk-3-dev libxcb1-dev libcurl4-openssl-dev libwebkit2gtk-4.0-dev
```
- Build plugin itself
```
    $ git clone --recursive https://github.com/edgjj/vitOTT.git
    $ mkdir build && cd build
    $ cmake ..
    $ cmake --build . --config Release
```
- You're great!

## What can you do with the source
The whole source code is licensed under the GPLv3. If you download the source or create builds you must comply with that license.

## Links
Vital: https://github.com/mtytel/vital

JUCE: https://github.com/juce-framework/JUCE
