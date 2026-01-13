cmake_minimum_required(VERSION 3.15)

project(godotcoacd)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Minimum policy version and skip install rules
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_SKIP_INSTALL_RULES ON)
# BEFORE add_subdirectory or FetchContent for OpenVDB
set(OPENVDB_BUILD_BINARIES OFF CACHE BOOL "Do not build OpenVDB binaries" FORCE)
set(OPENVDB_BUILD_VDB_PRINT OFF CACHE BOOL "Do not build vdb_print" FORCE)
set(TBB_WERROR OFF CACHE BOOL "Disable treating warnings as errors in TBB")

# MSVC-specific settings
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# Emscripten-specific settings
if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning-option -Wno-missing-template-arg-list-after-template-kw -s USE_PTHREADS=1 -s SIDE_MODULE=1")
    set(BOOST_CONTEXT_IMPLEMENTATION "ucontext")
endif()

if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning-option")
endif()

if (IOS)
    target_compile_options(tbb PRIVATE -Wno-shorten-64-to-32)
endif()

# Collect source files
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/CoACD-godot-src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp)

# Fetch GodotCPP suffix
get_target_property(NAME_SUFFIX godot-cpp GODOTCPP_SUFFIX)
get_target_property(PLATFORM godot-cpp GODOTCPP_PLATFORM)

message(STATUS "GodotCPP platform: ${PLATFORM}")

file(GLOB_RECURSE GODOT_COACD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Create shared laibrary
add_library(godotcoacd SHARED ${GODOT_COACD_SRC})

# Include GodotCPP module for documentation generation
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${godot-cpp_SOURCE_DIR}/cmake")
include(GodotCPPModule)

# Collect XML files for documentation
file(GLOB_RECURSE DOC_XML LIST_DIRECTORIES NO CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/doc_classes/*.xml")

# Conditionally add documentation sources
if(DOC_XML)
    if(GODOTCPP_TARGET MATCHES "editor|template_debug")
        target_doc_sources(godotcoacd ${DOC_XML})
    endif()
endif()

# Link libraries
target_link_libraries(godotcoacd godot-cpp coacd spdlog::spdlog)

if(MSVC)
    set(LIB_NAME "libgodotcoacd${NAME_SUFFIX}")
else()
    set(LIB_NAME "godotcoacd${NAME_SUFFIX}")
endif()

# Set target properties
set_target_properties(godotcoacd PROPERTIES
    OUTPUT_NAME ${LIB_NAME}
)

if (EMSCRIPTEN)
	SET_TARGET_PROPERTIES(godotcoacd PROPERTIES SUFFIX ".wasm")
endif()

set(GODOT_PROJECT_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/addons/godotcoacd/bin")

add_custom_command(TARGET godotcoacd POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:godotcoacd>" "${GODOT_PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:godotcoacd>"
)