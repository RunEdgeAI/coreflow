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
    vx_context context = Context::createContext();
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

    // Automatically cleanup when program exits
    std::cout << "completed successfully" << std::endl;
    return EXIT_SUCCESS;
}


