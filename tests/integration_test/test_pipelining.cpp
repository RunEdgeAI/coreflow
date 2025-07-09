/**
 * @file test_pipelining.cpp
 * @brief Test pipelining
 * @date 2025-07-07
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#include <VX/vx.h>
#include <VX/vx_lib_debug.h>
#include <VX/vx_khr_pipelining.h>
#include <VX/vx_nodes.h>
#include <gtest/gtest.h>

#include "vx_internal.h"

const std::vector<std::string> question_bank =
{
    "be very brief; what is the capital of the united states ?",
    "be very brief; who was the 20th president of the united states ?",
    "be very brief; what state is minneapolis in ?"
};

class PipeliningTest : public ::testing::Test
{
protected:
    vx_context context;
    vx_graph graph;

    void SetUp() override
    {
        // Initialize OpenVX context
        context = vxCreateContext();
        graph = vxCreateGraph(context);

        ASSERT_EQ(vxGetStatus(context), VX_SUCCESS);
        ASSERT_EQ(vxGetStatus(graph), VX_SUCCESS);
    }

    void TearDown() override
    {
        vxReleaseGraph(&graph);
        vxReleaseContext(&context);
    }
public:
    static const vx_enum VX_KERNEL_USER_TRIGGER = 0x9000;

    static vx_status VX_CALLBACK trigger_kernel_func(vx_node node, const vx_reference *parameters,
                                              vx_uint32 num)
    {
        (void)node;
        static vx_size i = 0u;
        if (num != 1) return VX_ERROR_INVALID_PARAMETERS;
        vx_array out_arr = (vx_array)parameters[0];

        // Example: Write a static string to the array
        const char *trigger_str = question_bank[i++].c_str();;
        vx_size len = strlen(trigger_str) + 1;
        vx_status status = vxTruncateArray(out_arr, 0);
        if (status != VX_SUCCESS) return status;
        status = vxAddArrayItems(out_arr, len, trigger_str, sizeof(vx_char));
        return status;
    }

    static void register_trigger_kernel(vx_context context)
    {
        vx_kernel kernel = vxAddUserKernel(
            context,
            "user.trigger",
            VX_KERNEL_USER_TRIGGER,
            trigger_kernel_func,
            1, // One parameter
            nullptr, nullptr, nullptr
        );
        if (kernel)
        {
            // Add one output parameter of type vx_array
            vxAddParameterToKernel(kernel, 0, VX_OUTPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED);
            vxFinalizeKernel(kernel);
            vxReleaseKernel(&kernel);
        }
    }
};

TEST_F(PipeliningTest, TestPipelining)
{

    vx_array in_arr = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
    vx_array out_arr = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
    ASSERT_EQ(vxGetStatus(in_arr), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(out_arr), VX_SUCCESS);

    vx_kernel kernel = vxGetKernelByEnum(context, VX_KERNEL_AIS_CHATBOT);
    vx_node node = vxCreateGenericNode(graph, kernel);
    ASSERT_EQ(vxGetStatus(kernel), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(node), VX_SUCCESS);

    ASSERT_EQ(vxSetParameterByIndex(node, 0, in_arr), VX_SUCCESS);
    ASSERT_EQ(vxSetParameterByIndex(node, 1, out_arr), VX_SUCCESS);
    // Set graph parameters for pipelining
    ASSERT_EQ(vxAddParameterToGraph(graph, vxGetParameterByIndex(node, 0)), VX_SUCCESS);
    ASSERT_EQ(vxAddParameterToGraph(graph, vxGetParameterByIndex(node, 1)), VX_SUCCESS);

    // Set up pipelining config
    vx_graph_parameter_queue_params_t qparams[2] = {};
    vx_reference in_refs[1] = { in_arr };
    vx_reference out_refs[1] = { out_arr };

    qparams[0].graph_parameter_index = 0;
    qparams[0].refs_list = in_refs;
    qparams[0].refs_list_size = 1;

    qparams[1].graph_parameter_index = 1;
    qparams[1].refs_list = out_refs;
    qparams[1].refs_list_size = 1;

    ASSERT_EQ(vxSetGraphScheduleConfig(graph, VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO, 2, qparams),
              VX_SUCCESS);

    ASSERT_EQ(vxVerifyGraph(graph), VX_SUCCESS);

    for (auto i = 0u; i < question_bank.size(); ++i)
    {
        ASSERT_EQ(vxTruncateArray(in_arr, 0), VX_SUCCESS);
        ASSERT_EQ(vxAddArrayItems(in_arr, question_bank[i].size() + 1, question_bank[i].c_str(),
                                  sizeof(vx_char)),
                  VX_SUCCESS);

        ASSERT_EQ(vxGraphParameterEnqueueReadyRef(graph, 0, in_refs, 1), VX_SUCCESS);
        ASSERT_EQ(vxGraphParameterEnqueueReadyRef(graph, 1, out_refs, 1), VX_SUCCESS);

        vx_reference out_done[1];
        vx_uint32 num_refs = 0;
        ASSERT_EQ(vxGraphParameterDequeueDoneRef(graph, 1, out_done, 1, &num_refs), VX_SUCCESS);
        ASSERT_EQ(num_refs, 1);
        ASSERT_EQ(out_done[0], out_arr);

        ASSERT_EQ(vxWaitGraph(graph), VX_SUCCESS);
    }

    // Optionally validate output contents here


    vxReleaseKernel(&kernel);
    vxReleaseNode(&node);
    vxReleaseArray(&in_arr);
    vxReleaseArray(&out_arr);
}

TEST_F(PipeliningTest, TestStreaming)
{

    vx_array in_arr = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
    vx_array out_arr = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
    ASSERT_EQ(vxGetStatus(in_arr), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(out_arr), VX_SUCCESS);

    ASSERT_EQ(vxTruncateArray(in_arr, 0), VX_SUCCESS);
    ASSERT_EQ(vxAddArrayItems(in_arr, question_bank[0].size() + 1, question_bank[0].c_str(),
                              sizeof(vx_char)), VX_SUCCESS);

    vx_kernel kernel = vxGetKernelByEnum(context, VX_KERNEL_AIS_CHATBOT);
    vx_node node = vxCreateGenericNode(graph, kernel);
    ASSERT_EQ(vxGetStatus(kernel), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(node), VX_SUCCESS);

    ASSERT_EQ(vxSetParameterByIndex(node, 0, in_arr), VX_SUCCESS);
    ASSERT_EQ(vxSetParameterByIndex(node, 1, out_arr), VX_SUCCESS);

    // Set graph parameters for pipelining
    ASSERT_EQ(vxAddParameterToGraph(graph, vxGetParameterByIndex(node, 0)), VX_SUCCESS);
    ASSERT_EQ(vxAddParameterToGraph(graph, vxGetParameterByIndex(node, 1)), VX_SUCCESS);

    // Setup Graph
    ASSERT_EQ(vxEnableEvents(context), VX_SUCCESS);
    ASSERT_EQ(vxEnableGraphStreaming(graph, node), VX_SUCCESS);
    ASSERT_EQ(vxStartGraphStreaming(graph), VX_SUCCESS);

    vx_event_t event;
    vx_size i = 1u;

    while (i < question_bank.size())
    {
        ASSERT_EQ(vxWaitEvent(context, &event, vx_false_e), VX_SUCCESS);
        if (event.type != VX_EVENT_GRAPH_COMPLETED) continue;
        ASSERT_EQ(vxTruncateArray(in_arr, 0), VX_SUCCESS);
        ASSERT_EQ(vxAddArrayItems(in_arr, question_bank[i].size() + 1, question_bank[i].c_str(),
            sizeof(vx_char)), VX_SUCCESS);
        ++i;
    }

    ASSERT_EQ(vxStopGraphStreaming(graph), VX_SUCCESS);

    vxReleaseKernel(&kernel);
    vxReleaseNode(&node);
    vxReleaseArray(&in_arr);
    vxReleaseArray(&out_arr);
}

TEST_F(PipeliningTest, TestStreamingWithTriggerNode)
{

    register_trigger_kernel(context);

    vx_array out_arr = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
    ASSERT_EQ(vxGetStatus(out_arr), VX_SUCCESS);

    vx_array trigger_out_arr = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
    ASSERT_EQ(vxGetStatus(trigger_out_arr), VX_SUCCESS);

    // Create a trigger node (dummy node or user kernel)
    vx_kernel trigger_kernel = vxGetKernelByEnum(context, VX_KERNEL_USER_TRIGGER);
    vx_node trigger_node = vxCreateGenericNode(graph, trigger_kernel);
    ASSERT_EQ(vxGetStatus(trigger_kernel), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(trigger_node), VX_SUCCESS);

    // Set trigger node output
    ASSERT_EQ(vxSetParameterByIndex(trigger_node, 0, trigger_out_arr), VX_SUCCESS);

    // Now use trigger_out_arr as input to the chatbot node
    vx_kernel kernel = vxGetKernelByEnum(context, VX_KERNEL_AIS_CHATBOT);
    vx_node node = vxCreateGenericNode(graph, kernel);
    ASSERT_EQ(vxSetParameterByIndex(node, 0, trigger_out_arr), VX_SUCCESS);
    ASSERT_EQ(vxSetParameterByIndex(node, 1, out_arr), VX_SUCCESS);

    // Setup Graph
    ASSERT_EQ(vxEnableEvents(context), VX_SUCCESS);
    ASSERT_EQ(vxEnableGraphStreaming(graph, trigger_node), VX_SUCCESS);
    ASSERT_EQ(vxStartGraphStreaming(graph), VX_SUCCESS);

    vx_event_t event;
    vx_size i = 0u;

    // Feed data on trigger event
    while (i < question_bank.size())
    {
        ASSERT_EQ(vxWaitEvent(context, &event, vx_false_e), VX_SUCCESS);
        if (event.type != VX_EVENT_GRAPH_COMPLETED) continue;
        ++i;
    }

    ASSERT_EQ(vxStopGraphStreaming(graph), VX_SUCCESS);

    vxReleaseKernel(&trigger_kernel);
    vxReleaseNode(&trigger_node);
    vxReleaseKernel(&kernel);
    vxReleaseNode(&node);
    vxReleaseArray(&out_arr);
    vxReleaseArray(&trigger_out_arr);
}

TEST_F(PipeliningTest, TestStreamingAndPipelining)
{
    // 1. Create input/output arrays
    vx_size size = question_bank.size();
    std::vector<vx_array> in_arrs(size);
    std::vector<vx_array> out_arrs(size);
    for (vx_size i = 0; i < size; ++i)
    {
        in_arrs[i] = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
        out_arrs[i] = vxCreateArray(context, VX_TYPE_CHAR, VX_MAX_FILE_NAME);
        ASSERT_EQ(vxGetStatus(in_arrs[i]), VX_SUCCESS);
        ASSERT_EQ(vxGetStatus(out_arrs[i]), VX_SUCCESS);
    }

    // 2. Create kernel and node
    vx_kernel kernel = vxGetKernelByEnum(context, VX_KERNEL_AIS_CHATBOT);
    vx_node node = vxCreateGenericNode(graph, kernel);
    ASSERT_EQ(vxGetStatus(kernel), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(node), VX_SUCCESS);

    // 3. Set parameters for node
    ASSERT_EQ(vxSetParameterByIndex(node, 0, in_arrs[0]), VX_SUCCESS);
    ASSERT_EQ(vxSetParameterByIndex(node, 1, out_arrs[0]), VX_SUCCESS);

    // 4. Set graph parameters for pipelining
    ASSERT_EQ(vxAddParameterToGraph(graph, vxGetParameterByIndex(node, 0)), VX_SUCCESS);
    ASSERT_EQ(vxAddParameterToGraph(graph, vxGetParameterByIndex(node, 1)), VX_SUCCESS);

    // 5. Set up pipelining config
    vx_graph_parameter_queue_params_t qparams[2] = {};
    std::vector<vx_reference> in_refs(size);
    std::vector<vx_reference> out_refs(size);
    for (vx_size i = 0; i < size; ++i)
    {
        in_refs[i] = in_arrs[i];
        out_refs[i] = out_arrs[i];
    }
    qparams[0].graph_parameter_index = 0;
    qparams[0].refs_list = in_refs.data();
    qparams[0].refs_list_size = size;

    qparams[1].graph_parameter_index = 1;
    qparams[1].refs_list = out_refs.data();
    qparams[1].refs_list_size = size;

    ASSERT_EQ(vxSetGraphScheduleConfig(graph, VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO, 2, qparams), VX_SUCCESS);

    // 6. Enable events and streaming
    ASSERT_EQ(vxEnableEvents(context), VX_SUCCESS);
    ASSERT_EQ(vxEnableGraphStreaming(graph, node), VX_SUCCESS);
    ASSERT_EQ(vxStartGraphStreaming(graph), VX_SUCCESS);

    // 7. Enqueue all inputs
    for (vx_size i = 0; i < size; ++i)
    {
        ASSERT_EQ(vxTruncateArray(in_arrs[i], 0), VX_SUCCESS);
        ASSERT_EQ(vxAddArrayItems(in_arrs[i], question_bank[i].size() + 1, question_bank[i].c_str(), sizeof(vx_char)), VX_SUCCESS);
        ASSERT_EQ(vxGraphParameterEnqueueReadyRef(graph, 0, &in_refs[i], 1), VX_SUCCESS);
        ASSERT_EQ(vxGraphParameterEnqueueReadyRef(graph, 1, &out_refs[i], 1), VX_SUCCESS);
    }

    // 8. Dequeue outputs as they are ready, using events
    vx_size num_outputs = 0;
    vx_reference out_done[1];
    vx_uint32 num_refs = 0;
    while (num_outputs < size)
    {
        vx_event_t event;
        ASSERT_EQ(vxWaitEvent(context, &event, vx_false_e), VX_SUCCESS);
        if (event.type == VX_EVENT_GRAPH_COMPLETED)
        {
            // Dequeue output
            ASSERT_EQ(vxGraphParameterDequeueDoneRef(graph, 1, out_done, 1, &num_refs), VX_SUCCESS);
            ASSERT_EQ(num_refs, 1);
            ++num_outputs;
        }
    }

    ASSERT_EQ(vxStopGraphStreaming(graph), VX_SUCCESS);

    vxReleaseKernel(&kernel);
    vxReleaseNode(&node);
    for (vx_size i = 0; i < size; ++i)
    {
        vxReleaseArray(&in_arrs[i]);
        vxReleaseArray(&out_arrs[i]);
    }
}
