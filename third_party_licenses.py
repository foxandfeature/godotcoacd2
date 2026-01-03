# third_party_licenses.py

import os
import shutil
from coacd_build import COACD_SRC

# Destination directory for third-party licenses
THIRD_PARTY_DIR_NAME = "third-party-licenses"

# Mapping of libraries to their license files
THIRD_PARTY_LICENSES = {
    "boost": ["build/_deps/boost-src/LICENSE_1_0.txt"],
    "openvdb": ["build/_deps/openvdb-src/LICENSE"],
    "spdlog": ["build/_deps/spdlog-src/LICENSE"],
    "tbb": [
        "build/_deps/tbb-src/LICENSE.txt",
        "build/_deps/tbb-src/third-party-programs.txt",
    ],
}


def copy_third_party_licenses(plugin_dir):
    """
    Copies third-party license files into the plugin directory.
    """
    third_party_dir = os.path.join(plugin_dir, THIRD_PARTY_DIR_NAME)
    os.makedirs(third_party_dir, exist_ok=True)

    for lib, files in THIRD_PARTY_LICENSES.items():
        lib_dir = os.path.join(third_party_dir, lib)
        os.makedirs(lib_dir, exist_ok=True)
        for f in files:
            src = os.path.join(COACD_SRC, f)
            dst = os.path.join(lib_dir, os.path.basename(f))
            shutil.copy2(src, dst)