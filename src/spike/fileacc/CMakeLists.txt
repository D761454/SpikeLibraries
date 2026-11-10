include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_link_libraries(spike_fileacc
    PRIVATE
        SpikeLibraries::debugl
		yaml-cpp
)
