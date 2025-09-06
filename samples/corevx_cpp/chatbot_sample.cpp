/**
 * @example chatbot_sample.cpp
 * @brief Interactove Sample with AI Chatbot
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <cstdlib>
#include <iostream>
#include <string>

#include <COREVX/all.hpp>

using namespace corevx;

int main()
{
    std::cout << "AI Chatbot - Type 'q' or 'quit' or 'exit' to stop" << std::endl;
    std::cout << "==========================================" << std::endl;

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

    // Create input string array
    auto input_string = Array::createArray(context, VX_TYPE_CHAR, VX_MAX_STRING_LENGTH);
    if (Error::getStatus(input_string) != VX_SUCCESS)
    {
        std::cerr << "Failed to create input string array" << std::endl;
        return EXIT_FAILURE;
    }

    // Create output string array
    auto output_string = Array::createArray(context, VX_TYPE_CHAR, VX_MAX_STRING_LENGTH);
    if (Error::getStatus(output_string) != VX_SUCCESS)
    {
        std::cerr << "Failed to create output string array" << std::endl;
        return EXIT_FAILURE;
    }

    // Get AI chatbot kernel
    auto kernel = Kernel::getKernelByEnum(context, VX_KERNEL_AIS_CHATBOT);
    if (Error::getStatus(kernel) != VX_SUCCESS)
    {
        std::cerr << "Failed to get AI chatbot kernel. Make sure AI server target is loaded." << std::endl;
        return EXIT_FAILURE;
    }

    // Create node
    auto node = Node::createNode(graph, kernel, {input_string, output_string});
    if (Error::getStatus(node) != VX_SUCCESS)
    {
        std::cerr << "Failed to create chatbot node" << std::endl;
        return EXIT_FAILURE;
    }

    // Main conversation loop
    std::string query;
    while (true)
    {
        std::cout << "\nYou: ";
        std::getline(std::cin, query);

        // Check for exit commands
        if (query == "quit" || query == "exit" || query == "q")
        {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        if (query.empty())
        {
            std::cout << "Please enter a question or type 'quit' to exit." << std::endl;
            continue;
        }

        // Clear previous input from array
        if (input_string->truncate(0) != VX_SUCCESS)
        {
            std::cerr << "Failed to clear input array" << std::endl;
            continue;
        }

        // Clear previous output
        if (output_string->truncate(0) != VX_SUCCESS)
        {
            std::cerr << "Failed to clear output array" << std::endl;
            continue;
        }

        // Add new query to the input array
        if (input_string->addItems(query.length() + 1, query.c_str(), sizeof(char)) != VX_SUCCESS)
        {
            std::cerr << "Failed to add input query to array" << std::endl;
            continue;
        }

        // Process graph
        if (graph->process() != VX_SUCCESS)
        {
            std::cerr << "Failed to process graph" << std::endl;
            continue;
        }

        // Read output
        auto num_items = output_string->numItems();

        if (num_items > 0)
        {
            void *data_ptr = nullptr;
            size_t stride = 0;
            if (output_string->accessArrayRange(0, num_items, &stride, &data_ptr, VX_READ_ONLY) == VX_SUCCESS)
            {
                // Direct access to the underlying string data - no copying needed!
                const char *response = static_cast<const char*>(data_ptr);
                std::cout << "AI: " << std::string(response, num_items) << std::endl;
            }
            else
            {
                std::cerr << "Failed to access output data" << std::endl;
            }
        }
        else
        {
            std::cout << "AI: No response received from AI server" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
