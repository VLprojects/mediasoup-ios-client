#!/usr/bin/env bash

set -e

PROJECT_PWD=${PWD}

# Import utils // OS, NUM_CORES
. scripts/common.sh

if [ "${OS}" != "Darwin" ] ; then
	echo "Only available for MacOS"
	exit 1;
fi

current_dir_name=${PROJECT_PWD##*/}
if [ "${current_dir_name}" != "mediasoup-client-native" ] ; then
	echo ">>> [ERROR] $(basename $0) must be called from mediasoup-client-native/ root directory" >&2
	exit 1
fi

# Run clang-format -i on 'include' and 'src' folders.
for dir in "include src test/*.test.* test/**/*.test.*"; do
    find ${dir} \
         \( -name '*.cpp' \
         -o -name '*.hpp' \) \
         -exec 'utils/clang/bin/clang-format' -i '{}' \;
    popd &>/dev/null
done
