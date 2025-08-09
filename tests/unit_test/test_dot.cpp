/**
 * @file test_vx_dot.cpp
 * @brief Unit tests for vxImportGraphFromDot function.
 * @date 2025-01-05
 *
 * Tests cover invalid parameters, file not found, minimal node import,
 * and edge reconstruction.
 *
 * (c) 2025 Edge.AI
 */
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <VX/vx.h>
#include "vx_internal.h"

using namespace corevx;

// Helper to write DOT file content to a temporary file.
static std::string writeTempDotFile(const std::string &content)
{
    std::filesystem::path tmpPath = std::filesystem::temp_directory_path() / "temp_graph.dot";
    std::ofstream ofs(tmpPath);
    ofs << content;
    ofs.close();
    return tmpPath.string();
}

class DotTest : public ::testing::Test {
protected:
    vx_context context = nullptr;
    vx_graph graph = nullptr;

    void SetUp() override {
        context = vxCreateContext();
        ASSERT_NE(context, nullptr);
        graph = vxCreateGraph(context);
        ASSERT_NE(graph, nullptr);
    }

    void TearDown() override {
        vxReleaseGraph(&graph);
        vxReleaseContext(&context);
    }
};

// Test that passing a null graph returns an error.
TEST_F(DotTest, NullGraphParameter)
{
    char dummyFile[] = "dummy.dot";
    vx_status status = vxImportGraphFromDot(nullptr, dummyFile, vx_false_e);
    EXPECT_EQ(status, VX_ERROR_INVALID_PARAMETERS);
}

// Test that a non-existent DOT file returns an error.
TEST_F(DotTest, FileNotFound)
{
    // Use a file path that does not exist.
    char nonExistentFile[] = "/tmp/does_not_exist.dot";
    vx_status status = vxImportGraphFromDot(graph, nonExistentFile, vx_false_e);
    EXPECT_EQ(status, VX_ERROR_INVALID_VALUE);
}

// Test minimal valid DOT file with one node.
// Here we assume the kernel "kernel_test" is valid and registered.
TEST_F(DotTest, ImportSingleNode)
{
    std::string dotContent =
        "digraph {\n"
        "N0 [label=\"N0\\norg.khronos.openvx.add\"];\n"
        "}\n";
    std::string tmpFile = writeTempDotFile(dotContent);

    vx_status status = vxImportGraphFromDot(graph, const_cast<char*>(tmpFile.c_str()), vx_false_e);
    // Expect success: the function must parse the node.
    EXPECT_EQ(status, VX_SUCCESS);
    // Further validations would typically check that graph->nodes[0] is set.
    // Assuming graph->numNodes is accessible:
    EXPECT_GT(graph->numNodes, 0u);

    std::filesystem::remove(tmpFile);
}

// Test edge reconstruction for direct node-to-node edge.
// DOT file defines two nodes and a node-to-node edge.
TEST_F(DotTest, ImportEdgeNodeToNode)
{
    std::string dotContent =
        "digraph {\n"
        "N0 [label=\"N0\\norg.khronos.openvx.scale_image\"];\n"
        "N1 [label=\"N1\\norg.khronos.openvx.scale_image\"];\n"
        "N0 -> N1;\n"
        "}\n";
    std::string tmpFile = writeTempDotFile(dotContent);

    vx_status status = vxImportGraphFromDot(graph, const_cast<char*>(tmpFile.c_str()), vx_false_e);
    EXPECT_EQ(status, VX_SUCCESS);
    // Optionally, verify that the connection was made.
    // For example, check that an input parameter of node N1 is set.
    if(graph->numNodes > 1)
    {
        vx_node node1 = graph->nodes[1];
        vx_kernel dstKernel = node1->kernel;
        bool connected = false;
        for(vx_uint32 i = 0; i < dstKernel->signature.num_parameters; ++i)
        {
            if(dstKernel->signature.directions[i] == VX_INPUT &&
               node1->parameters[i] != nullptr)
               {
                connected = true;
                break;
            }
        }
        EXPECT_TRUE(connected);
    }

    std::filesystem::remove(tmpFile);
}

// Test edge reconstruction for data mediated edge (N->D->N).
// Exported DOT contains a data node between two graph nodes.
TEST_F(DotTest, ImportEdgeDataMediated)
{
    std::string dotContent =
        "digraph {\n"
        "N0 [label=\"N0\\norg.khronos.openvx.scale_image\"];\n"
        "N1 [label=\"N1\\norg.khronos.openvx.scale_image\"];\n"
        "D0 [label=\"dummy\"];\n"
        "N0 -> D0 -> N1;\n"
        "}\n";
    std::string tmpFile = writeTempDotFile(dotContent);

    vx_status status = vxImportGraphFromDot(graph, const_cast<char*>(tmpFile.c_str()), vx_true_e);
    EXPECT_EQ(status, VX_SUCCESS);
    // Check that node N1 has an input set via the data edge.
    if(graph->numNodes > 1)
    {
        vx_node node1 = graph->nodes[1];
        vx_kernel dstKernel = node1->kernel;
        bool connected = false;
        for(vx_uint32 i = 0; i < dstKernel->signature.num_parameters; ++i)
        {
            if(dstKernel->signature.directions[i] == VX_INPUT &&
               node1->parameters[i] != nullptr)
               {
                connected = true;
                break;
            }
        }
        EXPECT_TRUE(connected);
    }

    std::filesystem::remove(tmpFile);
}

TEST_F(DotTest, ImportExportNoEdges)
{
    std::string dotContent =
        "digraph {\n"
        "\tsize=4;\n"
        "\trank=LR;\n"
        "\tnode [shape=oval style=filled fillcolor=red fontsize=27];\n"
        "\tN0 [label=\"N0\\norg.khronos.openvx.scale_image\"];\n"
        "\tN1 [label=\"N1\\norg.khronos.openvx.scale_image\"];\n"
        "}\n";
    std::string tmpInputFile = writeTempDotFile(dotContent);
    std::string tmpExportFile = tmpInputFile + ".export.dot";

    vx_status status = vxImportGraphFromDot(graph, const_cast<char*>(tmpInputFile.c_str()), vx_true_e);
    EXPECT_EQ(status, VX_SUCCESS);

    status = vxExportGraphToDot(graph, const_cast<char*>(tmpExportFile.c_str()), vx_true_e);
    EXPECT_EQ(status, VX_SUCCESS);

    // Compare the original and exported DOT files.
    std::ifstream ifs(tmpExportFile);
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    ifs.close();

    EXPECT_EQ(dotContent, buffer.str());

    std::filesystem::remove(tmpInputFile);
    std::filesystem::remove(tmpExportFile);
}