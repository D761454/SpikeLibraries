cmake_minimum_required(VERSION 3.20)
project(SpikeLibraries VERSION 1.0.0 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(SPIKE_LIBRARIES_BUILD_ALL_LIBRARIES "Build the SpikeLibraries components" ON)

option(SPIKE_LIBRARIES_BUILD_DEBUGGER "Build the Debug library" ON)
option(SPIKE_LIBRARIES_BUILD_MEMORY_SYSTEM "Build the Memory System library" ON)
option(SPIKE_LIBRARIES_BUILD_SML "Build the Spike Maths library" ON)
option(SPIKE_LIBRARIES_BUILD_ASSET_IMPORTER "Build the Asset Importer library" ON)
option(SPIKE_LIBRARIES_BUILD_FILE_ACCESSOR "Build the File System library" ON)
option(SPIKE_LIBRARIES_BUILD_IO_COMP "Build the Compression library" ON)

if(NOT DEFINED ENV{FBXSDK_ROOT})
    message(WARNING "FBXSDK_ROOT environment variable not set!\n"
		"Please download the Autodesk FBX SDK and set FBXSDK_ROOT to point to its root directory.")
	message(WARNING "The Asset importer will not be built with the fbx importer.")
	set(_ASSETIMP_FBX OFF)
else()
	set(FBXSDK_ROOT $ENV{FBXSDK_ROOT})
	file(TO_CMAKE_PATH "${FBXSDK_ROOT}" FBXSDK_ROOT)
	add_library(fbx::sdk UNKNOWN IMPORTED)
	
	if(WIN32)
		set_target_properties(fbx::sdk PROPERTIES
			IMPORTED_LOCATION "${FBXSDK_ROOT}/lib/vs2022/x64/release/libfbxsdk.lib"
			IMPORTED_IMPLIB   "${FBXSDK_ROOT}/lib/vs2022/x64/release/libfbxsdk.lib"
			INTERFACE_INCLUDE_DIRECTORIES "${FBXSDK_ROOT}/include"
		)
		
		set_target_properties(fbx::sdk PROPERTIES
			INTERFACE_LINK_LIBRARIES "wininet;ksuser;dsound"
		)
		set(_ASSETIMP_FBX ON)
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		set_target_properties(fbx::sdk PROPERTIES
			IMPORTED_LOCATION "${FBXSDK_ROOT}/lib/gcc/x64/release/libfbxsdk.a"
			INTERFACE_INCLUDE_DIRECTORIES "${FBXSDK_ROOT}/include"
		)
		
		set_target_properties(fbx::sdk PROPERTIES
			INTERFACE_LINK_LIBRARIES "pthread;dl;xml2;z"
		)
		set(_ASSETIMP_FBX ON)
	else()
		message(WARNING "Unsupported platform for FBX SDK. Asset importer will not be built with the fbx importer.")
		set(_ASSETIMP_FBX OFF)
	endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

# Third-party dependencies
include(FetchContent)

# Disabling unneeded stuff

# Zstd: Disable CLI binaries and shared library targets
set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_SHARED OFF CACHE BOOL "" FORCE)

# Zlib
set(ZLIB_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ZLIB_BUILD_TESTING OFF CACHE BOOL "" FORCE)

# JsonCpp: Turn off internal unit tests
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_OBJECT_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# YAML-CPP: Turn off tests and tools
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Fetching
FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.2
)
FetchContent_MakeAvailable(zlib)

FetchContent_Declare(
    zstd
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG        v1.5.7
    SOURCE_SUBDIR  build/cmake
)
FetchContent_MakeAvailable(zstd)

FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG        1.9.8
)
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(jsoncpp)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        yaml-cpp-0.9.0
)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(yaml-cpp)

set_target_properties(zlibstatic libzstd_static jsoncpp_static yaml-cpp PROPERTIES
	FOLDER "ThirdParty"
)

if(SPIKE_LIBRARIES_BUILD_ALL_LIBRARIES)
    # Every immediate directory in include/spike is treated as a component.
    file(GLOB _spike_component_dirs LIST_DIRECTORIES true
        "${CMAKE_CURRENT_SOURCE_DIR}/include/spike/*")
    foreach(_component_dir IN LISTS _spike_component_dirs)
        if(IS_DIRECTORY "${_component_dir}")
            get_filename_component(_component "${_component_dir}" NAME)
            set(_component_cmake "${CMAKE_CURRENT_SOURCE_DIR}/src/spike/${_component}/CMakeLists.txt")
            if(EXISTS "${_component_cmake}")
                add_subdirectory("src/spike/${_component}")
            else()
                message(STATUS "Skipping ${_component}: no src/spike/${_component}/CMakeLists.txt")
            endif()
        endif()
    endforeach()
else()
	if (SPIKE_LIBRARIES_BUILD_ASSET_IMPORTER)
		if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/spike/assetimp")
            set(_component_cmake "${CMAKE_CURRENT_SOURCE_DIR}/src/spike/assetimp/CMakeLists.txt")
            if(EXISTS "${_component_cmake}")
                add_subdirectory("src/spike/assetimp")
            else()
                message(STATUS "Skipping asset importer: no src/spike/assetimp/CMakeLists.txt")
            endif()
        endif()
	endif()
	if (SPIKE_LIBRARIES_BUILD_DEBUGGER)
		if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/spike/debugl")
            set(_component_cmake "${CMAKE_CURRENT_SOURCE_DIR}/src/spike/debugl/CMakeLists.txt")
            if(EXISTS "${_component_cmake}")
                add_subdirectory("src/spike/debugl")
            else()
                message(STATUS "Skipping debugger: no src/spike/debugl/CMakeLists.txt")
            endif()
        endif()
	endif()
	if (SPIKE_LIBRARIES_BUILD_FILE_ACCESSOR)
		if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/spike/fileacc")
            set(_component_cmake "${CMAKE_CURRENT_SOURCE_DIR}/src/spike/fileacc/CMakeLists.txt")
            if(EXISTS "${_component_cmake}")
                add_subdirectory("src/spike/fileacc")
            else()
                message(STATUS "Skipping file accessor: no src/spike/fileacc/CMakeLists.txt")
            endif()
        endif()
	endif()
	if (SPIKE_LIBRARIES_BUILD_IO_COMP)
		if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/spike/iocomp")
            set(_component_cmake "${CMAKE_CURRENT_SOURCE_DIR}/src/spike/iocomp/CMakeLists.txt")
            if(EXISTS "${_component_cmake}")
                add_subdirectory("src/spike/iocomp")
            else()
                message(STATUS "Skipping file compressor: no src/spike/iocomp/CMakeLists.txt")
            endif()
        endif()
	endif()
	if (SPIKE_LIBRARIES_BUILD_MEMORY_SYSTEM)
		if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/spike/memsys")
            set(_component_cmake "${CMAKE_CURRENT_SOURCE_DIR}/src/spike/memsys/CMakeLists.txt")
            if(EXISTS "${_component_cmake}")
                add_subdirectory("src/spike/memsys")
            else()
                message(STATUS "Skipping memory system: no src/spike/memsys/CMakeLists.txt")
            endif()
        endif()
	endif()
	if (SPIKE_LIBRARIES_BUILD_SML)
		if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/spike/sml")
            set(_component_cmake "${CMAKE_CURRENT_SOURCE_DIR}/src/spike/sml/CMakeLists.txt")
            if(EXISTS "${_component_cmake}")
                add_subdirectory("src/spike/sml")
            else()
                message(STATUS "Skipping spike maths library: no src/spike/sml/CMakeLists.txt")
            endif()
        endif()
	endif()
endif()

install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
