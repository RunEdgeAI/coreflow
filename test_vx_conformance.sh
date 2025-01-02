#! /bin/bash
set -e

TARGET_DIR="${GITHUB_WORKSPACE:-$PWD}"
echo "Running from TARGET_DIR=$TARGET_DIR"

mkdir -p $TARGET_DIR/cts/build

# Set the environment variables
export OPENVX_DIR=$TARGET_DIR
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$OPENVX_DIR/bazel-bin:$OPENVX_DIR/bazel-bin/vxu:$OPENVX_DIR/bazel-bin/targets/c_model:$OPENVX_DIR/bazel-bin/targets/extras:$OPENVX_DIR/bazel-bin/targets/debug:$OPENVX_DIR/bazel-bin/targets/opencl:$OPENVX_DIR/cts/build/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OPENVX_DIR/bazel-bin:$OPENVX_DIR/bazel-bin/vxu:$OPENVX_DIR/bazel-bin/targets/c_model:$OPENVX_DIR/bazel-bin/targets/extras:$OPENVX_DIR/bazel-bin/targets/debug:$OPENVX_DIR/bazel-bin/targets/opencl:$OPENVX_DIR/cts/build/lib/
export VX_TEST_DATA_PATH=$OPENVX_DIR/cts/test_data/
export VX_CL_SOURCE_DIR=$OPENVX_DIR/kernels/opencl/

# Set the correct library extension based on the OS
if [[ "$(uname)" == "Darwin" ]]; then
  EXT=".dylib"
else
  EXT=".so"
fi

# Build the conformance test suite
cd $TARGET_DIR/cts/build
cmake \
-DCMAKE_BUILD_TYPE=Debug \
-DOPENVX_INCLUDES=$OPENVX_DIR/include \
-DOPENVX_LIBRARIES="$OPENVX_DIR/bazel-bin/libopenvx$EXT"\;"$OPENVX_DIR/bazel-bin/vxu/libvxu$EXT"\;pthread\;dl\;m \
-DOPENVX_USE_USER_DATA_OBJECT=ON \
-DOPENVX_USE_IX=ON \
-DOPENVX_USE_U1=ON \
-DOPENVX_USE_NN=ON \
-DOPENVX_USE_NN_16=ON \
-DOPENVX_CONFORMANCE_NNEF_IMPORT=ON \
-DOPENVX_CONFORMANCE_NEURAL_NETWORKS=ON \
..

# -DOPENVX_CONFORMANCE_VISION=ON \
# -DOPENVX_USE_ENHANCED_VISION=ON \
# -DOPENVX_USE_PIPELINING=ON \
# -DOPENVX_USE_STREAMING=ON \

cmake --build .

# Run the conformance test suite
./bin/vx_test_conformance

# Clean up
cd $TARGET_DIR
rm -rf $TARGET_DIR/cts/build
