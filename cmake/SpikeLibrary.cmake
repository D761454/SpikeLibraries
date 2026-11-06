# Build one static target for a Spike component directory. The component name
# is derived from this CMakeLists.txt's parent directory.
get_filename_component(_spike_component "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(_spike_target "spike_${_spike_component}")

file(GLOB _spike_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE _spike_headers CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/spike/${_spike_component}/*.h"
    "${PROJECT_SOURCE_DIR}/include/spike/${_spike_component}/*.hpp")

if(NOT _spike_sources)
    message(FATAL_ERROR "No .cpp sources found for ${_spike_component}")
endif()

add_library(${_spike_target} STATIC ${_spike_sources} ${_spike_headers})
add_library(SpikeLibraries::${_spike_component} ALIAS ${_spike_target})

target_compile_features(${_spike_target} PUBLIC cxx_std_20)
target_include_directories(${_spike_target}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(${_spike_target} PROPERTIES
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME ${_spike_component})

install(TARGETS ${_spike_target}
    EXPORT SpikeLibrariesTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
