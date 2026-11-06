include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_link_libraries(spike_fileacc
    PUBLIC
        SpikeLibraries::debugl
        ${SPIKE_ZLIB_TARGET}
        ${SPIKE_ZSTD_TARGET}
        ${SPIKE_YAMLCPP_TARGET})
