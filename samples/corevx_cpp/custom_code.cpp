/**
 * @example custom_code.cpp
 * @brief Custom kernel example
 * @version 0.1
 * @date 2025-08-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <cstdlib>
#include <iostream>

#include <COREFLOW/all.hpp>

using namespace coreflow;

int main()
{
    // 1. Create context
    auto context = Context::createContext();
    if (Error::getStatus(context) != VX_SUCCESS)
    {
        std::cerr << "Failed to create context\n";
        return EXIT_FAILURE;
    }

    // 2. Create a graph
    auto graph = Graph::createGraph(context);
    if (Error::getStatus(graph) != VX_SUCCESS)
    {
        std::cerr << "Failed to create graph\n";
        return EXIT_FAILURE;
    }

    // 3. Create input and output data objects -- in this case, scalars
    int32_t a = 7, b = 5;
    auto scalar_a = Scalar::createScalar(context, VX_TYPE_INT32, &a);
    auto scalar_b = Scalar::createScalar(context, VX_TYPE_INT32, &b);
    auto scalar_out = Scalar::createScalar(context, VX_TYPE_INT32, nullptr);

    // 4. Register the custom kernel
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
            c = a + b;
            scalar_out->writeValue(&c);

            return VX_SUCCESS;
        });
    if (Error::getStatus(add_kernel) != VX_SUCCESS)
    {
        std::cerr << "Custom kernel not registered!\n";
        return EXIT_FAILURE;
    }

    // 5. Add node to the graph
    auto add_node = Node::createNode(graph, add_kernel, {scalar_a, scalar_b, scalar_out});
    if (Error::getStatus(add_node) != VX_SUCCESS)
    {
        std::cerr << "Failed to create node\n";
        return EXIT_FAILURE;
    }

    // 6. Process the graph
    if (graph->process() != VX_SUCCESS)
    {
        std::cerr << "Graph processing failed\n";
        return EXIT_FAILURE;
    }

    // 7. Read back the result
    int32_t result = 0;
    scalar_out->readValue(&result);
    std::cout << "Result: " << a << " + " << b << " = " << result << std::endl;

    // 8. cleanup automatically done when program exits
    return EXIT_SUCCESS;
}