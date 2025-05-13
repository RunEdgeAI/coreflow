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

#include "vx_internal.h"

#ifdef OPENVX_USE_PIPELINING

static vx_status ownGraphPipelineValidateRefsList(
    const vx_graph_parameter_queue_params_t graph_parameters_queue_param)
{
    vx_status status = (vx_status)VX_SUCCESS;
    vx_status status1 = (vx_status)VX_SUCCESS;
    vx_meta_format meta_base = nullptr, meta = nullptr;
    vx_uint32 i;

    if (nullptr != graph_parameters_queue_param.refs_list[0])
    {
        meta_base = vxCreateMetaFormat(graph_parameters_queue_param.refs_list[0]->context);
        status = vxSetMetaFormatFromReference(meta_base, graph_parameters_queue_param.refs_list[0]);
    }

    if ( (VX_SUCCESS == status)
         && (nullptr != meta_base) )
    {
        for (i = 1; i < graph_parameters_queue_param.refs_list_size; i++)
        {
            if (nullptr != graph_parameters_queue_param.refs_list[i])
            {
                meta = vxCreateMetaFormat(graph_parameters_queue_param.refs_list[i]->context);

                if (nullptr != meta)
                {
                    status = vxSetMetaFormatFromReference(meta, graph_parameters_queue_param.refs_list[i]);
                }
                else
                {
                    status = VX_FAILURE;
                    VX_PRINT(VX_ZONE_ERROR, "Meta Format is NULL\n");
                }

                if (VX_SUCCESS == status)
                {
                    if (graph_parameters_queue_param.refs_list[0]->type ==
                        graph_parameters_queue_param.refs_list[i]->type)
                    {
                        if (vx_true_e != MetaFormat::isMetaFormatEqual(meta_base, meta, graph_parameters_queue_param.refs_list[0]->type))
                        {
                            status = VX_ERROR_INVALID_PARAMETERS;
                            VX_PRINT(VX_ZONE_ERROR, "Invalid meta data of reference list!\n");
                        }
                    }
                }
                else
                {
                    break;
                }

                if (Reference::isValidReference(meta, VX_TYPE_META_FORMAT) == vx_true_e)
                {
                    status1 = vxReleaseMetaFormat(&meta);
                    if (VX_SUCCESS != status1)
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Failed to release meta format object \n");
                        status = status1;
                    }
                }
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
                VX_PRINT(VX_ZONE_ERROR, "Invalid graph parameter ref list!\n");
            }
        }
    }

    if (Reference::isValidReference(meta_base, VX_TYPE_META_FORMAT) == vx_true_e)
    {
        status1 = vxReleaseMetaFormat(&meta_base);
        if (VX_SUCCESS != status1)
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to release meta format object \n");
            status = status1;
        }
    }

    return status;
}

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
        if (graph_schedule_mode == VX_GRAPH_SCHEDULE_MODE_NORMAL)
        {
            graph->scheduleMode = (vx_graph_schedule_mode_type_e)graph_schedule_mode;
        }
        else if (((graph_schedule_mode == VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO) ||
                    (graph_schedule_mode == VX_GRAPH_SCHEDULE_MODE_QUEUE_MANUAL)) &&
                    (graph_parameters_list_size <= graph->numParams))
        {
            graph->scheduleMode = (vx_graph_schedule_mode_type_e)graph_schedule_mode;

            /* Pipelining is enabled */
            // graph->isPipeliningEnabled = vx_true_e;

            for (vx_uint32 i = 0; (i < graph_parameters_list_size) && (status == VX_SUCCESS); i++)
            {
                if (graph_parameters_queue_params_list[i].refs_list != nullptr)
                {
                    if ((graph_parameters_queue_params_list[i].graph_parameter_index >= graph->numParams) ||
                        (graph_parameters_queue_params_list[i].refs_list_size >= VX_OBJ_DESC_QUEUE_MAX_DEPTH))
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Invalid parameters\n");
                        status = VX_ERROR_INVALID_PARAMETERS;
                    }
                    else
                    {
                        graph->parameters[i].numBufs = graph_parameters_queue_params_list[i].refs_list_size;
                        graph->parameters[i].type = graph_parameters_queue_params_list[i].refs_list[0]->type;

                        status = ownGraphPipelineValidateRefsList(graph_parameters_queue_params_list[i]);

                        if (VX_SUCCESS == status)
                        {
                            for (vx_uint32 buf_id = 0; buf_id < graph->parameters[i].numBufs; buf_id++)
                            {
                                graph->parameters[i].refs_list[buf_id] = graph_parameters_queue_params_list[i].refs_list[buf_id];
                            }
                        }
                        else
                        {
                            VX_PRINT(VX_ZONE_ERROR,
                                        "Graph parameter refs list at index %d contains "
                                        "inconsistent meta data. Please ensure that all buffers "
                                        "in list contain the same meta data\n",
                                        i);
                        }
                    }
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                    VX_PRINT(VX_ZONE_ERROR,
                                "Invalid parameters: graph_parameters_queue_params_list at index "
                                "%d is NULL\n",
                                i);
                    break;
                }
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR,
                        "user parameter list (%d) > number of graph parameters (%d)\n",
                        graph_parameters_list_size, graph->numParams);
            status = VX_ERROR_INVALID_PARAMETERS;
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
            if (!paramQueue.enqueue(refs[i]))
            {
                status = VX_ERROR_NO_RESOURCES; // queue full
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
        graph_parameter_index >= graph->numParams ||
        !refs ||
        !num_refs)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    if (VX_SUCCESS == status)
    {
        auto& paramQueue = graph->parameters[graph_parameter_index].queue;

        while (count < max_refs && paramQueue.dequeue(ref))
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
        graph_parameter_index >= graph->numParams ||
        !num_refs)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    if (VX_SUCCESS == status)
    {
        auto& paramQueue = graph->parameters[graph_parameter_index].queue;
        *num_refs = static_cast<vx_uint32>(paramQueue.size());
    }

    return status;
}

#endif /* OPENVX_USE_PIPELINING */
