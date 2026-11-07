include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_link_libraries(spike_assetimp
    PRIVATE
        SpikeLibraries::debugl
        jsoncpp_static
)

if (_ASSETIMP_FBX)
	target_link_libraries(spike_assetimp
    PRIVATE
		fbx::sdk
	)
	
	add_compile_definitions(_USING_FBX)
endif()