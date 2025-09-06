# CoreFlow: A Modern Orchestration Framework
[TOC]

## 🚀 Why CoreFlow?
<p align="left">
  <img width="35%" src="https://img.shields.io/badge/Graph%20Execution%20Runtime-blue?style=for-the-badge&logo=graphql" />
</p>

![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)
![macOS](https://img.shields.io/badge/macOS-000000?logo=apple&logoColor=F0F0F0)
![Windows](https://custom-icon-badges.demolab.com/badge/Windows-0078D6?logo=windows11&logoColor=white)
![](https://img.shields.io/badge/OpenVX-1.3.1-blue)
[TOC]

**CoreFlow** is a modern, high-performance graph execution runtime designed for AI, ML, and computer vision applications. Our platform combines:

- **🔗 No-code Graph Builder**: Visual, intuitive UI to build pipelines with automatic optimization
- **🤖 Cross-platform Support**: Seamless deployment across ARM or x86 with unified APIs.
- **⚡ High Performance**: Optimized determinstic execution for real-time processing across CPU, GPU, and specialized hardware
- **🛠️ Developer Friendly**: Simple C++ API with comprehensive error handling
- **🔧 Extensible**: Easy to add custom kernels and processing behaviors

## 🔧 Architecture

### Graph-Based Execution
CoreFlow uses a graph-based execution model where:
- **Graphs** represent execution modeled as DAG (Directed Acyclic Graph)
- **Nodes** represent processing operations
- **Edges** define data flow between operations
- **Kernels** define a runnable operation or algorithm
- **Targets** define a collection of suppported operations

### Built-in AI/ML Support
- **ONNX Runtime**: Deploy models using Open Neural Network Exchange
- **TensorFlow Lite**: Leverage Google's on-device AI framework
- **Executorch**: Leverage Meta's on-device AI framework
- **Model Management**: Easy model loading and switching

### Performance Features
- **Memory Management**: Automatic memory optimization
- **Hardware Acceleration**: CPU, GPU, and specialized hardware support
- **Streaming and Pipelining Support**: Automatic graph runtime optimization
- **Error Handling**: Comprehensive error reporting and recovery

## License
This project is of confidental and contains proprietary material. See the [LICENSE](LICENSE.md) file for more details.

## 🆘 Support

- **Documentation**: [runedge.ai/docs/orchestration](https://www.runedge.ai/docs/orchestration)
- **Email**: support@corevx.com

---

<p align="center">
  <strong>Ready to build the future with CoreFlow?</strong><br>
  Start with our samples and discover the power of CoreFlow's execution model.
</p>

<div class="section_buttons">

| Previous          |                              Next |
|:------------------|----------------------------------:|
| | [Getting Started](docs/install.md) |

</div>