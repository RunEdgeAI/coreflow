/**
 * @file test_aiserver.cpp
 * @brief Test with internal model server
 * @version 0.1
 * @date 2025-06-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <string>

#include <VX/vx.h>
#include <VX/vx_lib_debug.h>
#include <gtest/gtest.h>

class AiServerIntegrationTest : public ::testing::Test
{
    protected:
        vx_context context;
        vx_graph graph;

        void SetUp() override
        {
            // Initialize OpenVX context
            context = vxCreateContext();
            ASSERT_EQ(vxGetStatus((vx_reference)context), VX_SUCCESS);
        }

        void TearDown() override
        {
            vxReleaseGraph(&graph);
            vxReleaseContext(&context);
        }
};

TEST_F(AiServerIntegrationTest, AiServerTest)
{
    std::string query = "what is the capital of the United States ?";

    // Create input string
    vx_array input_string = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
    ASSERT_EQ(vxGetStatus((vx_reference)input_string), VX_SUCCESS);
    ASSERT_EQ(VX_SUCCESS,
              vxAddArrayItems(input_string, query.length() + 1, query.c_str(), sizeof(char)));

    // Create output string
    vx_array output_string = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
    ASSERT_EQ(vxGetStatus((vx_reference)output_string), VX_SUCCESS);

    // Create graph
    graph = vxCreateGraph(context);
    ASSERT_EQ(vxGetStatus((vx_reference)graph), VX_SUCCESS);

    // Get AI chatbot kernel
    vx_kernel kernel = vxGetKernelByEnum(context, VX_KERNEL_AIS_CHATBOT);
    ASSERT_EQ(vxGetStatus((vx_reference)kernel), VX_SUCCESS);

    // Create node
    vx_node node = vxCreateGenericNode(graph, kernel);
    ASSERT_EQ(vxGetStatus((vx_reference)node), VX_SUCCESS);

    // Set node parameters
    ASSERT_EQ(VX_SUCCESS, vxSetParameterByIndex(node, 0, (vx_reference)input_string));
    ASSERT_EQ(VX_SUCCESS, vxSetParameterByIndex(node, 1, (vx_reference)output_string));

    // Verify graph
    ASSERT_EQ(vxVerifyGraph(graph), VX_SUCCESS);

    // Process graph
    ASSERT_EQ(vxProcessGraph(graph), VX_SUCCESS);

    // Read output
    char output_buffer[VX_MAX_FILE_NAME];
    vx_size num_items = 0;
    ASSERT_EQ(VX_SUCCESS,
              vxQueryArray(output_string, VX_ARRAY_NUMITEMS, &num_items, sizeof(num_items)));
    ASSERT_EQ(VX_SUCCESS, vxCopyArrayRange(output_string, 0, num_items, sizeof(char), output_buffer,
                                           VX_READ_ONLY, VX_MEMORY_TYPE_HOST));
    output_buffer[num_items] = '\0';

    // Validate results
    std::string response(output_buffer);
    ASSERT_TRUE(response.find("Washington, D.C.") != std::string::npos)
        << "Expected response to contain 'Washington, D.C.', but got: " << response;

    // Cleanup
    vxReleaseArray(&input_string);
    vxReleaseArray(&output_string);
    vxReleaseKernel(&kernel);
    vxReleaseNode(&node);
}