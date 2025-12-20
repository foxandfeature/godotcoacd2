#!/usr/bin/env python3
import os
import sys
import shutil
import subprocess

# -------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------

COACD_BUILD_DIR = "CoACD-godot-src/build"
ADDONS_SRC = "addons"

BUILD_DIR = "build"
ADDONS_DIR = os.path.join(BUILD_DIR, "addons")
PLUGIN_DIR = os.path.join(ADDONS_DIR, "godotcoacd")
BIN_DIR = os.path.join(PLUGIN_DIR, "bin")

LICENSE_SRC = "LICENSE.txt"

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
THIRD_PARTY_LICENSE_DIR = os.path.join(PLUGIN_DIR, "third-party-licenses")

# -------------------------------------------------------------------
# Directory setup
# -------------------------------------------------------------------

os.makedirs(COACD_BUILD_DIR, exist_ok=True)

# Copy extension descriptor
shutil.copytree(ADDONS_SRC, ADDONS_DIR, dirs_exist_ok=True)

# Load Godot C++ environment
env = SConscript("godot-cpp/SConstruct")

# -------------------------------------------------------------------
# Build configuration
# -------------------------------------------------------------------

# Include paths
env.Append(CPPPATH=[
    "src/",
    "CoACD-godot-src/build/_deps/spdlog-src/include",
])

# Source files
sources = [Glob("src/*.cpp")]

# Library paths
env.Append(LIBPATH=[
    COACD_BUILD_DIR,
    os.path.join(COACD_BUILD_DIR, "_deps/spdlog-build"),
    os.path.join(COACD_BUILD_DIR, "_deps/openvdb-build/openvdb/openvdb"),
    os.path.join(COACD_BUILD_DIR, "appleclang_17.0_cxx20_64_release"),
])

# Libraries
env.Append(LIBS=["coacd", "spdlog", "openvdb", "tbb"])

# Compiler flags
env.Append(CXXFLAGS=["-fexceptions"])

platform = ""

def copy_third_party_licenses():
    """Copy each third-party license file into its own subfolder, preserving filenames."""
    os.makedirs(THIRD_PARTY_LICENSE_DIR, exist_ok=True)

    for lib_name, src_list in THIRD_PARTY_LICENSES.items():
        dest_dir = os.path.join(THIRD_PARTY_LICENSE_DIR, lib_name)
        os.makedirs(dest_dir, exist_ok=True)

        for src_path in src_list:
            base = os.path.splitext(os.path.basename(src_path))[0]
            dest_path = os.path.join(dest_dir, base + ".txt")

            shutil.copy2(src_path, dest_path)

# -------------------------------------------------------------------
# macOS: build CoACD-godot-src dependency
# -------------------------------------------------------------------

if sys.platform == "darwin":
    platform = "macos"
    if (
        not GetOption("clean")
        and ARGUMENTS.get("skip_coacd_build", "false") != "true"
    ):
        subprocess.run(
            [
                "cmake",
                "..",
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
            ],
            cwd=COACD_BUILD_DIR,
            check=True,
        )
        subprocess.run(
            ["make", "main", "-j"],
            cwd=COACD_BUILD_DIR,
            check=True,
        )

# -------------------------------------------------------------------
# Optional: generate documentation sources
# -------------------------------------------------------------------

shutil.copy(LICENSE_SRC, PLUGIN_DIR)
copy_third_party_licenses()

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData(
            "src/gen/doc_data.gen.cpp",
            source=Glob("doc_classes/*.xml"),
        )
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference (pre-4.3 baseline).")

# -------------------------------------------------------------------
# Build library target
# -------------------------------------------------------------------

if platform == "macos":
    print(env["CCFLAGS"])
    library = env.SharedLibrary(
        os.path.join(
            BIN_DIR,
            "libgodotcoacd.{0}.{1}.framework/libgodotcoacd.{0}.{1}".format(
                platform, env["target"]
            ),
        ),
        source=sources,
    )

elif platform == "ios":
    if env.get("ios_simulator"):
        library = env.StaticLibrary(
            os.path.join(
                BIN_DIR,
                "libgodotcoacd.{0}.{1}.simulator.a".format(
                    platform, env["target"]
                ),
            ),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            os.path.join(
                BIN_DIR,
                "libgodotcoacd.{0}.{1}.a".format(platform, env["target"])
            ),
            source=sources,
        )

else:
    library = env.SharedLibrary(
        os.path.join(
            BIN_DIR,
            "libgodotcoacd{0}{1}".format(env["suffix"], env["SHLIBSUFFIX"])
        ),
        source=sources,
    )

# -------------------------------------------------------------------
# Packaging
# -------------------------------------------------------------------

def cleanup(target, source, env):
    """Archive build and clean up temporary files."""
    archive_path = os.path.join(BUILD_DIR, "GodotCoACD")
    shutil.make_archive(
        archive_path,
        "zip",
        root_dir=BUILD_DIR,
        base_dir="addons",
    )

zip_file = os.path.join(BUILD_DIR, "GodotCoACD-godot-src.zip")
zip_target = Command(zip_file, library, cleanup)

Default([zip_target])