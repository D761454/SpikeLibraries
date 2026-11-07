include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_link_libraries(spike_assetimp
    PRIVATE
        SpikeLibraries::debugl
        jsoncpp_static
		fbx::sdk
)