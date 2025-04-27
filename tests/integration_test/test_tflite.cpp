/**
 * @file test_tflite.cpp
 * @brief Test TFLite Target
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

#include <VX/vx.h>

#include "vx_internal.h"

class TFLiteIntegrationTest : public ::testing::Test
{
protected:
    vx_context context;
    vx_graph graph;
    std::string model_path = "./tests/raw/matmul_model.tflite";

    void SetUp() override
    {
        // Initialize OpenVX context
        context = vxCreateContext();
        ASSERT_EQ(vxGetStatus(context), VX_SUCCESS);
    }

    void TearDown() override
    {
        vxReleaseGraph(&graph);
        vxReleaseContext(&context);
    }
};

TEST_F(TFLiteIntegrationTest, TfliteMatMul)
{
    const vx_size numDims = 2u;
    vx_size inputADims[] = {3, 4};
    vx_size inputBDims[] = {4, 3};
    vx_size outputDims[] = {3, 3};

    // Create input tensors
    vx_tensor input_a = vxCreateTensor(context, numDims, inputADims, VX_TYPE_FLOAT32, 0);
    vx_tensor input_b = vxCreateTensor(context, numDims, inputBDims, VX_TYPE_FLOAT32, 0);
    vx_tensor output_c = vxCreateTensor(context, numDims, outputDims, VX_TYPE_FLOAT32, 0);
    ASSERT_EQ(vxGetStatus(input_a), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(input_b), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(output_c), VX_SUCCESS);

    // Query tensor strides
    vx_size inputAStride[numDims];
    vx_size inputBStride[numDims];
    vx_size outputStride[numDims];
    ASSERT_EQ(VX_SUCCESS, vxQueryTensor(input_a, VX_TENSOR_STRIDE, inputAStride, sizeof(inputAStride)));
    ASSERT_EQ(VX_SUCCESS, vxQueryTensor(input_b, VX_TENSOR_STRIDE, inputBStride, sizeof(inputBStride)));
    ASSERT_EQ(VX_SUCCESS, vxQueryTensor(output_c, VX_TENSOR_STRIDE, outputStride, sizeof(outputStride)));

    // Create object arrays for inputs and outputs
    vx_object_array input_tensors = vxCreateObjectArrayWithType(context, VX_TYPE_TENSOR);
    vx_object_array output_tensors = vxCreateObjectArrayWithType(context, VX_TYPE_TENSOR);
    ASSERT_EQ(vxGetStatus(input_tensors), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(output_tensors), VX_SUCCESS);

    // Set object array with items
    ASSERT_EQ(VX_SUCCESS, vxSetObjectArrayItem(input_tensors, 0, (vx_reference)input_a));
    ASSERT_EQ(VX_SUCCESS, vxSetObjectArrayItem(input_tensors, 1, (vx_reference)input_b));
    ASSERT_EQ(VX_SUCCESS, vxSetObjectArrayItem(output_tensors, 0, (vx_reference)output_c));
    ASSERT_EQ(input_a, (vx_tensor)vxGetObjectArrayItem(input_tensors, 0));
    ASSERT_EQ(input_b, (vx_tensor)vxGetObjectArrayItem(input_tensors, 1));
    ASSERT_EQ(output_c, (vx_tensor)vxGetObjectArrayItem(output_tensors, 0));

    // Create model path array
    vx_array model_path_array = vxCreateArray(context, VX_TYPE_CHAR, model_path.length() + 1);
    ASSERT_EQ(vxGetStatus(model_path_array), VX_SUCCESS);
    ASSERT_EQ(VX_SUCCESS, vxAddArrayItems(model_path_array, model_path.length() + 1, model_path.c_str(), sizeof(char)));

    // Create graph
    graph = vxCreateGraph(context);
    ASSERT_EQ(vxGetStatus(graph), VX_SUCCESS);

    // Get tflite kernel
    vx_kernel kernel = vxGetKernelByEnum(context, VX_KERNEL_LITERT_CPU_INF);
    ASSERT_EQ(vxGetStatus(kernel), VX_SUCCESS);

    // Create node
    vx_node node = vxCreateGenericNode(graph, kernel);
    ASSERT_EQ(vxGetStatus(node), VX_SUCCESS);

    // Set node parameters
    ASSERT_EQ(VX_SUCCESS, vxSetParameterByIndex(node, 0, (vx_reference)model_path_array));
    ASSERT_EQ(VX_SUCCESS, vxSetParameterByIndex(node, 1, (vx_reference)input_tensors));
    ASSERT_EQ(VX_SUCCESS, vxSetParameterByIndex(node, 2, (vx_reference)output_tensors));

    // Verify graph
    ASSERT_EQ(vxVerifyGraph(graph), VX_SUCCESS);

    // Fill input data
    vx_float32 input_data_a[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    vx_float32 input_data_b[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    vx_size viewStart[VX_MAX_TENSOR_DIMENSIONS] = {0};

    ASSERT_EQ(VX_SUCCESS, vxCopyTensorPatch((vx_tensor)vxGetObjectArrayItem(input_tensors, 0), numDims, viewStart, inputADims, inputAStride, input_data_a, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST));
    ASSERT_EQ(VX_SUCCESS, vxCopyTensorPatch((vx_tensor)vxGetObjectArrayItem(input_tensors, 1), numDims, viewStart, inputBDims, inputBStride, input_data_b, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST));

    // Process graph
    ASSERT_EQ(vxProcessGraph(graph), VX_SUCCESS);

    // Read output
    float output_data[9];
    ASSERT_EQ(VX_SUCCESS, vxCopyTensorPatch((vx_tensor)vxGetObjectArrayItem(output_tensors, 0), numDims, viewStart, outputDims, outputStride, output_data, VX_READ_ONLY, VX_MEMORY_TYPE_HOST));

    // Validate results
    float expected[9] = {70, 80, 90, 158, 184, 210, 246, 288, 330};
    for (vx_uint8 i = 0; i < 9; i++)
    {
        EXPECT_NEAR(output_data[i], expected[i], 1e-5);
    }

    // Cleanup
    vxReleaseTensor(&input_a);
    vxReleaseTensor(&input_b);
    vxReleaseTensor(&output_c);
    vxReleaseArray(&model_path_array);
    vxReleaseObjectArray(&input_tensors);
    vxReleaseObjectArray(&output_tensors);
    vxReleaseKernel(&kernel);
    vxReleaseNode(&node);
}