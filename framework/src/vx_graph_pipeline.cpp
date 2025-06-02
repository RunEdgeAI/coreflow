/*
 * Copyright (c) 2012-2017 The Khronos Group Inc.
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

#include <algorithm>

#include "vx_internal.h"

#ifdef OPENVX_USE_PIPELINING

VX_API_ENTRY vx_status vxSetGraphScheduleConfig(
    vx_graph graph,
    vx_enum graph_schedule_mode,
    vx_uint32 graph_parameters_list_size,
    const vx_graph_parameter_queue_params_t graph_parameters_queue_params_list[]
    )
{
    vx_status status = VX_SUCCESS;

    if (vx_true_e != Reference::isValidReference(graph, VX_TYPE_GRAPH))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid reference\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        if (graph->verified == vx_true_e)
        {
            VX_PRINT(VX_ZONE_ERROR, "Not supported on verified graph\n");
            status = VX_ERROR_NOT_SUPPORTED;
        }
    }

    if (VX_SUCCESS == status)
    {
        if ((graph_schedule_mode < VX_GRAPH_SCHEDULE_MODE_NORMAL) ||
            (graph_schedule_mode > VX_GRAPH_SCHEDULE_MODE_QUEUE_MANUAL))
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid graph schedule mode\n");
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if (VX_SUCCESS == status)
    {
        if (graph_parameters_list_size > graph->numParams)
        {
            VX_PRINT(VX_ZONE_ERROR,
                        "user parameter list (%d) > number of graph parameters (%d)\n",
                        graph_parameters_list_size, graph->numParams);
            status = VX_ERROR_INVALID_PARAMETERS;
        }
        /* Set the number of params that can possibly be enqueued */
        graph->numEnqueableParams = graph_parameters_list_size;
    }

    if (VX_SUCCESS == status)
    {
        graph->scheduleMode = (vx_graph_schedule_mode_type_e)graph_schedule_mode;

        if (((graph_schedule_mode == VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO) ||
             (graph_schedule_mode == VX_GRAPH_SCHEDULE_MODE_QUEUE_MANUAL)))
        {
            for (vx_uint32 i = 0; (i < graph_parameters_list_size) && (status == VX_SUCCESS); i++)
            {
                if ((graph_parameters_queue_params_list[i].refs_list == nullptr) ||
                    (graph_parameters_queue_params_list[i].graph_parameter_index >=
                     graph->numParams) ||
                    (graph_parameters_queue_params_list[i].refs_list_size >=
                     VX_INT_MAX_QUEUE_DEPTH))
                {
                    VX_PRINT(VX_ZONE_ERROR,
                        "Invalid parameters: graph_parameters_queue_params_list at index "
                        "%d is NULL\n",
                        i);
                    status = VX_ERROR_INVALID_PARAMETERS;
                    break;
                }

                // Validate the refs list types
                if (VX_SUCCESS != graph->pipelineValidateRefsList(graph_parameters_queue_params_list[i]))
                {
                    VX_PRINT(VX_ZONE_ERROR,
                                "Graph parameter refs list at index %d contains "
                                "inconsistent meta data. Please ensure that all buffers "
                                "in list contain the same meta data\n",
                                i);
                    status = VX_ERROR_INVALID_PARAMETERS;
                    break;
                }

                // Associate refs with the graph parameter
                graph->parameters[i].numBufs = graph_parameters_queue_params_list[i].refs_list_size;
                graph->parameters[i].type = graph_parameters_queue_params_list[i].refs_list[0]->type;

                for (vx_uint32 buf_id = 0; buf_id < graph->parameters[i].numBufs; buf_id++)
                {
                    graph->parameters[i].refs_list[buf_id] = graph_parameters_queue_params_list[i].refs_list[buf_id];
                }
            }
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxGraphParameterEnqueueReadyRef(vx_graph graph,
                vx_uint32 graph_parameter_index,
                vx_reference *refs,
                vx_uint32 num_refs)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Reference::isValidReference(graph) ||
        graph_parameter_index >= graph->numParams ||
        !refs ||
        num_refs == 0)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    if (VX_SUCCESS == status)
    {
        auto& paramQueue = graph->parameters[graph_parameter_index].queue;

        for (vx_uint32 i = 0; i < num_refs; ++i)
        {
            // Sanity check to validate the reference metadata
            if (!Reference::isValidReference(refs[i], graph->parameters[graph_parameter_index].type))
            {
                VX_PRINT(VX_ZONE_ERROR, "Invalid reference metadata for graph parameter %u\n", graph_parameter_index);
                status = VX_ERROR_INVALID_PARAMETERS;
            }

            if (VX_SUCCESS == status && !paramQueue.enqueuePending(refs[i]))
            {
                status = VX_ERROR_NO_RESOURCES;
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        vx_bool readyToSchedule = vx_true_e;
        vx_uint32 numParams = std::min(graph->numParams, graph->numEnqueableParams);

        while (readyToSchedule)
        {
            for (vx_uint32 i = 0; i < numParams; ++i)
            {
                if (graph->parameters[i].queue.pendingQueueSize() == 0)
                {
                    readyToSchedule = vx_false_e;
                    break;
                }
            }

            if (readyToSchedule)
            {
                for (vx_uint32 i = 0; i < numParams; ++i)
                {
                    graph->parameters[i].queue.movePendingToReady();
                }

                if (graph->scheduleMode == VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO)
                {
                    /* Schedule the graph */
                    status = vxScheduleGraph(graph);
                }
            }
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxGraphParameterDequeueDoneRef(vx_graph graph,
            vx_uint32 graph_parameter_index,
            vx_reference *refs,
            vx_uint32 max_refs,
            vx_uint32 *num_refs)
{
    vx_status status = VX_SUCCESS;
    vx_uint32 count = 0;
    vx_reference ref;

    if (vx_false_e == Reference::isValidReference(graph) ||
        graph_parameter_index >= graph->numEnqueableParams || !refs || !num_refs)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    if (VX_SUCCESS == status)
    {
        auto& paramQueue = graph->parameters[graph_parameter_index].queue;

        // Block until at least one "done" reference is available
        paramQueue.waitForDoneRef();

        while (count < max_refs && paramQueue.doneQueueSize() > 0 && paramQueue.dequeueDone(ref))
        {
            refs[count++] = ref;
        }
        *num_refs = count;

        status = (count > 0) ? VX_SUCCESS : VX_FAILURE;
    }

    return status;
}


VX_API_ENTRY vx_status VX_API_CALL vxGraphParameterCheckDoneRef(vx_graph graph,
            vx_uint32 graph_parameter_index,
            vx_uint32 *num_refs)
{
    vx_status status = VX_SUCCESS;

    if (vx_false_e == Reference::isValidReference(graph) ||
        graph_parameter_index >= graph->numEnqueableParams || !num_refs)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    if (VX_SUCCESS == status)
    {
        auto& paramQueue = graph->parameters[graph_parameter_index].queue;
        *num_refs = static_cast<vx_uint32>(paramQueue.doneQueueSize());
    }

    return status;
}

#endif /* OPENVX_USE_PIPELINING */
