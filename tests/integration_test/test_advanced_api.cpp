/**
 * @file test_advanced+api.cpp
 * @brief Test Framework Advanced API
 * @version 0.1
 * @date 2025-06-26
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include <VX/vx.h>
#include <gtest/gtest.h>

#include "vx_internal.h"

class AdvancedIntegrationTest : public ::testing::Test
{
    protected:
        vx_context context;
        vx_graph graph;

        // LogCapture struct for custom callback test
        struct LogCapture
        {
                vx_status status = VX_SUCCESS;
                std::string message;
                static void VX_CALLBACK Callback(vx_context, vx_reference, vx_status status,
                                                 const vx_char string[])
                {
                    instance().status = status;
                    instance().message = string;
                }
                static LogCapture& instance()
                {
                    static LogCapture inst;
                    return inst;
                }
                void reset()
                {
                    status = VX_SUCCESS;
                    message.clear();
                }
        };

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

    public:
        // Dummy kernel function and validator for user kernel test
        static int dummy_kernel_call_count;

        static vx_status VX_CALLBACK DummyKernelFunc(vx_node, const vx_reference*, vx_uint32)
        {
            ++dummy_kernel_call_count;
            return VX_SUCCESS;
        }

        static vx_status VX_CALLBACK DummyKernelValidate(vx_node, const vx_reference[], vx_uint32,
                                                         vx_meta_format[])
        {
            return VX_SUCCESS;
        }

        static vx_action VX_CALLBACK callback(vx_node node)
        {
            vx_action action = VX_ACTION_ABANDON;
            vx_size expected = 2;
            vx_parameter param = vxGetParameterByIndex(node, 1);  // copied Value
            if (vxGetStatus(param) == VX_SUCCESS)
            {
                vx_scalar scalar = nullptr;
                vxQueryParameter(param, VX_PARAMETER_REF, &scalar, sizeof(scalar));
                if (vxGetStatus(scalar) == VX_SUCCESS)
                {
                    vx_uint8 value = 0u;
                    vxCopyScalar(scalar, &value, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
                    if (value == expected)
                    {
                        action = VX_ACTION_CONTINUE;
                    }
                }
            }
            return action;
        }
};

// Define the static member
int AdvancedIntegrationTest::dummy_kernel_call_count = 0;

TEST_F(AdvancedIntegrationTest, TestNodeCallback)
{
    vx_size in_count = 2u, out_count = 0;

    // Create IOs
    vx_scalar input = vxCreateScalar(context, VX_TYPE_SIZE, &in_count);
    vx_scalar output = vxCreateScalar(context, VX_TYPE_SIZE, &out_count);
    ASSERT_EQ(vxGetStatus(input), VX_SUCCESS);
    ASSERT_EQ(vxGetStatus(output), VX_SUCCESS);

    // Create graph
    graph = vxCreateGraph(context);
    ASSERT_EQ(vxGetStatus(graph), VX_SUCCESS);

    // Create node
    vx_node node = vxCopyNode(graph, (vx_reference)input, (vx_reference)output);
    ASSERT_EQ(vxGetStatus(node), VX_SUCCESS);

    // Assign callback
    ASSERT_EQ(vxAssignNodeCallback(node, &AdvancedIntegrationTest::callback), VX_SUCCESS);
    ASSERT_EQ(vxRetrieveNodeCallback(node), &AdvancedIntegrationTest::callback);

    // Verify graph
    ASSERT_EQ(vxVerifyGraph(graph), VX_SUCCESS);

    // Process graph
    ASSERT_EQ(vxProcessGraph(graph), VX_SUCCESS);

    // Validate results
    ASSERT_EQ(vxCopyScalar(output, &out_count, VX_READ_ONLY, VX_MEMORY_TYPE_HOST), VX_SUCCESS);
    ASSERT_EQ(in_count, out_count);

    // Cleanup
    vxReleaseScalar(&input);
    vxReleaseScalar(&output);
    vxReleaseNode(&node);
}

TEST_F(AdvancedIntegrationTest, TestAddLogEntryWithHelperLogReader)
{
    vxRegisterHelperAsLogReader(context);
    const char* test_message = "UnitTest log entry: %d";
    int test_value = 42;
    vxAddLogEntry((vx_reference)context, VX_FAILURE, test_message, test_value);
    char log_message[VX_MAX_LOG_MESSAGE_LEN] = {0};
    vx_status log_status = vxGetLogEntry((vx_reference)context, log_message);
    ASSERT_EQ(log_status, VX_FAILURE);
    ASSERT_STREQ(log_message, "UnitTest log entry: 42");

    // The log should now be empty
    log_status = vxGetLogEntry((vx_reference)context, log_message);
    ASSERT_EQ(log_status, VX_SUCCESS);
}

TEST_F(AdvancedIntegrationTest, TestAddLogEntryWithCustomCallback)
{
    LogCapture::instance().reset();
    vxRegisterLogCallback(context, &LogCapture::Callback, vx_false_e);
    const char* test_message = "CustomCallback log entry: %s %d";
    const char* word = "value";
    int number = 99;
    vxAddLogEntry((vx_reference)context, VX_ERROR_INVALID_VALUE, test_message, word, number);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_EQ(LogCapture::instance().status, VX_ERROR_INVALID_VALUE);
    ASSERT_EQ(LogCapture::instance().message, "CustomCallback log entry: value 99");
    vxRegisterLogCallback(context, nullptr, vx_false_e);
}

TEST_F(AdvancedIntegrationTest, TestDirectiveLoggingAndPerformance)
{
    // Enable logging and check that log entries are recorded
    ASSERT_EQ(vxDirective((vx_reference)context, VX_DIRECTIVE_ENABLE_LOGGING), VX_SUCCESS);
    vxRegisterHelperAsLogReader(context);
    vxAddLogEntry((vx_reference)context, VX_FAILURE, "Log should be recorded");
    char log_message[VX_MAX_LOG_MESSAGE_LEN] = {0};
    vx_status log_status = vxGetLogEntry((vx_reference)context, log_message);
    ASSERT_EQ(log_status, VX_FAILURE);
    ASSERT_STREQ(log_message, "Log should be recorded");

    // Disable logging and check that log entries are not recorded
    ASSERT_EQ(vxDirective((vx_reference)context, VX_DIRECTIVE_DISABLE_LOGGING), VX_SUCCESS);
    vxAddLogEntry((vx_reference)context, VX_FAILURE, "Log should NOT be recorded");
    log_status = vxGetLogEntry((vx_reference)context, log_message);
    ASSERT_EQ(log_status, VX_SUCCESS);  // No new log entry

    // Enable/disable performance (should be supported for context)
    ASSERT_EQ(vxDirective((vx_reference)context, VX_DIRECTIVE_ENABLE_PERFORMANCE), VX_SUCCESS);
    ASSERT_EQ(vxDirective((vx_reference)context, VX_DIRECTIVE_DISABLE_PERFORMANCE), VX_SUCCESS);

    // Try a directive not supported for this reference type (e.g., performance on a scalar)
    vx_scalar scalar = vxCreateScalar(context, VX_TYPE_INT32, nullptr);
    ASSERT_EQ(vxDirective((vx_reference)scalar, VX_DIRECTIVE_ENABLE_PERFORMANCE),
              VX_ERROR_NOT_SUPPORTED);
    vxReleaseScalar(&scalar);
}

TEST_F(AdvancedIntegrationTest, TestAddUserKernelLifecycle)
{
    AdvancedIntegrationTest::dummy_kernel_call_count = 0;
    const char* kernel_name = "org.khronos.unittest.dummy";
    vx_enum kernel_enum = 0;
    ASSERT_EQ(vxAllocateUserKernelId(context, &kernel_enum), VX_SUCCESS);
    vx_kernel kernel = vxAddUserKernel(
        context, kernel_name, kernel_enum, &AdvancedIntegrationTest::DummyKernelFunc, 1,
        &AdvancedIntegrationTest::DummyKernelValidate, nullptr, nullptr);
    ASSERT_NE(kernel, nullptr);
    ASSERT_EQ(vxGetStatus((vx_reference)kernel), VX_SUCCESS);

    // Add a parameter (required before finalizing)
    ASSERT_EQ(
        vxAddParameterToKernel(kernel, 0, VX_INPUT, VX_TYPE_UINT8, VX_PARAMETER_STATE_REQUIRED),
        VX_SUCCESS);

    // Finalize the kernel
    ASSERT_EQ(vxFinalizeKernel(kernel), VX_SUCCESS);

    // Kernel should be retrievable by name
    vx_kernel found = vxGetKernelByName(context, kernel_name);
    ASSERT_EQ(found, kernel);

    // Create a real graph and scalar parameter
    vx_graph graph = vxCreateGraph(context);
    ASSERT_EQ(vxGetStatus((vx_reference)graph), VX_SUCCESS);
    vx_uint8 scalar_value = 123;
    vx_scalar scalar = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_value);
    ASSERT_EQ(vxGetStatus((vx_reference)scalar), VX_SUCCESS);

    // Add a node using the user kernel and the scalar as input
    vx_node node = vxCreateGenericNode(graph, kernel);
    ASSERT_NE(node, nullptr);
    ASSERT_EQ(vxGetStatus((vx_reference)node), VX_SUCCESS);
    ASSERT_EQ(vxSetParameterByIndex(node, 0, (vx_reference)scalar), VX_SUCCESS);

    // Verify and process the graph
    ASSERT_EQ(vxVerifyGraph(graph), VX_SUCCESS);
    ASSERT_EQ(vxProcessGraph(graph), VX_SUCCESS);
    ASSERT_EQ(AdvancedIntegrationTest::dummy_kernel_call_count, 1);

    // Cleanup
    vxReleaseNode(&node);
    vxReleaseScalar(&scalar);
    vxReleaseGraph(&graph);
    ASSERT_EQ(vxRemoveKernel(kernel), VX_SUCCESS);
    ASSERT_EQ(vxReleaseKernel(&kernel), VX_SUCCESS);
}