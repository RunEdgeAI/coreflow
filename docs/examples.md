# 📚 CoreFlow Sample Applications
[TOC]
Welcome to the CoreFlow sample applications! This collection demonstrates the power and flexibility of our **graph-based execution runtime** with built-in AI extensions. Whether you're building computer vision pipelines, deploying machine learning models, or creating custom AI workflows, these samples showcase how CoreFlow can accelerate your development and deployment.

This section provides sample applications that showcase potential usage or may used as reference to develop your own products.

## 🌍 Hello World

Start your CoreFlow journey with our Hello World sample. This demonstrates graph loading and execution - perfect for understanding the basics.

```cpp
// Import graph
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
- Multiple graph support

**Perfect for**: Learning CoreFlow basics, prototyping, simple automation

### Run the hello world sample
```bash
./bin/examples/hello_world ../data/graph.xml
```

---

## 🤖 AI Chatbot Sample

Experience the power of AI integration with our interactive chatbot sample. This demonstrates how CoreFlow seamlessly integrates AI models into your applications with just a few lines of code.

**Key Features**:
- Interactive command-line interface
- Real-time AI responses
- Continuous operation

**Perfect for**: Customer service bots, interactive applications, AI-powered assistants

### Pre-requisites
* Model server -- Ollama used in this sample
    * [Download ollama:](https://ollama.com/download/linux)
        ```
        curl -fsSL https://ollama.com/install.sh | sh
        ```
    * Pull Llama3.2 3B:
        ```
        ollama pull llama3.2:3b
        ```
    * Serve model:
        ```
        ollama serve &
        ```
### Run the AI chatbot sample

```bash
./bin/examples/chatbot_sample

AI Chatbot - Type 'q' or 'quit' or 'exit' to stop
==========================================

You: hi
AI: How can I assist you today?

You: tell me about yourself
AI: I'm an artificial intelligence model known as Llama.
Llama stands for "Large Language Model Meta AI."

You: what is the preamble to the US constition ?
AI: The preamble to the United States Constitution is:

"We the People of the United States, in Order to form a more perfect Union, establish
Justice, insure domestic Tranquility, provide for the common defence, promote the general
Welfare, and secure the Blessings of Liberty to ourselves and our Posterity, do ordain
and establish this Constitution for the United States of America."
```

---

## 🔢 Digit Classification with ONNX-RT

Dive into machine learning with our interactive MNIST digit classification sample. This showcases ONNX Runtime integration and demonstrates how to build intelligent ML-powered systems.

**Key Features**:
- Interactive digit pattern generation
- Real-time classification results
- Confidence scoring
- Multiple pattern types (0-9)

**Perfect for**: lightweight ML enabled pipelines or workflows

### Run the digit classification using ONNX-RT sample

```bash
./bin/examples/ort_classification_sample

Interactive MNIST Digit Classification Using ONNX RT
===================================================

Enter single digit (0-9) (or type 'auto' to auto-generate digit, 'quit' to exit): auto
Auto-generating digit: 1

Digit Pattern (28x28):
=====================
............................
............................
............................
............................
.............##.............
.............##.............
.............##.............
.............##.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
..............#.............
............................
............................
............................
............................

Processing digit classification...

Classification Results:
======================
Digit 0: -3.0747
Digit 1: 8.6244 <-- PREDICTED
Digit 2: -3.4604
Digit 3: -2.2309
Digit 4: 2.1414
Digit 5: -1.1872
Digit 6: -1.5213
Digit 7: 0.1319
Digit 8: -2.3113
Digit 9: -3.9702

Predicted digit: 1 (confidence: 862.44%)

==================================================
```

---

## 🖼️ Image Processing Pipeline

Explore traditional computer vision with our image processing pipeline. This sample demonstrates how CoreFlow handles complex multi-stage image transformations efficiently.

**Key Features**:
- Color space conversion
- Gaussian blur processing
- Memory-efficient operations
- Pipeline optimization

**Perfect for**: Image preprocessing, computer vision pipelines, real-time video processing


### Run the image processing sample
```bash
./bin/examples/blur_pipeline
```

```cpp
// Create processing pipeline
auto rgb = Image::createImage(context, width, height, VX_DF_IMAGE_RGB);
auto yuv = Image::createImage(context, width, height, VX_DF_IMAGE_YUV4);
auto gray = Image::createImage(context, width, height, VX_DF_IMAGE_U8);
auto blur = Image::createImage(context, width, height, VX_DF_IMAGE_U8);
```

---

## 🔧 Custom Kernel Development

Learn how to extend CoreFlow with your own custom processing code on the fly. This sample shows the complete workflow from kernel creation to graph integration.

```cpp
    // Register the custom kernel
    auto add_kernel = Kernel::registerCustomKernel(
        context,
        "example.scalar_add",
        {
            // direction, type, state
            {VX_INPUT, VX_TYPE_INT32, VX_PARAMETER_STATE_REQUIRED},
            {VX_INPUT, VX_TYPE_INT32, VX_PARAMETER_STATE_REQUIRED},
            {VX_OUTPUT, VX_TYPE_INT32, VX_PARAMETER_STATE_REQUIRED}
        },
        // Custom kernel function via lambda
        [](vx_node node, const vx_reference parameters[], vx_uint32 num) -> vx_status {
            (void)node;
            (void)num;
            int32_t a = 0, b = 0, c = 0;
            auto scalar_a = (vx_scalar)parameters[0];
            auto scalar_b = (vx_scalar)parameters[1];
            auto scalar_out = (vx_scalar)parameters[2];

            scalar_a->readValue(&a);
            scalar_b->readValue(&b);
            // Add two scalar values
            c = a + b;
            scalar_out->writeValue(&c);

            return VX_SUCCESS;
        });
```

**Key Features**:
- Custom kernel implementation
- Graph integration
- Parameter handling

**Perfect for**: Custom algorithms, specialized processing, domain-specific solutions


### Run the custom code sample
```bash
./bin/examples/custom_code

Result: 7 + 5 = 12
```


<div class="section_buttons">

| Previous          |                              Next |
|:------------------|----------------------------------:|
| [Getting Started](install.md) | [OpenVX Integration](openvx.md) |

</div>