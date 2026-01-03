from test_all import test_all

COMMANDS = [
    "scons platform=windows arch=x86_32",
    "scons platform=windows arch=x86_64",
    "scons platform=windows arch=arm64",

    "scons platform=android arch=x86_32",
    "scons platform=android arch=x86_64",
    "scons platform=android arch=arm32",
    "scons platform=android arch=arm64",

    "scons platform=web"
]

test_all(COMMANDS)