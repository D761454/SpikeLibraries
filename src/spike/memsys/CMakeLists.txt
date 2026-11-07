include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_link_libraries(spike_memsys PUBLIC SpikeLibraries::debugl)
