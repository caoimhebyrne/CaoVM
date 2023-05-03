# Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
#
# SPDX-License-Identifier: MIT

#
# build-and-run.sh
# Builds and runs the project via CMake
#

BUILD_DIRECTORY="Build"

# Change into the build directory and build the project
cd "${BUILD_DIRECTORY}"
cmake --build .

cd ..

# Execute the outputted binary
eval "${BUILD_DIRECTORY}/jvm $@"
