#!/usr/bin/env bash

# build config for dyn-reso-suppressor vst3.
# override any variable by exporting it before running the scripts.
# env vars always win.

# build

# where cmake drops its stuff (relative to repo root)
: "${BUILD_DIR:=build}"

# toolchain file for cross-compiling. leave empty for native linux.
: "${TOOLCHAIN_FILE:=mingw-toolchain.cmake}"

# debug or release
: "${BUILD_TYPE:=Release}"

# parallel jobs — defaults to all logical cpus
: "${JOBS:=$(nproc)}"

# install

# where the finished .vst3 bundle gets copied.
# common paths:
#   wine:         "$HOME/.wine/drive_c/Program Files/Common Files/VST3"
#   linux native: "$HOME/.vst3"
#   custom prefix: "$WINEPREFIX/drive_c/Program Files/Common Files/VST3"
: "${VST3_INSTALL_DIR:=$HOME/.wine/drive_c/Program Files/Common Files/VST3}"

# must match the target name in CMakeLists.txt
: "${PLUGIN_NAME:=DynResoSuppressor}"

# must match the PRODUCT_NAME in CMakeLists.txt
: "${PRODUCT_NAME:=dyn-reso-suppressor}"
