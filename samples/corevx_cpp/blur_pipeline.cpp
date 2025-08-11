#include <cstdlib>

#include <COREVX/all.hpp>

using namespace corevx;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Create context
    vx_context context = Context::createContext();
    if (Error::getStatus(context) != VX_SUCCESS)
    {
        std::cerr << "Failed to create Context" << std::endl;
        return EXIT_FAILURE;
    }

    // Create graph
    vx_graph graph = Graph::createGraph(context);
    if (Error::getStatus(graph) != VX_SUCCESS)
    {
        std::cerr << "Failed to create Graph" << std::endl;
        return EXIT_FAILURE;
    }

    // Create data objects
    const vx_uint32 width = 256, height = 256;
    vx_image rgb  = Image::createImage(context, width, height, VX_DF_IMAGE_RGB);
    vx_image yuv  = Image::createImage(context, width, height, VX_DF_IMAGE_YUV4);
    vx_image gray = Image::createImage(context, width, height, VX_DF_IMAGE_U8);
    vx_image blur = Image::createImage(context, width, height, VX_DF_IMAGE_U8);

    // Color convert (RGB -> YUV)
    vx_node ncc = vxColorConvertNode(graph, rgb, yuv);
    // Extract Y plane to gray
    vx_node nce = vxChannelExtractNode(graph, yuv, VX_CHANNEL_Y, gray);
    // Box filter 3x3
    vx_node nbox = vxBox3x3Node(graph, gray, blur);
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


