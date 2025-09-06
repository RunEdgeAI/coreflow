# 🛠️ Getting Started
[TOC]

## Prerequisites

Before you begin, ensure you have the following installed:

- A C++20 compatible compiler (GCC 8+, Clang 7+, or MSVC 2019+)
- CoreFlow runtime libraries

## Steps

1. Procure core-sdk release:
   ```bash
   core_sdk_vX.X.X.tar.gz
   ```

2. Extract core-sdk release:
   ```bash
   tar -xf core_sdk_vX.X.X.tar.gz
   ```


## Project Structure
The release tarball

- `include/`     : Public headers
    - `VX/`      : OpenVX headers
    - `COREFLOW/`: CoreFlow headers
- `lib/`         : Public libraries for SDK
- `bin/examples` : Example applications

## Building

Customers will be provided pre-compiled CoreFlow library as well as sample binaries.

## Running Your First Example
The simplest way to start is with the `hello_world` example, which imports and executes any graph that was exported using **Edge Studio** - the no-code graph builder:

```bash
# Hello World (requires XML file)
./bin/examples/corevx_cpp/hello_world graph.xml
```

You can likewise run any of the examples in the bin directory with:
```bash
# AI Chatbot
./bin/examples/corevx_cpp/chatbot_sample

# MNIST Classification
./bin/examples/corevx_cpp/ort_classification_sample

# Image Processing
./bin/examples/corevx_cpp/blur_pipeline

# Custom Kernel
./bin/examples/corevx_cpp/custom_code
```

<div class="section_buttons">

| Previous          |                              Next |
|:------------------|----------------------------------:|
| [Home](../README.md) | [Examples](examples.md) |

</div>