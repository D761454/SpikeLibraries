include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_link_libraries(spike_fileacc
    PRIVATE
		yaml-cpp
)

if (SPIKE_LIBRARIES_BUILD_FILE_ACCESSOR_ISOLATED)

elseif (SPIKE_LIBRARIES_BUILD_ALL_LIBRARIES OR SPIKE_LIBRARIES_BUILD_DEBUGGER)
	target_link_libraries(spike_fileacc
    PRIVATE
        SpikeLibraries::debugl
	)
	add_compile_definitions(_SPIKE_DEBUG)
endif()