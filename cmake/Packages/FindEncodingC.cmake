function(encoding_rs_c_dependency_jail)
	find_program(cargo_command NAMES cargo cargo.exe)
	if (NOT cargo_command)
		message(FATAL_ERROR "Could not find the \"cargo\" command to invoke for building the hsivonen/encoding_rs_c library")
	endif()

	FetchContent_Declare(encoding_rs_c_external
		GIT_REPOSITORY https://github.com/hsivonen/encoding_c.git
	)
	FetchContent_MakeAvailable(encoding_rs_c_external)
	if (BUILD_SHARED_LIBS)
		set(encoding_rs_c_type SHARED)
	else()
		set(encoding_rs_c_type STATIC)
	endif()
	set(cargo_build_result_json_file ${encoding_rs_c_external_BINARY_DIR}/cargo_build_result.json)
	set(encoding_rs_c_static_lib ${encoding_rs_c_external_BINARY_DIR}/$<IF:$<CONFIG:Release,MinSizeRel,RelWithDebInfo>,release,debug>/${CMAKE_STATIC_LIBRARY_PREFIX}encoding_c${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(encoding_rs_c_dynamic_lib ${encoding_rs_c_external_BINARY_DIR}/$<IF:$<CONFIG:Release,MinSizeRel,RelWithDebInfo>,release,debug>/${CMAKE_SHARED_LIBRARY_PREFIX}encoding_c${CMAKE_SHARED_LIBRARY_SUFFIX})
	add_custom_command(OUTPUT ${cargo_build_result_json_file}
			$<$<OR:$<BOOL:${WIN32}>,$<STREQUAL:${encoding_rs_c_type},STATIC>>:${encoding_rs_c_static_lib}>
			$<$<STREQUAL:${encoding_rs_c_type},SHARED>:${encoding_rs_c_dynamic_lib}>
		COMMAND ${cargo_command} rustc
			-Z unstable-options
			$<$<CONFIG:Release,MinSizeRel,RelWithDebInfo>:--release>
			--message-format=json
			--target-dir=${encoding_rs_c_external_BINARY_DIR}
			--crate-type=$<IF:$<STREQUAL:${BUILD_SHARED_LIBS},SHARED>,cdylib,staticlib>
			> ${cargo_build_result_json_file}
		VERBATIM
		COMMENT "[ztd.platform] building rust library encoding_c..."
		WORKING_DIRECTORY ${encoding_rs_c_external_SOURCE_DIR}
	)
	add_custom_target(encoding_rs_c_external_build
		DEPENDS ${cargo_build_result_json_file} ${encoding_rs_c_static_lib}
	)
	add_library(encoding_rs_c INTERFACE)
	add_library(encoding_rs::c ALIAS encoding_rs_c)
	target_include_directories(encoding_rs_c
		INTERFACE
			${encoding_rs_c_external_SOURCE_DIR}/include
	)
	target_link_libraries(encoding_rs_c
		INTERFACE
			$<IF:$<STREQUAL:$<TARGET_PROPERTY:encoding_rs_c,TYPE>,SHARED>,${encoding_rs_c_dynamic_lib},${encoding_rs_c_static_lib}>
			$<$<OR:$<BOOL:${WIN32}>,$<STREQUAL:$<TARGET_PROPERTY:encoding_rs_c,TYPE>,STATIC>>:${encoding_rs_c_static_lib}>
	)
	add_dependencies(encoding_rs_c encoding_rs_c_external_build)
endfunction()
encoding_rs_c_dependency_jail()
