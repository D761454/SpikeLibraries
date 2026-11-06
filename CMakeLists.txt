cmake_minimum_required(VERSION 3.20)
project(SpikeLibraries VERSION 1.0.0 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(SPIKE_LIBRARIES_BUILD_ALL_LIBRARIES "Build the SpikeLibraries components" ON)

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
endif()

install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
