import os
import shutil
from coacd_build import build as build_coacd
from coacd_build import setup_linking as setup_coacd_linking
from coacd_build import COACD_BUILD
from third_party_licenses import copy_third_party_licenses

# -------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------

unpackaged = ARGUMENTS.get("unpackaged", "false") == "true"

LICENSE_SRC = "LICENSE.txt"
BUILD_DIR = "build"
ADDONS_DIR = os.path.join(BUILD_DIR, "addons")
PLUGIN_DIR = os.path.join(ADDONS_DIR, "godotcoacd")
BIN_DIR = BUILD_DIR if unpackaged else os.path.join(PLUGIN_DIR, "bin")

# -------------------------------------------------------------------
# Helper Functions
# -------------------------------------------------------------------

def get_compiler(env):
    """Detect compiler type from environment."""
    compiler = os.path.basename(env.get("CC", ""))
    if env.get("is_msvc", False):
        return "msvc"
    if compiler == "emcc":
        return "mingw"
    return compiler

def copy_addons():
    shutil.copytree("addons", ADDONS_DIR, dirs_exist_ok=True)

# -------------------------------------------------------------------
# Setup Build Environment
# -------------------------------------------------------------------

os.makedirs(BIN_DIR, exist_ok=True)
if not unpackaged:
    copy_addons()

env = SConscript("godot-cpp/SConstruct")
compiler = get_compiler(env)

# -------------------------------------------------------------------
# Build Configuration
# -------------------------------------------------------------------

env.Append(
    CPPPATH=[
        "src",
        f"{COACD_BUILD}/_deps/spdlog-src/include",
    ]
)

sources = list(Glob("src/*.cpp"))

COMPILER_FLAGS = {
    "msvc": dict(
        CXXFLAGS=["/EHsc"],
        CPPDEFINES=["SPDLOG_COMPILED_LIB"],
    ),
}

PLATFORM_FLAGS = {
    "web": dict(CPPDEFINES=["SPDLOG_NO_EXCEPTIONS"]),
}

env.Append(**COMPILER_FLAGS.get(compiler, {}))
env.Append(**PLATFORM_FLAGS.get(env["platform"], {}))

# -------------------------------------------------------------------
# Build CoACD
# -------------------------------------------------------------------

if not (GetOption("clean") or ARGUMENTS.get("skip_coacd_build", "false") == "true"):
    build_coacd(env, compiler)

setup_coacd_linking(env, compiler)

# -------------------------------------------------------------------
# Documentation & licenses
# -------------------------------------------------------------------

if not unpackaged:
    shutil.copy(LICENSE_SRC, PLUGIN_DIR)
    copy_third_party_licenses(PLUGIN_DIR)

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