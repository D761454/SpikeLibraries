## SpikeLibraries v1.0.0

Reading this change-log is the recommended way to keep up to date with each libraries capabilities alongside the addition of any new libraries.

Please refer to the [Wiki](https://github.com/D761454/SpikeLibraries/wiki) for details on using each library within your projects.

## Spike Libraries v1.2.0

This is a much needed update that will reduce interdependency between all full spike libraries (header and source).

Equally, all of the libraries have been updated with relevant safety checks to prevent accidental misuse and provide users with xontext on failure to 
allow for graceful error handling.

In doing so, much of the internal functionality of the libraries has changed, however the public API remains mostly the same.

Key Notes:
- v1.0.0-1.1.0 Compressed Data with an uncompressed size > max value of unsigned int is incompatible with the new iocomp library due 
to the updated method of crc32 calculation, which now handles byte buffers larger than the max size of an unsigned int.
- Compression to a buffer requiresa capacity value to ensure no memory is falsely overwritten.
- The different memsys allocator types should be far safer to use, with harsher alignment checking and error handling.
- Memsys has an optional logging callback the user may hook into if they include Allocator.h, allowing the user to incorporate 
any method of logging for errors within memsys, with these providing more context than is possible with errlib returns.
- Assetimp contains its own vector types, removing dependnecy on sml.

## Binaries Info

The pre-built binaries do not have the fbx importer built into the asset importer library. 
This is because Autodesk's license agreement prevents direct automated unauthenticated hotlinking of their installers from scripts. 
If you need the fbx importer, you can download the [fbx sdks](https://aps.autodesk.com/developer/overview/fbx-sdk) and create an 
FBXSDK_ROOT environment variable pointing to the sdks directory. Doing so will allow cmake to locate the sdks and generate the 
asset importer project with the working fbx importer.

**Full Changelog**: https://github.com/D761454/SpikeLibraries/commits/v1.1.1