## SpikeLibraries v1.0.0

Reading this change-log is the recommended way to keep up to date with each libraries capabilities alongside the addition of any new libraries.

Please refer to the [Wiki](https://github.com/D761454/SpikeLibraries/wiki) for details on using each library within your projects.

## Initial Release State

The initial release of the Spike Libraries contains:

- Type define headers
- flag_set type for space optimised flags
- Timer functions for profiling or time-step calculations
- POD write/read helpers for efficient file read/write
- nivector type for fast resize without default memory initialization
- error type for handling
- maths library for vector, quaternion, matrix and extended maths for 3D applications
- memory management for optimised memory allocation
- debug logging for simple runtime logging
- file parser helper for YAML read/write, including helpers for binary string read/write and compression/decompression (likely to be split up or renamed)
- asset importer for simplified game asset imports, including shaders (under a specific format), textures, and meshes (obj, gltf/glb and fbx)

## Binaries Info

The pre-built binaries do not have the fbx importer built into the asset importer library. 
This is because Autodesk's license agreement prevents direct automated unauthenticated hotlinking of their installers from scripts. 
If you need the fbx importer, you can download the [fbx sdks](https://aps.autodesk.com/developer/overview/fbx-sdk) and create an 
FBXSDK_ROOT environment variable pointing to the sdks directory. Doing so will allow cmake to locate the sdks and generate the 
asset importer project with the working fbx importer.

Note that GitHub are now clamping release notes sometimes really badly, click on a header/title to read full notes.

**Full Changelog**: https://github.com/D761454/SpikeLibraries/commits/v1.0.0