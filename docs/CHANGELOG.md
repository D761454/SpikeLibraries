## SpikeLibraries v1.0.0

Reading this change-log is the recommended way to keep up to date with each libraries capabilities alongside the addition of any new libraries.

Please refer to the [Wiki](https://github.com/D761454/SpikeLibraries/wiki) for details on using each library within your projects.

## Spike Libraries v1.1.1

This is a much needed update that will reduce interdependency between some spike libraries, 
both header-only and full libraries.


## Binaries Info

The pre-built binaries do not have the fbx importer built into the asset importer library. 
This is because Autodesk's license agreement prevents direct automated unauthenticated hotlinking of their installers from scripts. 
If you need the fbx importer, you can download the [fbx sdks](https://aps.autodesk.com/developer/overview/fbx-sdk) and create an 
FBXSDK_ROOT environment variable pointing to the sdks directory. Doing so will allow cmake to locate the sdks and generate the 
asset importer project with the working fbx importer.

**Full Changelog**: https://github.com/D761454/SpikeLibraries/commits/v1.1.1