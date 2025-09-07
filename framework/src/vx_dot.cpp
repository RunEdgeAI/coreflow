/*
 * Copyright (c) 2012-2017 The Khronos Group Inc. *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>

#include "vx_internal.h"
#include "vx_type_pairs.h"

using namespace coreflow;

#if defined(EXPERIMENTAL_USE_DOT)
VX_API_ENTRY vx_status VX_API_CALL vxExportGraphToDot(vx_graph graph, vx_char dotfile[], vx_bool showData)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (Reference::isValidReference(graph, VX_TYPE_GRAPH) == vx_true_e)
    {
        FILE *fp = fopen(dotfile, "w+");
        if (fp)
        {
            vx_uint32 n, p, n2, d;
            vx_uint32 num_next, next_nodes[VX_INT_MAX_REF];
            vx_uint32 num_last, last_nodes[VX_INT_MAX_REF];
            vx_uint32 num_left, left_nodes[VX_INT_MAX_REF];
            vx_uint32 dep_nodes[VX_INT_MAX_REF];
            vx_reference data[VX_INT_MAX_REF];
            vx_uint32 num_data = 0u;

            status = VX_SUCCESS;
            fprintf(fp, "digraph {\n");
            fprintf(fp, "\tsize=4;\n");
            fprintf(fp, "\trank=LR;\n");
            fprintf(fp, "\tnode [shape=oval style=filled fillcolor=red fontsize=27];\n");
            for (n = 0; n < graph->numNodes; n++)
            {
                vx_node node = graph->nodes[n];
                fprintf(fp, "\tN%u [label=\"N%u\\n%s\"];\n", n, n, node->kernel->name);
                if (showData)
                {
                    for (p = 0u; p < node->kernel->signature.num_parameters; p++)
                    {
                        if (node->parameters[p] == nullptr) continue;
                        for (d = 0u; d < num_data; d++)
                        {
                            if (data[d] == node->parameters[p])
                                break;
                        }
                        if (d == num_data)
                        {
                            /* new reference added to data list */
                            data[num_data++] = node->parameters[p];
                        }
                    }
                }
            }
            if (showData)
            {
                for (d = 0u; d < num_data; d++)
                {
                    vx_int32 i = TypePairs::stringFromType(data[d]->type);
                    if (data[d] == nullptr) continue;

                    switch (data[d]->type)
                    {
                        case VX_TYPE_IMAGE:
                        {
                            vx_image image = (vx_image)data[d];
                            vx_char fcc[5];
                            strncpy(fcc, (char *)&image->format, 4);
                            fcc[4] = '\0';
                            fprintf(fp, "\tD%u [shape=box label=\"Image\\n%ux%u\\n%4s\"];\n", d, image->width, image->height, fcc);
                            break;
                        }
                        case VX_TYPE_ARRAY:
                        {
                            vx_array arr = (vx_array)data[d];
                            fprintf(fp, "\tD%u [shape=box label=\"Array\\n%s\\nItems: %zu\"];\n", d, type_pairs[i].name, arr->capacity);
                            break;
                        }
                        case VX_TYPE_PYRAMID:
                        {
                            vx_pyramid pyr = (vx_pyramid)data[d];
                            fprintf(fp, "\tD%u [shape=triangle label=\"Pyramid\\n%lfx" VX_FMT_REF "\\nLevels: %zu\"];\n",
                                    d, pyr->scale, (void*)pyr->levels, pyr->numLevels);
                            break;
                        }
                        case VX_TYPE_SCALAR:
                        {
                            vx_scalar scalar = (vx_scalar)data[d];
                            vx_enum scalar_type;
                            vxQueryScalar(scalar, VX_SCALAR_TYPE, &scalar_type, sizeof(scalar_type));
                            char value_str[64];
                            switch (scalar_type)
                            {
                                case VX_TYPE_CHAR:
                                {
                                    vx_char value;
                                    vxCopyScalar(scalar, &value, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
                                    snprintf(value_str, sizeof(value_str), "%c", value);
                                    break;
                                }
                                case VX_TYPE_INT8:
                                case VX_TYPE_UINT8:
                                case VX_TYPE_INT16:
                                case VX_TYPE_UINT16:
                                case VX_TYPE_INT32:
                                case VX_TYPE_UINT32:
                                {
                                    vx_int32 value;
                                    vxCopyScalar(scalar, &value, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
                                    snprintf(value_str, sizeof(value_str), "%d", value);
                                    break;
                                }
                                case VX_TYPE_FLOAT32:
                                {
                                    vx_float32 value;
                                    vxCopyScalar(scalar, &value, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
                                    snprintf(value_str, sizeof(value_str), "%.2f", value);
                                    break;
                                }
                                default:
                                    snprintf(value_str, sizeof(value_str), "Unknown");
                            }
                            fprintf(fp, "\tD%u [shape=box label=\"Scalar\\n%s\\nValue: %s\"];\n", d, type_pairs[i].name, value_str);
                            break;
                        }
                        case VX_TYPE_MATRIX:
                        {
                            vx_matrix matrix = (vx_matrix)data[d];
                            fprintf(fp, "\tD%u [shape=box label=\"Matrix\\n%zux%zu\"];\n", d, matrix->columns, matrix->rows);
                            break;
                        }
                        case VX_TYPE_CONVOLUTION:
                        {
                            vx_convolution conv = (vx_convolution)data[d];
                            fprintf(fp, "\tD%u [shape=box label=\"Convolution\\n%zux%zu\"];\n", d, conv->columns, conv->rows);
                            break;
                        }
                        case VX_TYPE_DISTRIBUTION:
                        {
                            vx_distribution dist = (vx_distribution)data[d];
                            vx_size bins = 0;
                            vx_status bin_status = vxQueryDistribution(dist, VX_DISTRIBUTION_BINS, &bins, sizeof(bins));
                            if (bin_status == VX_SUCCESS)
                            {
                                fprintf(fp, "\tD%u [shape=box label=\"Distribution\\nBins: %zu\"];\n", d, bins);
                            }
                            else
                            {
                                fprintf(fp, "\tD%u [shape=box label=\"Distribution\"];\n", d);
                            }
                            break;
                        }
                        case VX_TYPE_LUT:
                        {
                            vx_lut lut = (vx_lut)data[d];
                            fprintf(fp, "\tD%u [shape=box label=\"LUT\\nCount: %zu\"];\n", d, lut->num_items);
                            break;
                        }
                        case VX_TYPE_THRESHOLD:
                        {
                            vx_threshold thresh = (vx_threshold)data[d];
                            fprintf(fp, "\tD%u [shape=box label=\"Threshold\\nType: %d\"];\n", d, thresh->thresh_type);
                            break;
                        }
                        case VX_TYPE_TENSOR:
                        {
                            vx_tensor tensor = (vx_tensor)data[d];
                            fprintf(fp, "\tD%u [shape=box label=\"Tensor\\nRank: %u\"];\n", d, tensor->number_of_dimensions);
                            break;
                        }
                        case VX_TYPE_OBJECT_ARRAY:
                        {
                            vx_object_array obj_array = (vx_object_array)data[d];
                            fprintf(fp, "\tD%u [shape=box label=\"Object Array\\nCount: %zu\"];\n", d, obj_array->num_items);
                            break;
                        }
                        default:
                            fprintf(fp, "\tD%u [shape=box label=\"%s\"];\n", d, type_pairs[i].name);
                            break;
                    }
                }
            }

            graph->clearVisitation();
            graph->clearExecution();
            memcpy(next_nodes, graph->heads, graph->numHeads * sizeof(graph->heads[0]));
            num_next = graph->numHeads;
            num_last = 0;
            num_left = 0;
            do {
                for (n = 0; n < num_next; n++)
                {
                    /* for each head, start tracing the graph */
                    vx_node node = graph->nodes[next_nodes[n]];

                    if (graph->nodes[next_nodes[n]]->executed == vx_true_e) continue;

                    for (p = 0; p < node->kernel->signature.num_parameters; p++)
                    {
                        vx_uint32 count = dimof(next_nodes);

                        if (showData && node->kernel->signature.directions[p] == VX_INPUT)
                        {
                            graph->findNodesWithReference(node->parameters[p],
                                                     nullptr, &count,VX_OUTPUT);
                            if (count > 0) continue;
                            for (d = 0u; d < num_data; d++)
                                if (data[d] == node->parameters[p])
                                    break;
                            if (d == num_data) continue; /* ref not found */
                            fprintf(fp, "\tD%u -> N%u;\n", d, next_nodes[n]);
                        }
                        else if (node->kernel->signature.directions[p] == VX_OUTPUT)
                        {
                            status = graph->findNodesWithReference(
                                                              node->parameters[p],
                                                              dep_nodes,
                                                              &count,
                                                              VX_INPUT);
                            /* printf("N%u has %u dep nodes on parameter[%u], %d\n", next_nodes[n], count, p, status); */
                            for (n2 = 0; status == VX_SUCCESS && n2 < count; n2++)
                            {
                                if (showData)
                                {
                                    for (d = 0u; d < num_data; d++)
                                        if (data[d] == node->parameters[p])
                                            break;
                                    fprintf(fp, "\tN%u -> D%u -> N%u;\n", next_nodes[n], d, dep_nodes[n2]);
                                }
                                else
                                {
                                    fprintf(fp, "\tN%u -> N%u;\n", next_nodes[n], dep_nodes[n2]);
                                }
                            }
                        }
                    }
                }
                memcpy(last_nodes, next_nodes, num_next * sizeof(next_nodes[0]));
                num_last = num_next;
                num_next = 0;
                graph->findNextNodes(last_nodes, num_last, next_nodes, &num_next, left_nodes, &num_left);
            } while (num_next > 0);
            graph->clearVisitation();
            graph->clearExecution();
            fprintf(fp, "}\n");
            fclose(fp);
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to open file for writing!\n");
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a graph!\n");
    }
    return status;
}

// Helper function to trim whitespace from both ends
std::string_view trim(std::string_view sv)
{
    const char* whitespace = " \t\n\r\f\v";
    sv.remove_prefix(sv.find_first_not_of(whitespace));
    sv.remove_suffix(sv.size() - sv.find_last_not_of(whitespace) - 1);
    return sv;
}

VX_API_ENTRY vx_status VX_API_CALL vxImportGraphFromDot(vx_graph graph, vx_char dotfile[], vx_bool acceptData)
{
    vx_status status = VX_SUCCESS;
    constexpr vx_uint32 MAX_REF = VX_INT_MAX_REF;
    vx_node nodes[MAX_REF] = { nullptr };
    vx_uint32 numNodes = 0u;
    vx_reference data[MAX_REF] = { nullptr };
    vx_uint32 numData = 0u;
    std::string line;
    // Regex to capture node lines with format: N<num> [label="N<num>\n<kernelName>"];
    std::regex node_regex{ R"regex(^\s*N(\d+)\s+\[label="N\d+\\n([^"]+)")regex" };
    // Regex to capture data definitions if acceptData is enabled.
    std::regex data_regex{ R"regex(^\s*D(\d+)\s+\[)regex" };
    std::filesystem::path dotPath(dotfile);
    std::ifstream file(dotPath);

    // Verify that the input graph is valid.
    if (Reference::isValidReference(graph, VX_TYPE_GRAPH) != vx_true_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid graph passed in!\n");
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    if (VX_SUCCESS == status)
    {
        if (!file.is_open())
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to open file for reading: %s\n", dotfile);
            status = VX_ERROR_INVALID_VALUE;
        }
    }

    if (VX_SUCCESS == status)
    {
        while (std::getline(file, line))
        {
            std::string_view sv = trim(line);
            if (sv.empty() || sv == "{" || sv == "}")
                continue;

            std::smatch match;
            // Process node definitions.
            if (sv.starts_with("N") && std::regex_search(line, match, node_regex))
            {
                unsigned int index = std::stoi(match[1].str());
                std::string kernelName = match[2].str();

                // Retrieve kernel by name.
                vx_context context = vxGetContext(reinterpret_cast<vx_reference>(graph));
                vx_kernel kernel = vxGetKernelByName(context, kernelName.c_str());
                if (kernel == nullptr || vxGetStatus(reinterpret_cast<vx_reference>(kernel)) != VX_SUCCESS)
                {
                    VX_PRINT(VX_ZONE_ERROR, "Unknown or invalid kernel: %s\n", kernelName.c_str());
                    status = VX_FAILURE;
                    break;
                }

                // Create the node using the retrieved kernel.
                vx_node node = vxCreateGenericNode(graph, kernel);
                // Release external reference to kernel.
                vxReleaseKernel(&kernel);
                if (node == nullptr || vxGetStatus(reinterpret_cast<vx_reference>(node)) != VX_SUCCESS)
                {
                    VX_PRINT(VX_ZONE_ERROR, "Failed to create node for kernel %s\n", kernelName.c_str());
                    status = VX_FAILURE;
                    break;
                }
                if (index < MAX_REF)
                {
                    nodes[index] = node;
                    if (index + 1 > numNodes)
                        numNodes = index + 1;
                }
                continue;
            }

            // Process data definitions if acceptData is true.
            if (acceptData && sv.starts_with("D") && std::regex_search(line, match, data_regex))
            {
                unsigned int dindex = std::stoi(match[1].str());
                // Here we use a dummy pointer as a placeholder.
                // data[dindex] = reinterpret_cast<vx_reference>(0x1);
                if (dindex + 1 > numData)
                    numData = dindex + 1;
                continue;
            }

            // Edge definitions (lines containing "->") are detected.
            if (line.find("->") != std::string::npos)
            {
                std::smatch edge_match;
                // Regex for an edge with data between nodes: N<number> -> D<number> -> N<number>
                std::regex edge_regex_ndn{ R"regex(^\s*N(\d+)\s*->\s*D(\d+)\s*->\s*N(\d+)\s*;?)regex" };
                // Regex for a direct node-to-node edge: N<number> -> N<number>
                std::regex edge_regex_nn{ R"regex(^\s*N(\d+)\s*->\s*N(\d+)\s*;?)regex" };

                if (std::regex_search(line, edge_match, edge_regex_ndn))
                {
                    // unsigned int src_idx = std::stoi(edge_match[1].str());
                    unsigned int d_idx   = std::stoi(edge_match[2].str());
                    unsigned int dst_idx = std::stoi(edge_match[3].str());
                    if (nodes[dst_idx]) // && data[d_idx] != nullptr)
                    {
                        // Find an available input slot in the destination node.
                        vx_kernel dstKernel = nodes[dst_idx]->kernel;
                        vx_uint32 slot = 0;
                        for (; slot < dstKernel->signature.num_parameters; ++slot)
                        {
                            if (dstKernel->signature.directions[slot] == VX_INPUT &&
                                nodes[dst_idx]->parameters[slot] == nullptr)
                            {
                                vx_status setStatus = vxSetParameterByIndex(nodes[dst_idx], slot, data[d_idx]);
                                if (setStatus != VX_SUCCESS)
                                {
                                    VX_PRINT(VX_ZONE_ERROR, "Failed to set parameter at index %u for node N%u\n", slot, dst_idx);
                                }
                                break;
                            }
                        }
                        if (slot == dstKernel->signature.num_parameters)
                        {
                            VX_PRINT(VX_ZONE_ERROR, "No available input slot in node N%u for data edge: %s\n", dst_idx, line.c_str());
                        }
                    }
                    else
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Invalid indices in edge: %s\n", line.c_str());
                    }
                }
                else if (std::regex_search(line, edge_match, edge_regex_nn))
                {
                    unsigned int src_idx = std::stoi(edge_match[1].str());
                    unsigned int dst_idx = std::stoi(edge_match[2].str());
                    if (nodes[dst_idx] && nodes[src_idx])
                    {
                        // Identify the source node's output by searching for the first non-null VX_OUTPUT.
                        vx_kernel srcKernel = nodes[src_idx]->kernel;
                        vx_reference outRef = nullptr;
                        for (vx_uint32 k = 0; k < srcKernel->signature.num_parameters; ++k)
                        {
                            vx_status setStatus = vxSetParameterByIndex(nodes[src_idx], k, nullptr);
                            if (setStatus != VX_SUCCESS)
                            {
                                VX_PRINT(VX_ZONE_ERROR, "Failed to set parameter at index %u for node N%u\n", k, src_idx);
                            }
                            if (srcKernel->signature.directions[k] == VX_OUTPUT &&
                                nodes[src_idx]->parameters[k] != nullptr)
                            {
                                outRef = nodes[src_idx]->parameters[k];
                                break;
                            }
                        }
                        if (outRef == nullptr)
                        {
                            VX_PRINT(VX_ZONE_ERROR, "No output parameter found for source node N%u\n", src_idx);
                        }
                        else
                        {
                            // Find an available input slot in the destination node.
                            vx_kernel dstKernel = nodes[dst_idx]->kernel;
                            vx_uint32 slot = 0;
                            for (; slot < dstKernel->signature.num_parameters; ++slot)
                            {
                                if (dstKernel->signature.directions[slot] == VX_INPUT &&
                                    nodes[dst_idx]->parameters[slot] == nullptr)
                                {
                                    vx_status setStatus = vxSetParameterByIndex(nodes[dst_idx], slot, outRef);
                                    if (setStatus != VX_SUCCESS)
                                    {
                                        VX_PRINT(VX_ZONE_ERROR, "Failed to connect node N%u to N%u at input slot %u\n",
                                                    src_idx, dst_idx, slot);
                                    }
                                    break;
                                }
                            }
                            if (slot == dstKernel->signature.num_parameters)
                            {
                                VX_PRINT(VX_ZONE_ERROR, "No available input slot in node N%u for node edge: %s\n", dst_idx, line.c_str());
                            }
                        }
                    }
                    else
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Invalid node indices in edge: %s\n", line.c_str());
                    }
                }
                else
                {
                    VX_PRINT(VX_ZONE_WARNING, "Unrecognized edge format: %s\n", line.c_str());
                }
                continue;
            }
        }
    }
    file.close();

    VX_PRINT(VX_ZONE_INFO,
        "%s: Imported %u nodes and %u data items.\n",
        __func__, numNodes, numData);

    return status;
}

#endif /* EXPERIMENTAL_USE_DOT */
