# OpenVX Integration
[TOC]
For teams that need compliance with the <a href="https://www.khronos.org/openvx/" target="_blank">Khronos OpenVX™</a> standard, CoreFlow provides a modern execution runtime with seamless integration into OpenVX -- providing an implementation of the OpenVX specification version 1.3.1, with various extensions and features enabled.

## Supported Extensions in CoreFlow
The following extensions and features are enabled in the CoreFlow project:

- **OPENVX_USE_USER_DATA_OBJECT**: Enables the user data object extension, allowing custom data objects to be used within the OpenVX framework.
- **OPENVX_USE_IX**: Enables the import/export extension, facilitating the import and export of data between different OpenVX contexts.
- **OPENVX_USE_XML**: Enables XML-based graph serialization and deserialization.
- **OPENVX_USE_S16**: Enables support for 16-bit signed integer data types.
- **OPENVX_USE_OPENCL_INTEROP**: Enables interoperability with OpenCL, allowing OpenVX to leverage OpenCL for acceleration.
- **OPENVX_USE_NN**: Enables the neural network extension, providing support for neural network operations within OpenVX.
- **OPENVX_USE_NN_16**: Enables half float (float16) support for the neural network extension.

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
The implementation in this project passes all OpenVX conformance tests to verify the implementation against the standard.

<div class="section_buttons">

| Previous          |                              Next |
|:------------------|----------------------------------:|
| [Examples](examples.md) | [OpenVX Examples](openvx-examples.md) |

</div>