# CoreVX Project
This project is an implementation of the OpenVX specification version 1.3.1, with various extensions and features enabled. 
The project is built using Google's Bazel build system and is validated by unit tests, integration tests, and the OpenVX conformance tests.

## Project Structure
```
.bazelrc
.bazelversion
.gitignore
BUILD
build_tools/
    platforms/
docs/
    Doxyfile
framework/
    include/
    src/
include/
    VX/
    vx_ix_format.txt
    windows/
kernels/
    c_model/
    debug/
    extras/
    NNEF-Tools/
    opencl/
    README
    tiling/
    utils/
    venum/
MODULE.bazel
MODULE.bazel.lock
targets/
    c_model/
    debug/
    extras/
    opencl/
    tiling/
    venum/
tests/
vxu/
WORKSPACE
```

## Enabled Extensions in CoreVX
The following extensions and features are enabled in the CoreVX project, as defined in the `vx_corevx_ext.h` file:

- **OPENVX_USE_USER_DATA_OBJECT**: Enables the user data object extension, allowing custom data objects to be used within the OpenVX framework.
- **OPENVX_USE_IX**: Enables the import/export extension, facilitating the import and export of data between different OpenVX contexts.
- **OPENVX_USE_XML**: Enables XML-based graph serialization and deserialization.
- **OPENVX_USE_S16**: Enables support for 16-bit signed integer data types.
- **OPENVX_USE_OPENCL_INTEROP**: Enables interoperability with OpenCL, allowing OpenVX to leverage OpenCL for acceleration.
- **OPENVX_USE_NN**: Enables the neural network extension, providing support for neural network operations within OpenVX.

### Conditional Extensions for ARM Architectures
The following extensions are conditionally enabled for ARM and ARM64 architectures:

- **OPENVX_USE_TILING**: Enables the tiling extension, which allows for tiled processing of images.
- **OPENVX_KHR_TILING**: Enables Khronos tiling extension for efficient image processing.
- **EXPERIMENTAL_USE_VENUM**: Enables experimental support for VENUM, a vector processing unit.

### Conformance and Experimental Features

- **OPENVX_CONFORMANCE_VISION**: Enables conformance for vision functions per the OpenVX specification.
- **OPENVX_USE_ENHANCED_VISION**: Enables conforamnce enhanced vision functions per the OpenVX specfication.
- **OPENVX_CONFORMANCE_NNEF_IMPORT**: Enables conformance for NNEF (Neural Network Exchange Format) import, ensuring compatibility with NNEF models.
- **OPENVX_CONFORMANCE_NEURAL_NETWORKS**: Enables conformance for neural networks, ensuring that neural network operations meet the OpenVX standard.
- **EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT**: Enables experimental support for 16-bit floating-point data types.
- **EXPERIMENTAL_USE_DOT**: Enables experimental support for DOT (Graphviz) output for graph visualization.
- **EXPERIMENTAL_USE_OPENCL**: Enables experimental support for OpenCL, allowing for further exploration and development of OpenCL-based acceleration within OpenVX.

## Conformance Tests
The project includes conformance tests to verify the implementation against the OpenVX standard. The test data path is set using the VX_TEST_DATA_PATH environment variable.

## License
This project is of confidental and propreitary material. See the LICENSE file for more details.
