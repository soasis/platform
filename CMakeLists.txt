# =============================================================================
#
# ztd.platform
# Copyright © 2022-2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
# Contact: opensource@soasis.org
#
# Commercial License Usage
# Licensees holding valid commercial ztd.platform licenses may use this file
# in accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and Shepherd's Oasis, LLC.
# For licensing terms and conditions see your agreement. For
# further information contact opensource@soasis.org.
#
# Apache License Version 2 Usage
# Alternatively, this file may be used under the terms of Apache License
# Version 2.0 (the "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
# https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# =========================================================================== #

cmake_minimum_required(VERSION 3.21.0)
cmake_policy(VERSION 3.21)

# # Project kickstart
# Includes a bunch of basic flags and utilities shared across projects
# See more at the github repository link below
include(FetchContent)
FetchContent_Declare(ztd.cmake
	GIT_REPOSITORY https://github.com/soasis/cmake
	GIT_TAG main)
FetchContent_MakeAvailable(ztd.cmake)
set(CMAKE_PROJECT_INCLUDE ${ZTD_CMAKE_PROJECT_PRELUDE})

# # Project declaration
# informs about the project, gives a description, version and MOST IMPORTANTLY
# the languages the project is going to use. Required.
project(ztd.platform
	VERSION 0.0.0
	DESCRIPTION "A collection of projects for managing C and C++ platform-specific dependencies."
	HOMEPAGE_URL "https://ztdplatform.readthedocs.io/en/latest/"
	LANGUAGES C CXX)

if(ZTD_PLATFORM_READTHEDOCS)
	# ReadTheDocs seems unable to handle the include at the project level: something must be going wrong?
	include(CheckCXXCompilerFlag)
	include(CheckCCompilerFlag)
	include(CheckIPOSupported)
	include(CMakePackageConfigHelpers)
	include(CMakeDependentOption)
	include(CMakePrintHelpers)
	include(GNUInstallDirs)
	include(FeatureSummary)
	include(FetchContent)
	include(CTest)
endif()

# # # Top-Level Directories
# Check if this is the top-level project or not
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(ZTD_PLATFORM_IS_TOP_LEVEL_PROJECT YES)
else()
	set(ZTD_PLATFORM_IS_TOP_LEVEL_PROJECT NO)
endif()

# Modify bad flags / change defaults if we are the top level
if(ZTD_PLATFORM_IS_TOP_LEVEL_PROJECT)
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/bin")
	endif()

	if(NOT DEFINED CMAKE_CXX_STANDARD)
		set(CMAKE_CXX_STANDARD 17)
	endif()

	if(NOT DEFINED CMAKE_C_STANDARD)
		set(CMAKE_C_STANDARD 23)
	endif()

	if(ZTD_PLATFORM_EXAMPLES OR ZTD_PLATFORM_TESTS OR ZTD_PLATFORM_SCRATCH)
		# normal flags
		check_compiler_flag(disable-permissive MSVC /permissive- GCC -pedantic)
		check_compiler_flag(updated-cpp-version-flag MSVC /Zc:__cplusplus)

		# Warning flags
		check_compiler_flag(warn-pedantic MSVC /permissive- GCC -pedantic)
		check_compiler_flag(warn-all MSVC /W4 GCC -Wall)
		check_compiler_flag(warn-errors MSVC /WX GCC -Werror)
		check_compiler_flag(warn-extra GCC -Wextra Clang -Wextra)
		check_compiler_flag(utf8-literal-encoding MSVC /execution-charset:utf-8 GCC -fexec-charset=utf-8)
		check_compiler_flag(utf8-source-encoding MSVC /source-charset:utf-8 GCC -finput-charset=utf-8)
		check_compiler_flag(extra-constexpr-depth MSVC /constexpr:depth2147483647 GCC -fconstexpr-depth=2147483647 Clang -fconstexpr-depth=2147483647)
		check_compiler_flag(extra-constexpr-steps MSVC /constexpr:steps2147483647 GCC -fconstexpr-ops-limit=2147483647 Clang -fconstexpr-steps=2147483647)
		check_compiler_flag(template-debugging-mode GCC -ftemplate-backtrace-limit=0)
	endif()
endif()

# # Options
option(ZTD_PLATFORM_CI "Whether or not we are in continuous integration mode" OFF)
option(ZTD_PLATFORM_TESTS "Enable build of tests" OFF)
option(ZTD_PLATFORM_DOCUMENTATION "Enable build of documentation" OFF)
option(ZTD_PLATFORM_DOCUMENTATION_NO_SPHINX "Turn off Sphinx usage (useful for ReadTheDocs builds)" OFF)
option(ZTD_PLATFORM_EXAMPLES "Enable build of examples" OFF)
option(ZTD_PLATFORM_USE_LIBICONV "Enable libiconv with ztd::platform" OFF)
option(ZTD_PLATFORM_USE_ICU "Enable ICU with ztd::platform" OFF)
option(ZTD_PLATFORM_USE_ENCODING_C "Enable encoding_rs's C bindings with ztd::platform" OFF)

# # Dependencies
# ztd.idk
FetchContent_Declare(ztd.idk
	GIT_REPOSITORY https://github.com/soasis/idk.git
	GIT_TAG main)
FetchContent_MakeAvailable(ztd.idk)

# encoding_c
if (ZTD_PLATFORM_USE_ENCODING_C)
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
				"--all-features"
				"--message-format=json"
				"--target-dir=${encoding_rs_c_external_BINARY_DIR}"
				"--crate-type=$<IF:$<STREQUAL:${encoding_rs_c_type},SHARED>,cdylib,staticlib>"
				$<$<BOOL:$<CONFIG:Release,MinSizeRel,RelWithDebInfo>>:--release>
				> ${cargo_build_result_json_file}
			COMMENT "[ztd.platform] building rust library encoding_c..."
			WORKING_DIRECTORY ${encoding_rs_c_external_SOURCE_DIR}
		)
		add_custom_target(encoding_rs_c_external_build
			DEPENDS ${cargo_build_result_json_file}
			$<$<OR:$<BOOL:${WIN32}>,$<STREQUAL:${encoding_rs_c_type},STATIC>>:${encoding_rs_c_static_lib}>
			$<$<STREQUAL:${encoding_rs_c_type},SHARED>:${encoding_rs_c_dynamic_lib}>
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
				$<$<BOOL:${WIN32}>:Ws2_32.lib>
				$<$<BOOL:${WIN32}>:Userenv.lib>
				$<$<BOOL:${WIN32}>:BCrypt.lib>
				${CMAKE_DL_LIBS}
		)
		add_dependencies(encoding_rs_c encoding_rs_c_external_build)
	endfunction()
	encoding_rs_c_dependency_jail()
endif()
string(CONCAT ztd-platform-encoding_c-define
	$<IF:$<BOOL:${ZTD_PLATFORM_USE_ENCODING_C}>,
		ZTD_PLATFORM_ENCODING_C=1,
		ZTD_PLATFORM_ENCODING_C=0
	>
)

# iconv package
if (ZTD_PLATFORM_USE_LIBICONV)
	# # Iconv setup
	if (EXISTS ${Iconv_ROOT})
		add_library(iconv_shared SHARED IMPORTED)
		add_library(Iconv::Iconv ALIAS iconv_shared)
		set(iconv_root_location_dir ${Iconv_ROOT})
		cmake_path(APPEND iconv_imported_location_file ${Iconv_ROOT} "lib" "iconv.lib" )
		cmake_path(GET iconv_imported_location_file STEM iconv_binary_location_file)
		cmake_path(GET iconv_imported_location_file PARENT_PATH iconv_binary_location_dir)
		cmake_path(GET iconv_binary_location_dir PARENT_PATH iconv_root_location_dir)
		set(iconv_dll_path ${iconv_root_location_dir}/bin/${iconv_binary_location_file}${CMAKE_SHARED_LIBRARY_SUFFIX})
		if (NOT EXISTS ${iconv_dll_path})
			set(iconv_dll_path ${iconv_root_location_dir}/bin/${iconv_binary_location_file}-1${CMAKE_SHARED_LIBRARY_SUFFIX})
			if (NOT EXISTS ${iconv_dll_path})
				set(iconv_dll_path ${iconv_root_location_dir}/bin/${iconv_binary_location_file}1${CMAKE_SHARED_LIBRARY_SUFFIX})
				if (NOT EXISTS ${iconv_dll_path})
					set(iconv_dll_path ${iconv_root_location_dir}/bin/${iconv_binary_location_file}-2${CMAKE_SHARED_LIBRARY_SUFFIX})
					if (NOT EXISTS ${iconv_dll_path})
						set(iconv_dll_path ${iconv_root_location_dir}/bin/${iconv_binary_location_file}2${CMAKE_SHARED_LIBRARY_SUFFIX})
					endif()
				endif()
			endif()
		endif()
		cmake_path(GET iconv_binary_location_dir PARENT_PATH iconv_dll_path)
		cmake_path(APPEND charset_dll_path ${iconv_binary_location_dir} "charset-2.dll" )
		if (NOT EXISTS ${charset_dll_path})
			cmake_path(APPEND charset_dll_path ${iconv_binary_location_dir} "charset-1.dll" )
			if (NOT EXISTS ${charset_dll_path})
				cmake_path(APPEND charset_dll_path ${iconv_binary_location_dir} "charset.dll" )
			endif()
		endif()
		if (EXISTS ${charset_dll_path})
			add_library(charset_shared SHARED IMPORTED)
			add_library(Iconv::charset ALIAS charset_shared)
			set_target_properties(iconv_shared
				PROPERTIES
				IMPORTED_LOCATION ${iconv_dll_path}
				IMPORTED_IMPLIB ${iconv_imported_location_file}
			)
			target_link_libraries(iconv_shared
				INTERFACE Iconv::charset
			)
		endif()
		set_target_properties(iconv_shared
			PROPERTIES
			IMPORTED_LOCATION ${iconv_dll_path}
			IMPORTED_IMPLIB ${iconv_imported_location_file}
		)
		
		cmake_path(APPEND iconv_include_directories ${Iconv_ROOT} "include")
		if (NOT EXISTS ${iconv_include_directories})
			get_target_property(iconv_include_directories Iconv::Iconv INCLUDE_DIRECTORIES)
			if (NOT EXISTS ${iconv_include_directories})
				get_target_property(iconv_include_directories Iconv::Iconv INTERFACE_INCLUDE_DIRECTORIES)
				if (NOT EXISTS ${iconv_include_directories})
					set(iconv_include_directories ${iconv_root_location_dir}/include/)
				endif()
			endif()
		endif()
		target_include_directories(iconv_shared
			INTERFACE
				${iconv_include_directories}
		)
		get_target_property(Iconv_INCLUDE_DIRECTORIES Iconv::Iconv INTERFACE_INCLUDE_DIRECTORIES)
	else()
		find_package(Iconv REQUIRED)
	endif()
endif()
string(CONCAT ztd-platform-libiconv-define
	$<IF:$<BOOL:${ZTD_PLATFORM_USE_LIBICONV}>,
		ZTD_PLATFORM_LIBICONV=1,
		ZTD_PLATFORM_LIBICONV=0
	>
)
string(CONCAT ztd-platform-libiconv-load-define
	$<IF:$<AND:$<TARGET_EXISTS:Iconv::Iconv>,$<BOOL:${ZTD_PLATFORM_USE_LIBICONV}>>,
		ZTD_PLATFORM_LIBICONV_LOAD=0,
		ZTD_PLATFORM_LIBICONV_LOAD=1
	>
)
string(CONCAT ztd-platform-libiconv-header-define
	$<IF:$<AND:$<TARGET_EXISTS:Iconv::Iconv>,$<BOOL:${ZTD_PLATFORM_USE_LIBICONV}>>,
		ZTD_PLATFORM_ICONV_H=1,
		ZTD_PLATFORM_ICONV_H=0
	>
)
string(CONCAT ztd-platform-static-libiconv-define
	$<IF:
		$<AND:$<TARGET_EXISTS:Iconv::Iconv>, $<BOOL:${ZTD_TEXT_USE_STATIC_LIBICONV}>,
			$<STREQUAL:
				$<$<BOOL:$<TARGET_NAME_IF_EXISTS:Iconv::Iconv>>:$<TARGET_PROPERTY:Iconv::Iconv,TYPE>>,
				STATIC_LIBRARY
			>
		>,
		ZTD_PLATFORM_LIBICONV_STATIC=1,
		ZTD_PLATFORM_LIBICONV_STATIC=0
	>
)
string(CONCAT ztd-platform-dynamic-libiconv-define
	$<IF:
		$<AND:$<TARGET_EXISTS:Iconv::Iconv>, $<BOOL:ZTD_PLATFORM_USE_LIBICONV>,
			$<STREQUAL:
				$<$<BOOL:$<TARGET_NAME_IF_EXISTS:Iconv::Iconv>>:$<TARGET_PROPERTY:Iconv::Iconv,TYPE>>,
				SHARED_LIBRARY
			>
		>,
		ZTD_PLATFORM_LIBICONV_DYNAMIC=1,
		ZTD_PLATFORM_LIBICONV_DYNAMIC=0
	>
)

# # ICU package
if (ZTD_PLATFORM_USE_ICU)
	if (EXISTS ${ICU_ROOT})
		if (ICU_VERSION MATCHES "[^0-9]*([0-9]+)[.]([0-9]+)?[.]?([0-9]+)?[.]?([0-9]+)?.*")
			set(icu_lib_version_suffix ${CMAKE_MATCH_1})
		else()
			set(icu_lib_version_suffix 71)
		endif()
		add_library(icu_uc_shared SHARED IMPORTED)
		add_library(ICU::uc ALIAS icu_uc_shared)
		cmake_path(APPEND icu_uc_imported_location_file ${ICU_ROOT} "lib" "icuuc.lib")
		cmake_path(GET icu_uc_imported_location_file STEM LAST_ONLY icu_uc_binary_location_file)
		cmake_path(GET icu_uc_imported_location_file PARENT_PATH icu_uc_binary_location_dir)
		set(icu_uc_dll_path ${icu_uc_binary_location_dir}/../bin/${icu_uc_binary_location_file}${icu_lib_version_suffix}${CMAKE_SHARED_LIBRARY_SUFFIX})
		set_target_properties(icu_uc_shared
			PROPERTIES
			IMPORTED_LOCATION ${icu_uc_dll_path}
			IMPORTED_IMPLIB ${icu_uc_imported_location_file}
		)
		target_include_directories(icu_uc_shared
			INTERFACE
				${icu_uc_binary_location_dir}/../include/
		)
		
		add_library(icu_i18n_shared SHARED IMPORTED)
		add_library(ICU::i18n::shared ALIAS icu_i18n_shared)
		cmake_path(APPEND icu_i18n_imported_location_file ${ICU_ROOT} "lib" "icuin.lib")
		cmake_path(GET icu_i18n_imported_location_file STEM LAST_ONLY icu_i18n_binary_location_file)
		cmake_path(GET icu_i18n_imported_location_file PARENT_PATH icu_i18n_binary_location_dir)
		set(icu_1i8n_dll_path ${icu_i18n_binary_location_dir}/../bin/${icu_i18n_binary_location_file}${icu_lib_version_suffix}${CMAKE_SHARED_LIBRARY_SUFFIX})
		set_target_properties(icu_i18n_shared
			PROPERTIES
			IMPORTED_LOCATION ${icu_1i8n_dll_path}
			IMPORTED_IMPLIB ${icu_i18n_imported_location_file}
		)
		target_include_directories(icu_i18n_shared
			INTERFACE
				${icu_i18n_binary_location_dir}/../include/
		)
	else()
		find_package(ICU COMPONENTS uc i18n)
	endif()
endif()
string(CONCAT ztd-platform-icu-define
	$<IF:$<BOOL:${ZTD_PLATFORM_USE_ICU}>,
		ZTD_PLATFORM_ICU=1,
		ZTD_PLATFORM_ICU=0
	>
)


# # Main Library
file(GLOB ztd.platform.includes CONFIGURE_DEPENDS include/*.hpp)
file(GLOB_RECURSE ztd.platform.sources
	LIST_DIRECTORIES FALSE
	CONFIGURE_DEPENDS
	source/*.c source/*.cpp)
add_library(ztd.platform ${ztd.platform.sources})
add_library(ztd::platform ALIAS ztd.platform)
target_include_directories(ztd.platform
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${PROJECT_SOURCE_DIR}/source/include)
target_compile_definitions(ztd.platform
	PRIVATE
		ZTD_PLATFORM_BUILD=1
	PUBLIC
		$<$<STREQUAL:$<TARGET_PROPERTY:ztd.platform,TYPE>,SHARED_LIBRARY>:ZTD_PLATFORM_DLL=1>
		${ztd-platform-libiconv-define}
		${ztd-platform-libiconv-load-define}
		${ztd-platform-libiconv-header-define}
		${ztd-platform-static-libiconv-define}
		${ztd-platform-dynamic-libiconv-define}
		${ztd-platform-icu-define}
		${ztd-platform-encoding_c-define}
)
target_link_libraries(ztd.platform
	PUBLIC
		ztd::idk
		$<$<BOOL:${ZTD_PLATFORM_USE_LIBICONV}>:$<$<TARGET_EXISTS:Iconv::Iconv>:Iconv::Iconv>>
		$<$<BOOL:${ZTD_PLATFORM_USE_ICU}>:$<$<TARGET_EXISTS:ICU::uc>:ICU::uc>>
		$<$<BOOL:${ZTD_PLATFORM_USE_ICU}>:$<$<TARGET_EXISTS:ICU::i18n>:ICU::i18n>>
		$<$<BOOL:${ZTD_PLATFORM_USE_ENCODING_C}>:$<$<TARGET_EXISTS:encoding_rs::c>:encoding_rs::c>>
)


if(ZTD_PLATFORM_IS_TOP_LEVEL_PROJECT)
	target_compile_options(ztd.platform PRIVATE
		${--utf8-literal-encoding}
		${--utf8-source-encoding}
		${--disable-permissive}
		${--updated-cpp-version-flag}
		${--warn-pedantic}
		${--warn-all}
		${--warn-extra}
		${--warn-errors}
	)
endif()

target_sources(ztd.platform PRIVATE ${ztd.platform.includes})
install(DIRECTORY include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ztd.platform
	PUBLIC_HEADER
	RUNTIME
	ARCHIVE
	LIBRARY
)

# # Config / Version packaging
# Version configurations
configure_package_config_file(
	cmake/ztd.platform-config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.platform/ztd.platform-config.cmake"
	INSTALL_DESTINATION lib/cmake/ztd.platform
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.platform/ztd.platform-config-version.cmake"
	COMPATIBILITY SameMajorVersion)

export(TARGETS ztd.platform
	FILE
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.platform/ztd.platform-targets.cmake")

# # Benchmarks, Tests, Examples
if(ZTD_PLATFORM_TESTS)
	include(CTest)
	add_subdirectory(tests)
endif()

if(ZTD_PLATFORM_DOCUMENTATION)
	add_subdirectory(documentation)
endif()

# For quick debugging and development only: don't peek! 🙈
mark_as_advanced(ZTD_PLATFORM_SCRATCH)
if(ZTD_PLATFORM_SCRATCH)
	add_executable(scratch main.cpp)
	target_link_libraries(scratch PRIVATE ztd::platform)
	target_compile_options(scratch
		PRIVATE
		${--template-debugging-mode}
		${--disable-permissive}
		${--updated-cpp-version-flag}
		${--warn-pedantic}
		${--warn-all}
		${--warn-extra}
		${--warn-errors})
	target_compile_features(scratch PRIVATE cxx_std_20)
endif()
