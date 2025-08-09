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
#include <vector>

/*!
 * \file
 * \brief The internal node implementation.
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
 * \param [in] node The node.
 * \param [in] graph The child graph.
 * \retval VX_ERROR_INVALID_GRAPH The Graph's parameters do not match the Node's
 * parameters.
 * \ingroup group_int_node
 */
vx_status vxSetChildGraphOfNode(vx_node node, vx_graph graph);

/*! \brief Retrieves the handle of the child graph, if it exists.
 * \param [in] node The node.
 * \return Returns the handle of the child graph or zero if it doesn't have one.
 * \ingroup group_int_node
 */
vx_graph vxGetChildGraphOfNode(vx_node node);

/*! \brief The internal representation of a node.
 * \ingroup group_int_node
 */
namespace corevx {

class Node : public Reference
{
public:
    /**
     * @brief Construct a new Node object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_node
     */
    Node(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Node object
     * @ingroup group_int_node
     */
    ~Node();

    /**
     * @brief Create a new node
     *
     * @param graph   The graph associated with this node
     * @param kernel  The kernel associated with this node
     * @return vx_node The node object
     * @ingroup group_int_node
     */
    static vx_node createNode(vx_graph graph, vx_kernel kernel);

    /*! \brief Node parameter setter, no check.
     * \ingroup group_int_node
     */
    void setParameter(vx_uint32 index, vx_reference value);

    /**
     * @brief Set the target for the node
     *
     * @param target_enum The target enum to set
     * @param target_string The target string to set
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_node
     */
    vx_status setTarget(vx_enum target_enum, const char *target_string);

    /**
     * @brief Get the local data size of the node
     *
     * @return vx_size The local data size
     * @ingroup group_int_node
     */
    vx_size localDataSize() const;

    /**
     * @brief Get the local data pointer of the node
     *
     * @return vx_ptr_t The local data pointer
     * @ingroup group_int_node
     */
    vx_ptr_t localDataPtr() const;

    /**
     * @brief Get the global data size of the node
     *
     * @return vx_size The global data size
     * @ingroup group_int_node
     */
    vx_size globalDataSize() const;

    /**
     * @brief Get the global data pointer of the node
     *
     * @return vx_ptr_t The global data pointer
     * @ingroup group_int_node
     */
    vx_ptr_t globalDataPtr() const;

    /**
     * @brief Get the border mode of the node
     *
     * @return vx_border_t The border mode
     * @ingroup group_int_node
     */
    vx_border_t border() const;

    /**
     * @brief Get the number of parameters of the node
     *
     * @return vx_uint32 The number of parameters
     * @ingroup group_int_node
     */
    vx_uint32 numParams() const;

    /**
     * @brief Get the performance of the node
     *
     * @return vx_perf_t The performance metrics
     * @ingroup group_int_node
     */
    vx_perf_t performance() const;

    /**
     * @brief Get the status of the node
     *
     * @return vx_status The status of the node
     * @ingroup group_int_node
     */
    vx_status getStatus() const;

    /**
     * @brief Set the Node Callback Fn object
     *
     * @param callback   The callback function to set
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_node
     */
    vx_status setCallbackFn(vx_nodecomplete_f callback);

    /**
     * @brief Is the node replicated?
     *
     * @return vx_bool True if the node is replicated, false otherwise
     * @ingroup group_int_node
     */
    vx_bool isReplicated() const;

    /**
     * @brief Get the replicated parameters flags
     *
     * @return const vx_bool* The array of flags indicating which parameters are replicated
     * @ingroup group_int_node
     */
    const vx_bool *replicatedFlags() const;

    /**
     * @brief Get the state of the node
     *
     * @return vx_enum The state of the node
     * @ingroup group_int_node
     */
    vx_enum getState() const;

    /**
     * @brief Get the valid rectangle reset flag
     *
     * @return vx_bool The valid rectangle reset flag
     * @ingroup group_int_node
     */
    vx_bool validRectReset() const;

    /**
     * @brief Get the OpenCL command queue for the node
     *
     * @return cl_command_queue The OpenCL command queue
     * @ingroup group_int_node
     */
    cl_command_queue clCommandQueue() const;

    /**
     * @brief Get the callback function for the node
     *
     * @return vx_nodecomplete_f The callback function
     * @ingroup group_int_node
     */
    vx_nodecomplete_f callbackFn() const;

    /**
     * @brief Replicate the node in the graph
     *
     * @param graph                The graph to replicate the node in
     * @param first_node           The first node to replicate
     * @param replicate            The array of flags indicating which parameters to replicate
     * @param number_of_parameters The number of parameters to replicate
     * @return vx_status           VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_node
     */
    static vx_status replicateNode(vx_graph graph, vx_node first_node, vx_bool *replicate,
                                   vx_uint32 number_of_parameters);

    /*! \brief Used to set the graph as a child of the node within another graph.
     * \param [in] node The node.
     * \param [in] graph The child graph.
     * \retval VX_ERROR_INVALID_GRAPH The Graph's parameters do not match the Node's
     * parameters.
     * \ingroup group_int_node
     */
    static vx_status setChildGraphOfNode(vx_node node, vx_graph graph);

    /*! \brief Retrieves the handle of the child graph, if it exists.
     * \param [in] node The node.
     * \return Returns the handle of the child graph or zero if it doesn't have one.
     * \ingroup group_int_node
     */
    static vx_graph getChildGraphOfNode(vx_node node);

    /*! \brief Used to completely destroy a node.
     * \ingroup group_int_node
     */
    void destruct() override final;

    /*! \brief Used to remove a node from a graph.
     * \ingroup group_int_node
     * \return A <tt>\ref vx_status_e</tt> enumeration.
     * \retval VX_SUCCESS No errors.
     * \retval VX_ERROR_INVALID_REFERENCE If *n is not a <tt>\ref vx_node</tt>.
     * \ingroup group_int_node
     */
    vx_status removeNode();

    /**
     * @brief Print node object
     *
     * @param node
     * @ingroup group_int_node
     */
    static void printNode(vx_node node);

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
    /*! \brief The node state */
    vx_node_state_e     state;
};

} // namespace corevx

#endif /* VX_NODE_H */