/**
 * @example blur_pipeline.cpp
 * @brief Image Processing Pipeline
 * @version 0.1
 * @date 2025-09-06
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 */
#include <cstdlib>

#include <COREVX/all.hpp>

using namespace corevx;

int main()
{
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

    // Create data objects
    const vx_uint32 width = 256, height = 256;
    auto rgb  = Image::createImage(context, width, height, VX_DF_IMAGE_RGB);
    auto yuv  = Image::createImage(context, width, height, VX_DF_IMAGE_YUV4);
    auto gray = Image::createImage(context, width, height, VX_DF_IMAGE_U8);
    auto blur = Image::createImage(context, width, height, VX_DF_IMAGE_U8);

    // Color convert (RGB -> YUV)
    auto ncc = vxColorConvertNode(graph, rgb, yuv);
    // Extract Y plane to gray
    auto nce = vxChannelExtractNode(graph, yuv, VX_CHANNEL_Y, gray);
    // Box filter 3x3
    auto nbox = vxBox3x3Node(graph, gray, blur);
    (void)ncc; (void)nce; (void)nbox;

    // Process graph
    if (graph->process() != VX_SUCCESS)
    {
        std::cerr << "Graph process failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Blur pipeline ran successfully" << std::endl;
    return EXIT_SUCCESS;
}


