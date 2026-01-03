import os
import re
import subprocess
import sys

# -------------------------------------------------------------------
# Constants
# -------------------------------------------------------------------

COACD_SRC = "CoACD-godot-src"
COACD_BUILD = os.path.join(COACD_SRC, "build")

COMMON_CMAKE_FLAGS = [
    "-DCMAKE_BUILD_TYPE=Release",
    "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
]

DEFAULT_BUILD_CMD = ["cmake", "--build", ".", "--target", "main", "--config", "Release"]

# Mapping architectures
ANDROID_ABI_MAP = {
    "arm32": "armeabi-v7a",
    "arm64": "arm64-v8a",
    "x86_32": "x86",
    "x86_64": "x86_64",
}

MSVC_ARCH_MAP = {
    "x86_64": "x64",
    "x86_32": "Win32",
    "arm64": "ARM64",
}

ARCH_BITS = {
    "arm32": 32,
    "arm64": 64,
    "x86_32": 32,
    "x86_64": 64,
}

# Platform-specific cmake arguments
PLATFORM_CMAKE_ARGS = {
    "android": lambda env: [
        "-G", "Ninja",
        f"-DCMAKE_TOOLCHAIN_FILE={android_ndk(env)}/build/cmake/android.toolchain.cmake",
        f"-DANDROID_ABI={ANDROID_ABI_MAP[env['arch']]}",
        "-DANDROID_PLATFORM=android-21",
    ],
    "web": lambda env: ["-DCMAKE_SKIP_INSTALL_RULES=ON"],
    "windows": lambda env: [],
}

PLATFORM_PREFIX = {
    "windows": {},
    "web": {
        "configure": ["cmd", "/c", "emcmake"],
        "build": ["cmd", "/c", "emmake"],
    }
}

COMPILER_CMAKE_ARGS = {
    "msvc": lambda env: [
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded",
        "-DCMAKE_CXX_FLAGS=/MT /EHsc /MP",
        "-A", MSVC_ARCH_MAP[env["arch"]],
    ],
    "mingw": lambda env: [
        "-DCMAKE_CXX_FLAGS="
        "-Wno-missing-template-arg-list-after-template-kw "
        "-Wno-unknown-warning-option "
        "-s USE_PTHREADS=1",
    ],
    "clang": lambda env: ["-DCMAKE_CXX_FLAGS=-Wno-unknown-warning-option"],
}

COMPILER_BUILD_ARGS = {
    "msvc": lambda env: ["--parallel"],
    "mingw": lambda env: [f"-j{(os.cpu_count() - 1) or 1}"],
    "clang": lambda env: [],
}

COMPILER_LIBS = {
    "msvc": {"LIBS": ["coacd", "libopenvdb", "tbb12", "spdlog"]},
    "clang": {"LIBS": ["coacd", "openvdb", "tbb", "spdlog"]},
}

PLATFORM_LIBS = {
    "windows": {},
    "web": {"LIBS": ["coacd"]},
}

TBB_MAP = {
    ("clang", "darwin"): ("appleclang", None),
    ("gnu", None): ("gnu", None),
    ("msvc", None): ("msvc", "mt"),
    ("clang", None): ("clang", None),
}

# -------------------------------------------------------------------
# Helper functions
# -------------------------------------------------------------------

def run(cmd, cwd):
    """Run a subprocess command in the specified directory."""
    subprocess.run(cmd, cwd=cwd, check=True)


def android_ndk(env):
    """Return the Android NDK path."""
    return env["ANDROID_HOME"] + "/ndk/" + env["ndk_version"]


def android_abi(arch):
    return ANDROID_ABI_MAP[arch]


def msvc_arch(arch):
    return MSVC_ARCH_MAP[arch]

def find_tbb_dir(env, prefix, runtime=None):
    """Locate TBB directory in the build folder."""
    bits = ARCH_BITS[(env["arch"])]
    if not bits:
        return None

    pattern = rf"^{prefix}_.*_cxx20_{bits}_{runtime + '_' if runtime else ''}release$"
    for d in os.scandir(COACD_BUILD):
        if d.is_dir() and re.match(pattern, d.name):
            return d.path
    return None


# -------------------------------------------------------------------
# Build functions
# -------------------------------------------------------------------

def build(env, compiler):
    os.makedirs(COACD_BUILD, exist_ok=True)

    platform = env["platform"]

    # ---------------- Configure ----------------
    cmake_cmd = ["cmake", "..", *COMMON_CMAKE_FLAGS]
    cmake_cmd += PLATFORM_CMAKE_ARGS[platform](env)
    cmake_cmd += COMPILER_CMAKE_ARGS[compiler](env)

    prefix = PLATFORM_PREFIX.get(platform, {}).get("configure", [])
    run([*prefix, *cmake_cmd], COACD_BUILD)

    # ---------------- Build ----------------
    build_prefix = PLATFORM_PREFIX.get(platform, {}).get("build", [])
    build_cmd = [*build_prefix, *DEFAULT_BUILD_CMD]
    build_cmd += COMPILER_BUILD_ARGS[compiler](env)
    run(build_cmd, COACD_BUILD)


def setup_linking(env, compiler):
    env.Append(
        LIBPATH=[
            COACD_BUILD,
            os.path.join(COACD_BUILD, "Release"),
            os.path.join(COACD_BUILD, "_deps", "spdlog-build", "Release"),
            os.path.join(COACD_BUILD, "_deps", "openvdb-build", "openvdb", "openvdb", "Release"),
        ]
    )

    env.Append(**COMPILER_LIBS[compiler])
    env.Append(**PLATFORM_LIBS[env["platform"]])

    if env["platform"] != "web":
        key = (compiler, sys.platform)
        prefix, runtime = TBB_MAP.get(key, TBB_MAP[(compiler, None)])
        tbb_dir = find_tbb_dir(env, prefix, runtime)
        if not tbb_dir:
            raise RuntimeError("TBB build not found")
        env.Append(LIBPATH=[tbb_dir])