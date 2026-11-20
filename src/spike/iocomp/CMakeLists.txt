include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_include_directories(spike_iocomp
	PRIVATE
		${zstd_SOURCE_DIR}/lib
)

target_link_libraries(spike_iocomp
    PRIVATE
        zlibstatic
		zstd_static
)

if (SPIKE_LIBRARIES_BUILD_IO_COMP_ISOLATED)

elseif (SPIKE_LIBRARIES_BUILD_ALL_LIBRARIES OR SPIKE_LIBRARIES_BUILD_DEBUGGER)
	target_link_libraries(spike_iocomp
    PRIVATE
        SpikeLibraries::debugl
	)
	add_compile_definitions(_SPIKE_DEBUG)
endif()