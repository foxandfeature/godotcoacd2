#!/usr/bin/env python3

import os
import re
import sys
import shutil
import subprocess

# -------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------

unpackaged = ARGUMENTS.get("unpackaged", "false") == "true"

ROOT_DIR = os.getcwd()
COACD_SRC_DIR = "CoACD-godot-src"
COACD_BUILD_DIR = os.path.join(COACD_SRC_DIR, "build")

ADDONS_SRC = "addons"
BUILD_DIR = "build"
ADDONS_DIR = os.path.join(BUILD_DIR, "addons")
PLUGIN_DIR = os.path.join(ADDONS_DIR, "godotcoacd")
BIN_DIR = BUILD_DIR if unpackaged else os.path.join(PLUGIN_DIR, "bin")

LICENSE_SRC = "LICENSE.txt"
THIRD_PARTY_LICENSE_DIR = os.path.join(PLUGIN_DIR, "third-party-licenses")

THIRD_PARTY_LICENSES = {
    "boost": ["CoACD-godot-src/build/_deps/boost-src/LICENSE_1_0.txt"],
    "openvdb": ["CoACD-godot-src/build/_deps/openvdb-src/LICENSE"],
    "spdlog": ["CoACD-godot-src/build/_deps/spdlog-src/LICENSE"],
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
    os.makedirs(THIRD_PARTY_LICENSE_DIR, exist_ok=True)
    for lib, files in THIRD_PARTY_LICENSES.items():
        dest = os.path.join(THIRD_PARTY_LICENSE_DIR, lib)
        os.makedirs(dest, exist_ok=True)
        for src in files:
            shutil.copy2(
                src,
                os.path.join(dest, os.path.splitext(os.path.basename(src))[0] + ".txt"),
            )


def build_coacd(cmake_args, build_cmd):
    if GetOption("clean") or ARGUMENTS.get("skip_coacd_build", "false") == "true":
        return
    run(["cmake", "..", *cmake_args], cwd=COACD_BUILD_DIR)
    run(build_cmd, cwd=COACD_BUILD_DIR)


def find_tbb_build_dir(prefix, runtime=None):
    ARCH_MAP = {
        "arm32": 32,
        "arm64": 64,
        "x86_32": 32,
        "x86_64": 64,
    }

    bitness = ARCH_MAP.get(env.get("arch"))
    if bitness is None:
        return None

    try:
        for entry in os.scandir(COACD_BUILD_DIR):
            if not entry.is_dir():
                continue

            if re.search(
                rf"^{prefix}_.*_cxx20_{bitness}_{runtime + '_' if runtime else ''}release$",
                entry.name,
            ):
                return entry.path
    except FileNotFoundError:
        return None


# -------------------------------------------------------------------
# Setup
# -------------------------------------------------------------------

ensure_dirs()
if not unpackaged:
    copy_addons()

env = SConscript("godot-cpp/SConstruct")

# -------------------------------------------------------------------
# Build configuration
# -------------------------------------------------------------------

env.Append(
    CPPPATH=[
        "src/",
        "CoACD-godot-src/build/_deps/spdlog-src/include",
    ],
    LIBPATH=[
        COACD_BUILD_DIR,
        os.path.join(COACD_BUILD_DIR, "Release"),
        os.path.join(COACD_BUILD_DIR, "_deps/spdlog-build"),
        os.path.join(COACD_BUILD_DIR, "_deps/spdlog-build/Release"),
        os.path.join(COACD_BUILD_DIR, "_deps/openvdb-build/openvdb/openvdb"),
        os.path.join(COACD_BUILD_DIR, "_deps/openvdb-build/openvdb/openvdb/Release"),
    ],
    LIBS=["coacd", "spdlog"],
)

sources = list(Glob("src/*.cpp"))

# Platform-specific flags
platform_flags = {
    "unix": {"CXXFLAGS": ["-fexceptions"], "LIBS": ["openvdb", "tbb"]},
    "windows": {
        "CXXFLAGS": ["/EHsc"],
        "CPPDEFINES": ["SPDLOG_COMPILED_LIB"],
        "LIBS": ["libopenvdb", "tbb12"],
    },
}

if env["platform"] in ("macos", "linux", "android"):
    env.Append(**platform_flags["unix"])
elif env["platform"] == "windows":
    env.Append(**platform_flags["windows"])

# -------------------------------------------------------------------
# CoACD Build
# -------------------------------------------------------------------


def get_android_ndk():
    return os.path.join(os.environ.get("ANDROID_HOME", ""), "ndk/28.1.13356709")


def get_android_abi():
    return {
        "arm32": "armeabi-v7a",
        "arm64": "arm64-v8a",
        "x86_32": "x86",
        "x86_64": "x86_64",
    }.get(env["arch"])


def get_windows_cmake_arch():
    return {
        "x86_64": "x64",
        "x86_32": "Win32",
        "arm64": "ARM64",
        # arm32 is intentionally not supported by MSVC
    }.get(env["arch"])


cmake_args = []
build_cmd = []

# Common Android setup
if env["platform"] == "android":
    NDK = get_android_ndk()
    ABI = get_android_abi()
    cmake_args = [
        "-G Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
        f"-DCMAKE_TOOLCHAIN_FILE={NDK}/build/cmake/android.toolchain.cmake",
        f"-DANDROID_ABI={ABI}",
        "-DANDROID_PLATFORM=android-21",
        "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
    ]
    if sys.platform == "win32":
        cmake_args += [
            "-DCMAKE_CXX_FLAGS=-Wno-error=unknown-warning-option",
            "-DCMAKE_C_FLAGS=-Wno-error=unknown-warning-option",
        ]
        pass
    build_cmd = ["cmake", "--build", ".", "--target", "main", "--config", "Release"]

# Non-Android setups
elif sys.platform in ("darwin", "linux") and env["platform"] in ("macos", "linux"):
    cmake_args = ["-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"]
    build_cmd = ["make", "main", "-j"]

elif sys.platform == "win32" and env["platform"] == "windows":
    cmake_args = [
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded",
        "-DTBB_TEST=OFF",
        "-DOPENVDB_CORE_SHARED=OFF",
        "-DCMAKE_CXX_FLAGS=/MT /EHsc",
        "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
    ]
   
    if env["arch"] == "arm64":
        cmake_args += ["-DCMAKE_CXX_FLAGS=\"-DBT_USE_NEON \""]

    win_arch = get_windows_cmake_arch()
    if not win_arch:
        raise RuntimeError(f"Unsupported Windows architecture '{env['arch']}'. ")

    cmake_args += ["-A", win_arch]

    build_cmd = ["cmake", "--build", ".", "--target", "main", "--config", "Release"]

else:
    raise RuntimeError(
        f"Unsupported system platform '{sys.platform}' "
        f"and target platform '{env['platform']}'"
    )

build_coacd(cmake_args, build_cmd)

# -------------------------------------------------------------------
# Locate TBB
# -------------------------------------------------------------------

tbb_map = {
    ("macos", "darwin"): "appleclang",
    ("linux", "linux"): "gnu",
    ("windows", "win32"): ("msvc", "mt"),
    ("android", "win32"): "clang",
    ("android", "linux"): "clang",
}

key = (env["platform"], sys.platform)
tbb_arg = tbb_map[key]
if isinstance(tbb_arg, tuple):
    tbb_build_dir = find_tbb_build_dir(*tbb_arg)
else:
    tbb_build_dir = find_tbb_build_dir(tbb_arg)

if not tbb_build_dir:
    raise RuntimeError("TBB folder not found")
env.Append(LIBPATH=tbb_build_dir)

# -------------------------------------------------------------------
# Documentation & licenses
# -------------------------------------------------------------------

if not unpackaged:
    shutil.copy(LICENSE_SRC, PLUGIN_DIR)
    copy_third_party_licenses()

if env["target"] in ("editor", "template_debug"):
    try:
        doc = env.GodotCPPDocData(
            "src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml")
        )
        sources.append(doc)
    except AttributeError:
        print("Class reference generation not supported.")

# -------------------------------------------------------------------
# Build output
# -------------------------------------------------------------------

if env["platform"] == "macos":
    library = env.SharedLibrary(
        os.path.join(
            BIN_DIR,
            f"libgodotcoacd.{env['platform']}.{env['target']}.framework/"
            f"libgodotcoacd.{env['platform']}.{env['target']}",
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    suffix = ".simulator.a" if env.get("ios_simulator") else ".a"
    library = env.StaticLibrary(
        os.path.join(
            BIN_DIR,
            f"libgodotcoacd.{env['platform']}.{env['target']}{suffix}",
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

Default(library if unpackaged else zip_target)
