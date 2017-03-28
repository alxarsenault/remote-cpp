cmake_minimum_required(VERSION 3.5.0)

# Check if we run cmake at root of workspace.
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Source and build directories cannot be the same.")
endif ()

project(rmte)
set(RMTE_VERSION 0.0.1)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_EXAMPLES "Build with examples" ON)

# Use folder.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(RMTE_INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/include/)

# Debug output path.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Debug)

# Release output path.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Release)

# Set debug as default build type.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CONFIG_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Release)
else ()
    set(CONFIG_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Debug)
endif ()

# Compiler flags.
set(CMAKE_CXX_FLAGS "-fPIC -std=c++14 -Wall -Wextra ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${CMAKE_CXX_FLAGS_RELEASE}")

find_package(Boost COMPONENTS system REQUIRED)

file(GLOB_RECURSE RMTE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include/rmte/*.hpp)
file(GLOB FOLDER_FILE_RMTE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include/rmte/*.hpp)

source_group("include" FILES ${FOLDER_FILE_RMTE_HEADER_FILES})

# Dummy project.
add_library(rmte_header STATIC ${RMTE_HEADER_FILES})
set_target_properties(rmte_header PROPERTIES LINKER_LANGUAGE CXX)

# Install Package Configuration
set(RMTE_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/rmte")

install(FILES ${RMTE_HEADER_FILES} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/rmte")

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/rmteConfigVersion.cmake"
	VERSION ${RMTE_VERSION}
	COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
	cmake/rmteConfig.cmake.in
	${RMTE_INSTALL_CONFIGDIR}/rmteConfig.cmake
	INSTALL_DESTINATION ${RMTE_INSTALL_CONFIGDIR}
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${RMTE_INSTALL_CONFIGDIR}/rmteConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/rmteConfigVersion.cmake"
	DESTINATION ${RMTE_INSTALL_CONFIGDIR}
)

add_library(rmte INTERFACE)
target_include_directories(rmte INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
install(TARGETS rmte EXPORT rmteTargets)

export(EXPORT rmteTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/rmteTargets.cmake"
)

install(
	EXPORT rmteTargets FILE rmteTargets.cmake
	DESTINATION ${RMTE_INSTALL_CONFIGDIR}
)

export(PACKAGE rmte)

# Examples.
if(BUILD_EXAMPLES)
	add_subdirectory(example/basic/client)
	add_subdirectory(example/basic/server)
endif(BUILD_EXAMPLES)
