include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_include_directories(spike_fileparser
	PRIVATE
		${zstd_SOURCE_DIR}/lib
)

target_link_libraries(spike_fileparser
    PRIVATE
        SpikeLibraries::debugl
        zlibstatic
		zstd_static
		yaml-cpp
)
