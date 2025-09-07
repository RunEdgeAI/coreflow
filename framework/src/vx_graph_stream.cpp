/*
 * Copyright (c) 2012-2019 The Khronos Group Inc.
 *
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
#include <VX/vx.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_khr_pipelining.h>

#include "vx_internal.h"

using namespace coreflow;

#ifdef OPENVX_USE_STREAMING

VX_API_ENTRY vx_status VX_API_CALL vxEnableGraphStreaming(vx_graph graph,
                vx_node trigger_node)
{
    vx_status status = VX_SUCCESS;
    vx_bool triggerNodeSet = vx_false_e;

    if (vx_false_e == Reference::isValidReference(graph, VX_TYPE_GRAPH))
    {
        VX_PRINT(VX_ZONE_ERROR, "invalid graph reference\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        graph->isStreamingEnabled = vx_true_e;

        if (vx_true_e == Reference::isValidReference(trigger_node, VX_TYPE_NODE))
        {
            for (vx_uint32 i = 0; i < graph->numNodes; i++)
            {
                if (graph->nodes[i] == trigger_node)
                {
                    graph->triggerNodeIndex = i;
                    triggerNodeSet = vx_true_e;
                    break;
                }
            }

            if (vx_false_e == triggerNodeSet)
            {
                VX_PRINT(VX_ZONE_ERROR, "trigger_node does not belong to graph\n");
                status = VX_ERROR_INVALID_PARAMETERS;
            }
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxStartGraphStreaming(vx_graph graph)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Reference::isValidReference(graph, VX_TYPE_GRAPH))
    {
        VX_PRINT(VX_ZONE_ERROR, "invalid graph reference\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        if (vx_true_e != graph->isStreamingEnabled)
        {
            VX_PRINT(VX_ZONE_ERROR,
                     "streaming has not been enabled. Please enable streaming prior to verifying "
                     "graph\n");
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if (VX_SUCCESS == status)
    {
        if (vx_false_e == graph->isStreaming)
        {
            graph->isStreaming = vx_true_e;
            graph->streamingThread = std::thread([graph]() { graph->streamingLoop(); });
            VX_PRINT(VX_ZONE_INFO, "Graph streaming thread started\n");
        }
        else
        {
            VX_PRINT(VX_ZONE_WARNING, "this graph is currently already streaming\n");
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxStopGraphStreaming(vx_graph graph)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Reference::isValidReference(graph, VX_TYPE_GRAPH))
    {
        VX_PRINT(VX_ZONE_ERROR, "invalid graph reference\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        if (vx_true_e != graph->isStreaming)
        {
            VX_PRINT(VX_ZONE_ERROR, "Streaming has not been started\n");
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if (VX_SUCCESS == status)
    {
        // First set streaming to false to stop the loop
        graph->isStreaming = vx_false_e;

        // Wait up to 5 seconds for the thread to finish
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Wait for any pending graph executions to complete
        vxWaitGraph(graph);

        // Wait and join streaming thread with a timeout
        if (graph->streamingThread.joinable())
        {
            // If thread is still running, force join
            graph->streamingThread.join();

            VX_PRINT(VX_ZONE_INFO, "Graph streaming joined\n");
        }

        // Reset streaming state
        graph->isStreamingEnabled = vx_false_e;
        graph->triggerNodeIndex = UINT32_MAX;

        // Reset node states
        for (vx_uint32 i = 0; i < graph->numNodes; i++)
        {
            if (graph->nodes[i] != nullptr)
            {
                graph->nodes[i]->state = VX_NODE_STATE_STEADY;
                graph->nodes[i]->executed = vx_false_e;
            }
        }
    }

    return status;
}

#endif /* OPENVX_USE_STREAMING */
