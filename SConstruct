#!/usr/bin/env python3

import os
import sys
import shutil
import subprocess

# -------------------------------------------------------------------
# Paths & configuration
# -------------------------------------------------------------------

ROOT_DIR = os.getcwd()

COACD_SRC_DIR = "CoACD-godot-src"
COACD_BUILD_DIR = os.path.join(COACD_SRC_DIR, "build")

ADDONS_SRC = "addons"
BUILD_DIR = "build"
ADDONS_DIR = os.path.join(BUILD_DIR, "addons")
PLUGIN_DIR = os.path.join(ADDONS_DIR, "godotcoacd")
BIN_DIR = os.path.join(PLUGIN_DIR, "bin")

LICENSE_SRC = "LICENSE.txt"
THIRD_PARTY_LICENSE_DIR = os.path.join(PLUGIN_DIR, "third-party-licenses")

THIRD_PARTY_LICENSES = {
    "boost": [
        "CoACD-godot-src/build/_deps/boost-src/LICENSE_1_0.txt",
    ],
    "openvdb": [
        "CoACD-godot-src/build/_deps/openvdb-src/LICENSE",
    ],
    "spdlog": [
        "CoACD-godot-src/build/_deps/spdlog-src/LICENSE",
    ],
    "tbb": [
        "CoACD-godot-src/build/_deps/tbb-src/LICENSE.txt",
        "CoACD-godot-src/build/_deps/tbb-src/third-party-programs.txt",
    ],
}

# -------------------------------------------------------------------
# Helpers
# -------------------------------------------------------------------


def run(cmd, cwd):
    subprocess.run(cmd, cwd=cwd, check=True)


def ensure_dirs():
    os.makedirs(COACD_BUILD_DIR, exist_ok=True)
    os.makedirs(BIN_DIR, exist_ok=True)


def copy_addons():
    shutil.copytree(ADDONS_SRC, ADDONS_DIR, dirs_exist_ok=True)


def copy_third_party_licenses():
    """Copy third-party license files into subfolders."""
    os.makedirs(THIRD_PARTY_LICENSE_DIR, exist_ok=True)

    for lib, files in THIRD_PARTY_LICENSES.items():
        dest = os.path.join(THIRD_PARTY_LICENSE_DIR, lib)
        os.makedirs(dest, exist_ok=True)

        for src in files:
            name = os.path.splitext(os.path.basename(src))[0] + ".txt"
            shutil.copy2(src, os.path.join(dest, name))


def build_coacd(cmake_args, build_cmd):
    if GetOption("clean"):
        return
    if ARGUMENTS.get("skip_coacd_build", "false") == "true":
        return

    run(["cmake", "..", *cmake_args], cwd=COACD_BUILD_DIR)
    run(build_cmd, cwd=COACD_BUILD_DIR)


# -------------------------------------------------------------------
# Setup
# -------------------------------------------------------------------

ensure_dirs()
copy_addons()

# Load Godot C++ environment
env = SConscript("godot-cpp/SConstruct")

# -------------------------------------------------------------------
# Build configuration
# -------------------------------------------------------------------

env.Append(
    CPPPATH=[
        "src/",
        "CoACD-godot-src/build/_deps/spdlog-src/include",
    ]
)

env.Append(
    LIBPATH=[
        COACD_BUILD_DIR,
        os.path.join(COACD_BUILD_DIR, "Release"),
        os.path.join(COACD_BUILD_DIR, "_deps/spdlog-build"),
        os.path.join(COACD_BUILD_DIR, "_deps/spdlog-build/Release"),
        os.path.join(COACD_BUILD_DIR, "_deps/openvdb-build/openvdb/openvdb"),
        os.path.join(COACD_BUILD_DIR, "_deps/openvdb-build/openvdb/openvdb/Release"),
        os.path.join(COACD_BUILD_DIR, "appleclang_17.0_cxx20_64_release"),
        os.path.join(COACD_BUILD_DIR, "msvc_19.44_cxx20_64_mt_release"),
    ]
)

env.Append(LIBS=["coacd", "spdlog"])

sources = list(Glob("src/*.cpp"))
platform = None

# -------------------------------------------------------------------
# Platform-specific configuration
# -------------------------------------------------------------------

if sys.platform == "darwin":
    platform = "macos"

    env.Append(CXXFLAGS=["-fexceptions"], LIBS=["openvdb", "tbb", "spdlog"])

    build_coacd(
        cmake_args=[
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
        ],
        build_cmd=["make", "main", "-j"],
    )

elif sys.platform == "win32":
    platform = "windows"

    env.Append(
        CXXFLAGS=["/EHsc"],
        CPPDEFINES=["SPDLOG_COMPILED_LIB"],
        LIBS=["libopenvdb", "tbb12"],
    )

    build_coacd(
        cmake_args=[
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded",
            "-DOPENVDB_CORE_SHARED=OFF",
            "-DTBB_TEST=OFF",
            "-DCMAKE_CXX_FLAGS=/MT",
            "-DCMAKE_CXX_FLAGS=/EHsc",
            "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
        ],
        build_cmd=[
            "cmake",
            "--build",
            ".",
            "--target",
            "main",
            "--config",
            "Release",
        ],
    )

# -------------------------------------------------------------------
# Documentation
# -------------------------------------------------------------------

shutil.copy(LICENSE_SRC, PLUGIN_DIR)
copy_third_party_licenses()

if env["target"] in ("editor", "template_debug"):
    try:
        doc = env.GodotCPPDocData(
            "src/gen/doc_data.gen.cpp",
            source=Glob("doc_classes/*.xml"),
        )
        sources.append(doc)
    except AttributeError:
        print("Class reference generation not supported.")

# -------------------------------------------------------------------
# Build output
# -------------------------------------------------------------------

if platform == "macos":
    library = env.SharedLibrary(
        os.path.join(
            BIN_DIR,
            f"libgodotcoacd.{platform}.{env['target']}.framework/"
            f"libgodotcoacd.{platform}.{env['target']}",
        ),
        source=sources,
    )
elif platform == "ios":
    suffix = ".simulator.a" if env.get("ios_simulator") else ".a"
    library = env.StaticLibrary(
        os.path.join(
            BIN_DIR,
            f"libgodotcoacd.{platform}.{env['target']}{suffix}",
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        os.path.join(
            BIN_DIR,
            f"libgodotcoacd{env['suffix']}{env['SHLIBSUFFIX']}",
        ),
        source=sources,
    )

# -------------------------------------------------------------------
# Packaging
# -------------------------------------------------------------------


def package(target, source, env):
    shutil.make_archive(
        os.path.join(BUILD_DIR, "GodotCoACD"),
        "zip",
        root_dir=BUILD_DIR,
        base_dir="addons",
    )


zip_target = Command(
    os.path.join(BUILD_DIR, "GodotCoACD-godot-src.zip"),
    library,
    package,
)

Default(zip_target)
