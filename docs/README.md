# SpikeLibraries
SpikeLibraries: Simple libraries for performing many common tasks within game development, from maths, debug logging, and memory management. Built for C++ with minimal dependencies (excluding the asset importer).

## Foreword

The asset importer library requires the use of the fbx sdks to build and work. As such, you should download the [fbx sdk]() and then create an environment variable under the name FBXSDK_ROOT pointing to the installation directory. Once this is done, the assetimporter library can be built succesfully.

## Summary

SpikeLibraries are simple extensible libraries for C++ that may be used across a variety of applications. The libraries are all self contained (no external dependencies, apart from other spike libraries (usually header only libraries)), portable and easy to use.

SpikeLibraries are designed to enable fast iteration on game, engine or tools related projects. The libraries tend towards simplicity to ensure wide use-cases, lacking in some 
higher end features. However, over time, we are open to implementing some more advanced ideas on-top of the present libraries, either as new libraries or improvements upon the old.

SpikeLibraries are primarily geared towards integration into game related projects, namely game engines and real-time 2D or 3D applications.

These are libraries i have developed and have been using for a while during the development of the [Spike Engine](https://sites.google.com/view/cayden-andrews/projects/c/spikeengine).
Feel free to let me know if you have used these libraries in you projects. I would love to see what you do with them.

## Usage

The core of each library will be contained within a few platform agnostic files, easily included within your projects. No specific build processes are required but it may be ideal to build the required library separately and include the '.lib' and include headers separately as to not clutter your project.

The libraries were built and tested using C++20. However, some libraries will work on earlier versions. If you wish for a library to be compatible on earlier versions which isn't already of C++, please open an issue and we would be glad to provide.

## Releases and Change-logs

See the [Releases](https://github.com/D761454/SpikeLibraries/releases) page for change-logs. Reading the change-logs is an appropriate way to keep up to date with the capabilities of each library and may bring your attention to new or unknown features.

## How to help

See the [Issues](https://github.com/D761454/SpikeLibraries/issues) page for current tasks underway within the libraries.

You may help with development with pull requests. However, please note that through doing this you also request that your code is reviewed and then taken into the libraries forever. The pull request should be created in the interest of users and to ensure its goal and changes are made clear so it may be accepted.

## License
All SpikeLibraries are licensed under the MIT License, see [LICENSE](https://github.com/D761454/SpikeLibraries/tree/main?tab=BSD-3-Clause-1-ov-file) for more info.
