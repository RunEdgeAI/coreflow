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

#include "vx_internal.h"
#include "vx_type_pairs.h"

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
                    vx_int32 i = ownStringFromType(data[d]->type);
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
                                    d, pyr->scale, pyr->levels, pyr->numLevels);
                            break;
                        }
                        case VX_TYPE_SCALAR:
                        {
                            vx_scalar scalar = (vx_scalar)data[d];
                            vx_enum scalar_type;
                            vxQueryScalar(scalar, VX_SCALAR_TYPE, &scalar_type, sizeof(scalar_type));
                            char value_str[64];
                            switch (scalar_type) {
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

            ownClearVisitation(graph);
            ownClearExecution(graph);
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
                            ownFindNodesWithReference(graph,node->parameters[p],
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
                            status = ownFindNodesWithReference(graph,
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
                ownFindNextNodes(graph, last_nodes, num_last, next_nodes, &num_next, left_nodes, &num_left);
            } while (num_next > 0);
            ownClearVisitation(graph);
            ownClearExecution(graph);
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

#endif /* EXPERIMENTAL_USE_DOT */
