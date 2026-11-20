include("${PROJECT_SOURCE_DIR}/cmake/SpikeLibrary.cmake")

target_link_libraries(spike_assetimp
    PRIVATE
        jsoncpp_static
)

if (SPIKE_LIBRARIES_BUILD_ASSET_IMPORTER_ISOLATED)

elseif (SPIKE_LIBRARIES_BUILD_ALL_LIBRARIES)
	target_link_libraries(spike_assetimp
    PRIVATE
        SpikeLibraries::sml
		SpikeLibraries::debugl
	)
	add_compile_definitions(_SPIKE_DEBUG)
	add_compile_definitions(_SPIKE_SML)
else()
	if (SPIKE_LIBRARIES_BUILD_SML)
		target_link_libraries(spike_assetimp
		PRIVATE
			SpikeLibraries::sml
		)
		add_compile_definitions(_SPIKE_SML)
	endif()
	
	if (SPIKE_LIBRARIES_BUILD_DEBUGGER)
		target_link_libraries(spike_assetimp
		PRIVATE
			SpikeLibraries::debugl
		)
		add_compile_definitions(_SPIKE_DEBUG)
	endif()
endif()

if (_ASSETIMP_FBX)
	target_link_libraries(spike_assetimp
    PRIVATE
		fbx::sdk
	)
	
	add_compile_definitions(_USING_FBX)
endif()