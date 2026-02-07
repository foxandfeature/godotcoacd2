cmake_minimum_required(VERSION 3.15)

project(godotcoacd LANGUAGES CXX)

# ------------------------------------------------------------------------------
# Global configuration
# ------------------------------------------------------------------------------

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_SKIP_INSTALL_RULES ON)

# Disable unused OpenVDB components
set(OPENVDB_BUILD_BINARIES OFF CACHE BOOL "" FORCE)
set(OPENVDB_BUILD_VDB_PRINT OFF CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# Platform-specific configuration
# ------------------------------------------------------------------------------

if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning-option -Wno-missing-template-arg-list-after-template-kw -s USE_PTHREADS=1 -s SIDE_MODULE=1")
    set(BOOST_CONTEXT_IMPLEMENTATION "ucontext")
endif()

if(ANDROID)
    add_compile_options(-Wno-unknown-warning-option)
endif()

if(IOS)
    add_compile_options(-Wno-shorten-64-to-32)
endif()

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

add_subdirectory(CoACD)
add_subdirectory(godot-cpp)

# Fetch GodotCPP suffix
get_target_property(NAME_SUFFIX godot-cpp GODOTCPP_SUFFIX)

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

file(GLOB_RECURSE GODOT_COACD_SRC
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ------------------------------------------------------------------------------
# Library target
# ------------------------------------------------------------------------------

if(IOS)
    add_library(godotcoacd STATIC ${GODOT_COACD_SRC})
else()
    add_library(godotcoacd SHARED ${GODOT_COACD_SRC})
endif()

set_target_properties(godotcoacd PROPERTIES
    CXX_STANDARD 17
)

target_link_libraries(godotcoacd
    PRIVATE
        godot-cpp
        coacd
        spdlog::spdlog
)

# ------------------------------------------------------------------------------
# Godot documentation support
# ------------------------------------------------------------------------------

set(CMAKE_MODULE_PATH
    "${CMAKE_MODULE_PATH};${godot-cpp_SOURCE_DIR}/cmake"
)
include(GodotCPPModule)

file(GLOB_RECURSE DOC_XML
    LIST_DIRECTORIES NO
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/doc_classes/*.xml"
)

if(DOC_XML AND GODOTCPP_TARGET MATCHES "editor|template_debug")
    message(STATUS "SOURCES = ${DOC_XML}")
    target_doc_sources(godotcoacd "${DOC_XML}")
endif()

# ------------------------------------------------------------------------------
# TARGET OUTPUT CONFIGURATION
# ------------------------------------------------------------------------------

if(MSVC)
    set(LIB_NAME "libgodotcoacd${NAME_SUFFIX}")
else()
    set(LIB_NAME "godotcoacd${NAME_SUFFIX}")
endif()

set_target_properties(godotcoacd PROPERTIES
    OUTPUT_NAME "${LIB_NAME}"
)

if(EMSCRIPTEN)
    set_target_properties(godotcoacd PROPERTIES SUFFIX ".wasm")
endif()

if(IOS)
    set(GODOT_PROJECT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
else()
    set(GODOT_PROJECT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/godotcoacd_release")
endif()

add_custom_command(
    TARGET godotcoacd POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:godotcoacd>"
        "${GODOT_PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:godotcoacd>"
)

message(STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")