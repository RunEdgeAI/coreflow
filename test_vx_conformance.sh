#! /bin/bash

mkdir -p cts/build

# Set the environment variables
export OPENVX_DIR=$PWD
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$OPENVX_DIR/bazel-bin:$OPENVX_DIR/bazel-bin/vxu:$OPENVX_DIR/bazel-bin/targets/c_model:$OPENVX_DIR/bazel-bin/targets/extras:$OPENVX_DIR/bazel-bin/targets/debug:$OPENVX_DIR/bazel-bin/targets/opencl:$OPENVX_DIR/cts/build/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OPENVX_DIR/bazel-bin:$OPENVX_DIR/bazel-bin/vxu:$OPENVX_DIR/bazel-bin/targets/c_model:$OPENVX_DIR/bazel-bin/targets/extras:$OPENVX_DIR/bazel-bin/targets/debug:$OPENVX_DIR/bazel-bin/targets/opencl:$OPENVX_DIR/cts/build/lib/
export VX_TEST_DATA_PATH=$HOME/Projects/OpenVX-sample-impl/cts/test_data/
export VX_CL_SOURCE_DIR=$OPENVX_DIR/kernels/opencl/

# Build the conformance test suite
cd cts/build
cmake \
-DCMAKE_BUILD_TYPE=Debug \
-DOPENVX_INCLUDES=$OPENVX_DIR/include \
-DOPENVX_LIBRARIES=$OPENVX_DIR/bazel-bin/libopenvx.dylib\;$OPENVX_DIR/bazel-bin/vxu/libvxu.dylib\;pthread\;dl\;m \
-DOPENVX_USE_USER_DATA_OBJECT=ON \
-DOPENVX_USE_IX=ON \
-DOPENVX_USE_U1=ON \
-DOPENVX_USE_NN=ON \
-DOPENVX_CONFORMANCE_NNEF_IMPORT=ON \
-DOPENVX_CONFORMANCE_NEURAL_NETWORKS=ON \
..

# -DOPENVX_USE_NN_16=ON \
# -DOPENVX_CONFORMANCE_VISION=ON \
# -DOPENVX_USE_ENHANCED_VISION=ON \
# -DOPENVX_USE_PIPELINING=ON \
# -DOPENVX_USE_STREAMING=ON \

cmake --build .

# Run the conformance test suite
./bin/vx_test_conformance

# Clean up
cd ../../
rm -rf cts/build
