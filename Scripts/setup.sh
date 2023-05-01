# Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
#
# SPDX-License-Identifier: MIT

# Amended from https://github.com/linusg/libjs-test262/blob/main/setup.sh

#
# setup.sh
# Generates build files via cmake, using Ninja for building
#

BUILD_DIRECTORY="Build"

## Clean-up
# rm -rf "${BUILD_DIRECTORY}"

# Create the `Build` directory
mkdir "${BUILD_DIRECTORY}"
cd "${BUILD_DIRECTORY}"

# Generate cmake files
cmake -GNinja .. -DSERENITY_SOURCE_DIR="${SERENITY_SOURCE_DIR}"

# Change back to the source directory
cd ..
