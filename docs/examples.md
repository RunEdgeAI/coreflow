# CoreFlow Sample Applications
[TOC]
Welcome to the CoreFlow sample applications! This collection demonstrates the power and flexibility of our **graph-based execution runtime** with built-in AI extensions. Whether you're building computer vision pipelines, deploying machine learning models, or creating custom AI workflows, these samples showcase how CoreFlow can accelerate your development and deployment.

## 📚 Sample Applications
This section provides sample applications that showcase potential usage or may used as reference to develop your own products.

### 🌍 Hello World
**File**: `hello_world.cpp`

Start your CoreFlow journey with our Hello World sample. This demonstrates graph loading and execution - perfect for understanding the basics.

```cpp
// Import graph from XML
auto graphs = xml::Import::importFromXML(context, argv[1]);

// Process graph
for (auto graph : graphs) {
    if (graph->process() == VX_SUCCESS) {
        std::cout << "Graph processed successfully" << std::endl;
    }
}
```

**Key Features**:
- XML graph definition
- Simple execution model
- Error handling
- Multiple graph support

**Perfect for**: Learning CoreVX basics, prototyping, simple automation

---

### 🤖 AI Chatbot Sample
**File**: `chatbot_sample.cpp`

Experience the power of AI integration with our interactive chatbot sample. This demonstrates how CoreVX seamlessly integrates AI models into your applications with just a few lines of code.

```cpp
// Create context and graph
auto context = Context::createContext();
auto graph = Graph::createGraph(context);

// Get AI chatbot kernel
auto kernel = Kernel::getKernelByEnum(context, VX_KERNEL_AIS_CHATBOT);
```

**Key Features**:
- Interactive command-line interface
- Real-time AI responses
- Built-in error handling
- Continuous operation mode

**Perfect for**: Customer service bots, interactive applications, AI-powered assistants

---

### 🔢 Digit Classification with ONNX-RT
**File**: `ort_classification_sample.cpp`

Dive into machine learning with our interactive MNIST digit classification sample. This showcases ONNX Runtime integration and demonstrates how to build intelligent image recognition systems.

```cpp
// Create ONNX Runtime inference node
auto kernel = Kernel::getKernelByEnum(context, VX_KERNEL_ORT_CPU_INF);
auto node = Node::createNode(graph, kernel, {model_path_array, input_tensors, output_tensors});
```

**Key Features**:
- Interactive digit pattern generation
- Real-time classification results
- Confidence scoring
- Multiple pattern types (0-9)

**Perfect for**: Document processing, handwritten text recognition, educational AI demos

---

### 🖼️ Image Processing Pipeline
**File**: `blur_pipeline.cpp`

Explore traditional computer vision with our image processing pipeline. This sample demonstrates how CoreFlow handles complex multi-stage image transformations efficiently.

```cpp
// Create processing pipeline
auto rgb = Image::createImage(context, width, height, VX_DF_IMAGE_RGB);
auto yuv = Image::createImage(context, width, height, VX_DF_IMAGE_YUV4);
auto gray = Image::createImage(context, width, height, VX_DF_IMAGE_U8);
auto blur = Image::createImage(context, width, height, VX_DF_IMAGE_U8);
```

**Key Features**:
- Color space conversion
- Gaussian blur processing
- Memory-efficient operations
- Pipeline optimization

**Perfect for**: Image preprocessing, computer vision pipelines, real-time video processing

---

### 🔧 Custom Kernel Development
**File**: `custom_code.cpp`

Learn how to extend CoreVX with your own custom processing kernels. This sample shows the complete workflow from kernel creation to graph integration.

```cpp
// Create custom kernel
auto kernel = Kernel::createKernel(context, "com.example.custom_kernel",
                                  custom_kernel_function, 1, 1, 0, 0);

// Add to graph
auto node = Node::createNode(graph, kernel, {input_array, output_array});
```

**Key Features**:
- Custom kernel implementation
- Graph integration
- Parameter handling
- Error management

**Perfect for**: Custom algorithms, specialized processing, domain-specific solutions

<div class="section_buttons">

| Previous          |                              Next |
|:------------------|----------------------------------:|
| [Getting Started](install.md) | [OpenVX Integration](openvx.md) |

</div>