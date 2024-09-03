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

#ifndef VX_NODE_H
#define VX_NODE_H

#include <VX/vx.h>

/*!
 * \file
 * \brief The internal node implementation.
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_node Internal Node API
 * \ingroup group_internal
 * \brief The internal Node API.
 */

/*! \brief The internal node attributes
 * \ingroup group_int_node
 */
enum vx_node_attribute_internal_e {
    /*\brief The attribute used to get the pointer to tile local memory */
    VX_NODE_ATTRIBUTE_TILE_MEMORY_PTR = VX_ATTRIBUTE_BASE(VX_ID_KHRONOS, VX_TYPE_NODE) + 0xD,
};

/*! \brief Used to set the graph as a child of the node within another graph.
 * \param [in] n The node.
 * \param [in] g The child graph.
 * \retval VX_ERROR_INVALID_GRAPH The Graph's parameters do not match the Node's
 * parameters.
 * \ingroup group_int_node
 */
vx_status ownSetChildGraphOfNode(vx_node node, vx_graph graph);

/*! \brief Retrieves the handle of the child graph, if it exists.
 * \param [in] n The node.
 * \return Returns the handle of the child graph or zero if it doesn't have one.
 * \ingroup group_int_node
 */
vx_graph ownGetChildGraphOfNode(vx_node node);

/*! \brief The internal representation of a node.
 * \ingroup group_int_node
 */
class Node : public Reference
{
public:
    Node(vx_context context, vx_reference scope);
    ~Node() = default;

    void setParameter(vx_uint32 index, vx_reference value);

    /*! \brief The pointer to the kernel structure */
    vx_kernel           kernel;
    /*! \brief The list of references which are the values to pass to the kernels */
    vx_reference        parameters[VX_INT_MAX_PARAMS];
    /*! \brief Status code returned from the last execution of the kernel. */
    vx_status           status;
    /*! \brief The performance logging variable */
    vx_perf_t           perf;
    /*! \brief A callback to call when the node is complete */
    vx_nodecomplete_f   callback;
    /*! \brief Enable to change read-only attributes (VX_NODE_LOCAL_DATA_SIZE, VX_NODE_LOCAL_DATA_PTR) for user-kernel initialization callback needs. */
    vx_bool             local_data_change_is_enabled;
    /*! \brief Indicates that attributes (VX_NODE_LOCAL_DATA_SIZE, VX_NODE_LOCAL_DATA_PTR) were set by implementation, but not user-kernel initialization callback. */
    vx_bool             local_data_set_by_implementation;
    /*! \brief A back reference to the parent graph. */
    vx_graph            graph;
    /*! \brief Used to keep track of visitation during execution and verification. */
    vx_bool             visited;
    /*! \brief This is used to determine if the node has executed. */
    vx_bool             executed;
    /*! \brief The instance version of the attributes of the kernel */
    vx_kernel_attr_t    attributes;
    /*! \brief The index of the target to execute the kernel on */
    vx_uint32           affinity;
    /*! \brief The child graph of the node. */
    vx_graph            child;
    /*! \brief The node cost factors */
    vx_cost_factors_t   costs;
    /*! \brief The node replica flag */
    vx_bool             is_replicated;
    /*! \brief The replicated parameters flags */
    vx_bool             replicated_flags[VX_INT_MAX_PARAMS];
};

#endif /* VX_NODE_H */
