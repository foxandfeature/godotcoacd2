# coacd_build.py
import os
import sys
import subprocess


COMMON_CMAKE_FLAGS = [
    "-DCMAKE_BUILD_TYPE=Release",
    "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
]


def run(cmd, cwd):
    subprocess.run(cmd, cwd=cwd, check=True)


def android_ndk():
    return os.path.join(os.environ.get("ANDROID_HOME", ""), "ndk/28.1.13356709")


def android_abi(arch):
    return {
        "arm32": "armeabi-v7a",
        "arm64": "arm64-v8a",
        "x86_32": "x86",
        "x86_64": "x86_64",
    }.get(arch)


def windows_arch(arch):
    return {
        "x86_64": "x64",
        "x86_32": "Win32",
        "arm64": "ARM64",
    }.get(arch)


# -------------------------------------------------------------------
# Platform configuration (data, not logic)
# -------------------------------------------------------------------

CMAKE_ARGS = {
    "unix": [],
    "android": lambda env: [
        "-G", "Ninja",
        f"-DCMAKE_TOOLCHAIN_FILE={android_ndk()}/build/cmake/android.toolchain.cmake",
        f"-DANDROID_ABI={android_abi(env['arch'])}",
        "-DANDROID_PLATFORM=android-21",
    ],
    "windows": lambda env: [
        "-A", windows_arch(env["arch"]),
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded",
        "-DCMAKE_CXX_FLAGS=/MT /EHsc",
    ],
    "web": [
        "-DCMAKE_SKIP_INSTALL_RULES=ON",
        "-DCMAKE_CXX_FLAGS=-Wno-missing-template-arg-list-after-template-kw "
        "-Wno-unknown-warning-option "
        "-s USE_PTHREADS=1"
    ],
}

DEFAULT_BUILD_CMD = ["cmake", "--build", ".", "--target", "main", "--config", "Release"]

# Platform-specific overrides
BUILD_CMD = {
    "web": ["cmd", "/c", "emmake", *DEFAULT_BUILD_CMD],
}


# -------------------------------------------------------------------
# Entry point
# -------------------------------------------------------------------

def build(env, build_dir, clean=False, skip=False):
    if clean or skip:
        return

    platform = env["platform"]
    family = "unix" if platform in ("macos", "linux") else platform

    # --- Configure ---
    cmake_cmd = ["cmake", "..", *COMMON_CMAKE_FLAGS]

    args = CMAKE_ARGS[family]
    cmake_cmd += args(env) if callable(args) else args

    # Web uses emcmake (MinGW/Emscripten)
    if family == "web":
        cmake_cmd = ["cmd", "/c", "emcmake", *cmake_cmd]

    run(cmake_cmd, build_dir)

    # --- Build ---
    build_cmd = BUILD_CMD.get(family, DEFAULT_BUILD_CMD)
    run(build_cmd, build_dir)