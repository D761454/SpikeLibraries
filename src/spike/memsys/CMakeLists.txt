include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

if (SPIKE_LIBRARIES_BUILD_MEMORY_SYSTEM_ISOLATED)
	
elseif (SPIKE_LIBRARIES_BUILD_ALL_LIBRARIES OR SPIKE_LIBRARIES_BUILD_DEBUGGER)
	target_link_libraries(spike_memsys
    PRIVATE
        SpikeLibraries::debugl
	)
	add_compile_definitions(_SPIKE_DEBUG)
endif()