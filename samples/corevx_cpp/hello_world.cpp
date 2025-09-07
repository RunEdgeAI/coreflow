/**
 * @example hello_world.cpp
 * @brief Hello World Example
 * @version 0.1
 * @date 2025-09-06
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 */
#include <cstdlib>
#include <iostream>

#include <COREVX/all.hpp>

using namespace corevx;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <graph.xml>" << std::endl;
        return EXIT_FAILURE;
    }

    // Create context
    auto context = Context::createContext();
    if (Error::getStatus(context) != VX_SUCCESS)
    {
        std::cerr << "Failed to create Context" << std::endl;
        return EXIT_FAILURE;
    }

    // Import graph(s) from XML
    auto graphs = xml::Import::importFromXML(context, argv[1]);
    if (graphs.empty())
    {
        std::cerr << "Failed to import graph from XML" << std::endl;
        return EXIT_FAILURE;
    }

    // Process graph
    for (auto graph : graphs)
    {
        if (Error::getStatus(graph) == VX_SUCCESS)
        {
            if (graph->process() == VX_SUCCESS)
            {
                std::cout << "Graph processed successfully" << std::endl;
            }
            else
            {
                std::cerr << "Failed to process graph" << std::endl;
            }
        }
    }

    // Print performance metrics
    for (auto graph : graphs)
    {
        auto perf = graph->performance();
        std::cout << "Graph " << (size_t)graph << " performance metrics:" << std::endl
                  << "  begin time (ns): " << perf.beg << std::endl
                  << "  end time (ns): " << perf.end << std::endl
                  << "  sum time (ns): " << perf.sum << std::endl
                  << "  num runs: " << perf.num << std::endl
                  << "  avg time (ns): " << perf.avg << std::endl
                  << "  min time (ns): " << perf.min << std::endl
                  << "  max time (ns): " << perf.max << std::endl
                  << std::endl;
    }

    // Automatically cleanup when program exits
    std::cout << "completed successfully" << std::endl;
    return EXIT_SUCCESS;
}


