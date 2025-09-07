/**
 * @example ort_classification_sample.cpp
 * @brief Interactive MNIST Digit Classification Sample using ONNX RT
 * @version 0.1
 * @date 2025-09-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <cstdlib>
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <string>
#include <sstream>

#include <COREFLOW/all.hpp>

using namespace coreflow;

/**
 * @brief Print the digit pattern to the console
 *
 * @param data The digit pattern
 */
void printDigitPattern(const std::vector<float>& data)
{
    std::cout << "\nDigit Pattern (28x28):" << std::endl;
    std::cout << "=====================" << std::endl;
    for (int i = 0; i < 28; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            char pixel = data[i * 28 + j] > 0.5f ? '#' : '.';
            std::cout << pixel;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/**
 * @brief Create a digit pattern from user input
 *
 * @param input The user input
 * @return The digit pattern
 */
std::vector<float> createDigitFromInput(std::string& input)
{
    std::vector<float> data(28 * 28, 0.0f);
    int digit = 0;

    // Generate a random digit pattern
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> digit_gen(0, 9);
    std::uniform_real_distribution<float> noise_gen(0.0f, 0.3f);

    if (input == "auto")
    {
        digit = digit_gen(gen);
        std::cout << "Auto-generating digit: " << digit << std::endl;
    }
    else if (input.length() == 1 && std::isdigit(input[0]))
    {
        digit = input[0] - '0';
        std::cout << "Creating simple pattern for digit: " << digit << std::endl;
    }
    else
    {
        std::cout << "Invalid input: " << input << std::endl;
        return data;
    }

    // Create simple patterns for each digit
    switch (digit)
    {
        case 0: // Zero - oval shape
            for (int i = 6; i < 22; i++)
            {
                for (int j = 6; j < 22; j++)
                {
                    int di = i - 14, dj = j - 14;
                    // Create an oval shape
                    float oval = (di*di)/36.0f + (dj*dj)/64.0f;
                    if (oval >= 0.8f && oval <= 1.2f)
                        data[i * 28 + j] = 1.0f;
                }
            }
            break;
        case 1: // Vertical line
            for (int i = 4; i < 24; i++)
            {
                data[i * 28 + 14] = 1.0f;
                if (i < 8) data[i * 28 + 13] = 1.0f;
            }
            break;
        case 2: // Two - very curved and flowing
            // Top horizontal line with slight curve
            for (int j = 6; j < 22; j++)
            {
                data[6 * 28 + j] = 1.0f;
                data[7 * 28 + j] = 1.0f;
                // Add slight curve
                if (j > 8 && j < 20) data[5 * 28 + j] = 1.0f;
            }
            // Top right curve - very rounded
            for (int i = 6; i < 12; i++)
            {
                data[i * 28 + 20] = 1.0f;
                data[i * 28 + 21] = 1.0f;
                // Add curve pixels
                if (i > 7) data[i * 28 + 19] = 1.0f;
                if (i > 8) data[i * 28 + 18] = 1.0f;
                if (i > 9) data[i * 28 + 17] = 1.0f;
            }
            // Middle horizontal with curve
            for (int j = 6; j < 22; j++)
            {
                data[12 * 28 + j] = 1.0f;
                data[13 * 28 + j] = 1.0f;
                // Add slight curve
                if (j > 8 && j < 20) data[11 * 28 + j] = 1.0f;
            }
            // Bottom left curve - very rounded
            for (int i = 13; i < 20; i++)
            {
                data[i * 28 + 6] = 1.0f;
                data[i * 28 + 7] = 1.0f;
                // Add curve pixels
                if (i < 18) data[i * 28 + 8] = 1.0f;
                if (i < 17) data[i * 28 + 9] = 1.0f;
                if (i < 16) data[i * 28 + 10] = 1.0f;
            }
            // Bottom horizontal with curve
            for (int j = 6; j < 22; j++)
            {
                data[20 * 28 + j] = 1.0f;
                data[21 * 28 + j] = 1.0f;
                // Add slight curve
                if (j > 8 && j < 20) data[22 * 28 + j] = 1.0f;
            }
            break;
        case 3: // Three - more recognizable shape
            // Top horizontal line
            for (int j = 6; j < 22; j++)
            {
                data[6 * 28 + j] = 1.0f;
                data[7 * 28 + j] = 1.0f;
            }
            // Top right vertical
            for (int i = 6; i < 12; i++)
            {
                data[i * 28 + 20] = 1.0f;
                data[i * 28 + 21] = 1.0f;
            }
            // Middle horizontal
            for (int j = 6; j < 22; j++)
            {
                data[12 * 28 + j] = 1.0f;
                data[13 * 28 + j] = 1.0f;
            }
            // Bottom right vertical
            for (int i = 13; i < 20; i++)
            {
                data[i * 28 + 20] = 1.0f;
                data[i * 28 + 21] = 1.0f;
            }
            // Bottom horizontal
            for (int j = 6; j < 22; j++)
            {
                data[20 * 28 + j] = 1.0f;
                data[21 * 28 + j] = 1.0f;
            }
            break;
        case 4: // Four
            for (int i = 4; i < 24; i++)
            {
                if (i < 14)
                {
                    data[i * 28 + 4] = 1.0f;
                    data[i * 28 + 20] = 1.0f;
                }
                else if (i < 18)
                {
                    for (int j = 4; j < 24; j++)
                        data[i * 28 + j] = 1.0f;
                }
                else
                {
                    data[i * 28 + 20] = 1.0f;
                }
            }
            break;
        case 5: // Five
            for (int i = 4; i < 24; i++)
            {
                if (i < 8 || (i > 10 && i < 14) || i > 20)
                {
                    for (int j = 4; j < 24; j++)
                        data[i * 28 + j] = 1.0f;
                }
                else if (i < 14)
                {
                    data[i * 28 + 4] = 1.0f;
                }
                else
                {
                    data[i * 28 + 20] = 1.0f;
                }
            }
            break;
        case 6: // Six
            for (int i = 4; i < 24; i++)
            {
                if (i < 8 || (i > 10 && i < 14) || i > 20)
                {
                    for (int j = 4; j < 24; j++)
                        data[i * 28 + j] = 1.0f;
                }
                else if (i < 14)
                {
                    data[i * 28 + 4] = 1.0f;
                }
                else
                {
                    data[i * 28 + 4] = 1.0f;
                    data[i * 28 + 20] = 1.0f;
                }
            }
            break;
        case 7: // Seven
            for (int i = 4; i < 24; i++)
            {
                if (i < 8)
                {
                    for (int j = 4; j < 24; j++)
                        data[i * 28 + j] = 1.0f;
                }
                else
                {
                    data[i * 28 + 20] = 1.0f;
                }
            }
            break;
        case 8: // Eight - clear two distinct loops
            // Top loop - smaller and more compact
            for (int i = 6; i < 12; i++)
            {
                for (int j = 8; j < 20; j++)
                {
                    int di = i - 9, dj = j - 14;
                    float circle = (di*di)/9.0f + (dj*dj)/36.0f;
                    if (circle >= 0.8f && circle <= 1.2f)
                        data[i * 28 + j] = 1.0f;
                }
            }
            // Bottom loop - larger and more prominent
            for (int i = 14; i < 22; i++)
            {
                for (int j = 6; j < 22; j++)
                {
                    int di = i - 18, dj = j - 14;
                    float circle = (di*di)/16.0f + (dj*dj)/64.0f;
                    if (circle >= 0.7f && circle <= 1.3f)
                        data[i * 28 + j] = 1.0f;
                }
            }
            // Connect the loops with vertical lines
            for (int i = 10; i < 16; i++)
            {
                data[i * 28 + 6] = 1.0f;
                data[i * 28 + 7] = 1.0f;
                data[i * 28 + 20] = 1.0f;
                data[i * 28 + 21] = 1.0f;
            }
            break;
                case 9: // Nine - completely redesigned to avoid confusion with 0
            // Top loop - large and prominent
            for (int i = 6; i < 14; i++)
            {
                for (int j = 6; j < 22; j++)
                {
                    int di = i - 10, dj = j - 14;
                    float circle = (di*di)/16.0f + (dj*dj)/64.0f;
                    if (circle >= 0.7f && circle <= 1.3f)
                        data[i * 28 + j] = 1.0f;
                }
            }
            // Bottom right curve - much smaller and distinct
            for (int i = 16; i < 22; i++)
            {
                for (int j = 10; j < 20; j++)
                {
                    int di = i - 19, dj = j - 15;
                    float circle = (di*di)/4.0f + (dj*dj)/25.0f;
                    if (circle >= 0.8f && circle <= 1.2f)
                        data[i * 28 + j] = 1.0f;
                }
            }
            // Connect with vertical lines on the right side only
            for (int i = 10; i < 18; i++)
            {
                data[i * 28 + 20] = 1.0f;
                data[i * 28 + 21] = 1.0f;
            }
            // Add a small vertical line on the left for the top loop
            for (int i = 6; i < 12; i++)
            {
                data[i * 28 + 6] = 1.0f;
                data[i * 28 + 7] = 1.0f;
            }
            break;
    }

    // Add some noise to make it more realistic
    for (int i = 0; i < 28 * 28; i++)
    {
        data[i] += noise_gen(gen);
        data[i] = std::min(1.0f, std::max(0.0f, data[i]));
    }

    return data;
}

/**
 * @brief Main function
 *
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int main()
{
    std::cout << "Interactive MNIST Digit Classification Using ONNX RT" << std::endl;
    std::cout << "===================================================" << std::endl;

    // Create context
    auto context = Context::createContext();
    if (Error::getStatus(context) != VX_SUCCESS)
    {
        std::cerr << "Failed to create Context" << std::endl;
        return EXIT_FAILURE;
    }

    // Create graph
    auto graph = Graph::createGraph(context);
    if (Error::getStatus(graph) != VX_SUCCESS)
    {
        std::cerr << "Failed to create Graph" << std::endl;
        return EXIT_FAILURE;
    }

    // Model path (you would need to provide an actual MNIST ONNX model)
    std::string model_path = "/Users/Andrew/Downloads/mnist.onnx";

    // Create model path array
    auto model_path_array = Array::createArray(context, VX_TYPE_CHAR, model_path.length() + 1);
    if (Error::getStatus(model_path_array) != VX_SUCCESS)
    {
        std::cerr << "Failed to create model path array" << std::endl;
        return EXIT_FAILURE;
    }

    // Add model path to array
    if (model_path_array->addItems(model_path.length() + 1, model_path.c_str(), sizeof(char)) != VX_SUCCESS)
    {
        std::cerr << "Failed to add model path to array" << std::endl;
        return EXIT_FAILURE;
    }

    // Create input tensor (28x28 grayscale image = 784 values)
    size_t input_dims[] = {1, 1, 28, 28}; // Batch=1, Channels=1, Height=28, Width=28
    auto input_tensor = Tensor::createTensor(context, 4, input_dims, VX_TYPE_FLOAT32, 0);
    if (Error::getStatus(input_tensor) != VX_SUCCESS)
    {
        std::cerr << "Failed to create input tensor" << std::endl;
        return EXIT_FAILURE;
    }

    // Create output tensor (10 class probabilities)
    size_t output_dims[] = {1, 10}; // Batch=1, Classes=10
    auto output_tensor = Tensor::createTensor(context, 2, output_dims, VX_TYPE_FLOAT32, 0);
    if (Error::getStatus(output_tensor) != VX_SUCCESS)
    {
        std::cerr << "Failed to create output tensor" << std::endl;
        return EXIT_FAILURE;
    }

    // Create object arrays for inputs and outputs
    auto input_tensors = ObjectArray::createObjectArray(context, VX_TYPE_TENSOR);
    auto output_tensors = ObjectArray::createObjectArray(context, VX_TYPE_TENSOR);
    if (Error::getStatus(input_tensors) != VX_SUCCESS || Error::getStatus(output_tensors) != VX_SUCCESS)
    {
        std::cerr << "Failed to create object arrays" << std::endl;
        return EXIT_FAILURE;
    }

    // Set object array items
    if (input_tensors->setItem(0, input_tensor) != VX_SUCCESS)
    {
        std::cerr << "Failed to set input tensor in array" << std::endl;
        return EXIT_FAILURE;
    }
    if (output_tensors->setItem(0, output_tensor) != VX_SUCCESS)
    {
        std::cerr << "Failed to set output tensor in array" << std::endl;
        return EXIT_FAILURE;
    }

    // Get ONNX runtime kernel
    auto kernel = Kernel::getKernelByEnum(context, VX_KERNEL_ORT_CPU_INF);
    if (Error::getStatus(kernel) != VX_SUCCESS)
    {
        std::cerr << "Failed to get ONNX runtime kernel. Make sure ONNX RT target is loaded." << std::endl;
        return EXIT_FAILURE;
    }

    // Create node
    auto node = Node::createNode(graph, kernel, {model_path_array, input_tensors, output_tensors});
    if (Error::getStatus(node) != VX_SUCCESS)
    {
        std::cerr << "Failed to create ONNX node" << std::endl;
        return EXIT_FAILURE;
    }

    // Get tensor strides once
    size_t view_start[4] = {0, 0, 0, 0};
    size_t output_view_start[2] = {0, 0};
    const size_t* input_strides = input_tensor->strides();
    const size_t* output_strides = output_tensor->strides();

    // Interactive loop
    while (true)
    {
        std::string input;
        std::cout << "\nEnter single digit (0-9) (or type 'auto' to auto-generate digit, 'quit' to exit): ";
        std::getline(std::cin, input);

        // Check if user provided any input
        if (input.empty())
        {
            continue;
        }

        // Check if user wants to exit
        if (input == "quit" || input == "q" || input == "exit")
        {
            std::cout << "Goodbye!" << std::endl;
            return EXIT_SUCCESS;
        }

        // Get user input for digit pattern
        std::vector<float> input_data = createDigitFromInput(input);

        // Display the pattern
        printDigitPattern(input_data);

        // Fill input tensor with user data
        if (input_tensor->copyPatch(4, view_start, input_dims, input_strides,
                                         input_data.data(), VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST) != VX_SUCCESS)
        {
            std::cerr << "Failed to copy input data to tensor" << std::endl;
            continue;
        }

        std::cout << "Processing digit classification..." << std::endl;

        // Process graph
        if (graph->process() != VX_SUCCESS)
        {
            std::cerr << "Failed to process graph" << std::endl;
            continue;
        }

        // Read output probabilities
        std::vector<float> output_data(10);
        if (output_tensor->copyPatch(2, output_view_start, output_dims, output_strides,
                                          output_data.data(), VX_READ_ONLY, VX_MEMORY_TYPE_HOST) != VX_SUCCESS)
        {
            std::cerr << "Failed to copy output data from tensor" << std::endl;
            continue;
        }

        // Find the predicted digit (highest probability)
        int predicted_digit = 0;
        float max_prob = output_data[0];
        for (int i = 1; i < 10; i++)
        {
            if (output_data[i] > max_prob)
            {
                max_prob = output_data[i];
                predicted_digit = i;
            }
        }

        // Display results
        std::cout << "\nClassification Results:" << std::endl;
        std::cout << "======================" << std::endl;
        for (int i = 0; i < 10; i++)
        {
            std::cout << "Digit " << i << ": " << std::fixed << std::setprecision(4)
                      << output_data[i] << (i == predicted_digit ? " <-- PREDICTED" : "") << std::endl;
        }

        std::cout << "\nPredicted digit: " << predicted_digit << " (confidence: "
                  << std::fixed << std::setprecision(2) << (max_prob * 100) << "%)" << std::endl;

        std::cout << "\n" << std::string(50, '=') << std::endl;
    }

    return EXIT_SUCCESS;
}
