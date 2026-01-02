#!/usr/bin/env python3

import os
import re
import sys
import shutil

from coacd_build import build as build_coacd

# -------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------

unpackaged = ARGUMENTS.get("unpackaged", "false") == "true"

ROOT = os.getcwd()
COACD_SRC = "CoACD-godot-src"
COACD_BUILD = os.path.join(COACD_SRC, "build")

BUILD_DIR = "build"
ADDONS_DIR = os.path.join(BUILD_DIR, "addons")
PLUGIN_DIR = os.path.join(ADDONS_DIR, "godotcoacd")
BIN_DIR = BUILD_DIR if unpackaged else os.path.join(PLUGIN_DIR, "bin")

LICENSE_SRC = "LICENSE.txt"
THIRD_PARTY_DIR = os.path.join(PLUGIN_DIR, "third-party-licenses")

THIRD_PARTY_LICENSES = {
    "boost": ["build/_deps/boost-src/LICENSE_1_0.txt"],
    "openvdb": ["build/_deps/openvdb-src/LICENSE"],
    "spdlog": ["build/_deps/spdlog-src/LICENSE"],
    "tbb": [
        "build/_deps/tbb-src/LICENSE.txt",
        "build/_deps/tbb-src/third-party-programs.txt",
    ],
}

# -------------------------------------------------------------------
# Helpers
# -------------------------------------------------------------------


def ensure_dirs():
    os.makedirs(COACD_BUILD, exist_ok=True)
    os.makedirs(BIN_DIR, exist_ok=True)


def copy_addons():
    shutil.copytree("addons", ADDONS_DIR, dirs_exist_ok=True)


def copy_third_party_licenses():
    os.makedirs(THIRD_PARTY_DIR, exist_ok=True)
    for lib, files in THIRD_PARTY_LICENSES.items():
        dst = os.path.join(THIRD_PARTY_DIR, lib)
        os.makedirs(dst, exist_ok=True)
        for f in files:
            src = os.path.join(COACD_SRC, f)
            shutil.copy2(src, os.path.join(dst, os.path.basename(f)))


def find_tbb_dir(prefix, runtime=None):
    arch_bits = {"arm32": 32, "arm64": 64, "x86_32": 32, "x86_64": 64}
    bits = arch_bits.get(env["arch"])
    if not bits:
        return None

    pattern = rf"^{prefix}_.*_cxx20_{bits}_{runtime + '_' if runtime else ''}release$"
    for d in os.scandir(COACD_BUILD):
        if d.is_dir() and re.match(pattern, d.name):
            return d.path
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
        "src",
        f"{COACD_BUILD}/_deps/spdlog-src/include",
    ],
    LIBPATH=[
        COACD_BUILD,
        f"{COACD_BUILD}/Release",
        f"{COACD_BUILD}/_deps/spdlog-build/Release",
        f"{COACD_BUILD}/_deps/openvdb-build/openvdb/openvdb/Release",
    ],
)

sources = list(Glob("src/*.cpp"))

PLATFORM_FLAGS = {
    "unix": dict(CXXFLAGS=["-fexceptions"], LIBS=["coacd", "openvdb", "tbb", "spdlog"]),
    "windows": dict(
        CXXFLAGS=["/EHsc"],
        CPPDEFINES=["SPDLOG_COMPILED_LIB"],
        LIBS=["coacd", "libopenvdb", "tbb12", "spdlog"],
    ),
    "web": dict(LIBS=["coacd"], CPPDEFINES=["SPDLOG_NO_EXCEPTIONS"]),
}

if env["platform"] in ("macos", "linux", "android"):
    env.Append(**PLATFORM_FLAGS["unix"])
elif env["platform"] == "windows":
    env.Append(**PLATFORM_FLAGS["windows"])
elif env["platform"] == "web":
    env.Append(**PLATFORM_FLAGS["web"])

# -------------------------------------------------------------------
# Build CoACD
# -------------------------------------------------------------------

build_coacd(
    env,
    COACD_BUILD,
    clean=GetOption("clean"),
    skip=ARGUMENTS.get("skip_coacd_build", "false") == "true",
)

# -------------------------------------------------------------------
# TBB lookup
# -------------------------------------------------------------------

if env["platform"] != "web":
    tbb_map = {
        ("macos", "darwin"): ("appleclang", None),
        ("linux", "linux"): ("gnu", None),
        ("windows", "win32"): ("msvc", "mt"),
        ("android", "linux"): ("clang", None),
        ("android", "win32"): ("clang", None),
    }

    prefix, runtime = tbb_map[(env["platform"], sys.platform)]
    tbb_dir = find_tbb_dir(prefix, runtime)
    if not tbb_dir:
        raise RuntimeError("TBB build not found")

    env.Append(LIBPATH=[tbb_dir])

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
