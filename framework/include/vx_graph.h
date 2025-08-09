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

#ifndef VX_GRAPH_H
#define VX_GRAPH_H

#include <COREVX/execution_queue.hpp>
#include <atomic>

#include "vx_internal.h"
#include "vx_reference.h"

/*!
 * \file
 * \brief The internal graph implementation
 *
 * \defgroup group_int_graph Internal Graph API
 * \ingroup group_internal
 * \brief The Internal Graph API
 */

/*! \brief The internal representation of a graph.
 * \ingroup group_int_graph
 */
namespace corevx {

class Graph : public Reference
{
private:
    /**
     * @brief Get the next node index given current node index
     *
     * @param index      The current node index .
     * @return vx_uint32 The next node index
     * @ingroup group_int_graph
     */
    vx_uint32 nextNode(vx_uint32 index);

    /**
     * @brief Locate reference given base location
     *
     * @param ref               Start reference.
     * @param start             Start location.
     * @param end               End location.
     * @return vx_reference     vx_reference if found, otherwise nullptr
     * @ingroup group_int_graph
     */
    static vx_reference locateBaseLocation(vx_reference ref, vx_size* start, vx_size* end);

    /**
     * @brief Locate tensor within view
     *
     * @param mddata      Tensor metadata.
     * @param start       Start view.
     * @param end         End view.
     * @return vx_tensor  vx_tensor if found, othewise nullptr
     * @ingroup group_int_graph
     */
    static vx_tensor locateView(vx_tensor mddata, vx_size* start, vx_size* end);

    /**
     * @brief Check write dependency between two references
     *
     * @param ref1      First reference to compare.
     * @param ref2      Second reference to compare.
     * @return vx_bool  vx_true_e if write dependency exists, otherwise vx_false_e
     * @ingroup group_int_graph
     */
    static vx_bool checkWriteDependency(vx_reference ref1, vx_reference ref2);

    /**
     * @brief Scan the entire context for graphs which may contain
     * this reference and mark them as unverified.
     *
     * @param ref   The reference to scan for.
     * @ingroup group_int_graph
     */
    static void contaminateGraphs(vx_reference ref);

    /**
     * @brief Destroy the Graph object
     * @ingroup group_int_graph
     */
    ~Graph();
public:
    /**
     * @brief Construct a new Graph object
     *
     * @param context   The context associated with this graph
     * @param scope     Parent reference of this graph
     * @ingroup group_int_graph
     */
    Graph(vx_context context, vx_reference scope);

    /**
     * @brief Create a graph
     *
     * @param context   The context associated with this graph
     * @return vx_graph The graph object
     * @ingroup group_int_graph
     */
    static vx_graph createGraph(vx_context context);

    /**
     * @brief Do a topological in-place sort of the nodes in list, with current
     * order maintained between independent nodes.
     *
     * @param list      list of nodes
     * @param nnodes    number of nodes
     * @ingroup group_int_graph
     */
    void topologicalSort(vx_node *list, vx_uint32 nnodes);

    /**
     * @brief Setup output
     *
     * @param n             index of node
     * @param p             index of parameter
     * @param vref          reference of the parameter
     * @param meta          parameter meta info
     * @param status        status
     * @param num_errors    num errors
     * @return vx_bool
     * @ingroup group_int_graph
     */
    vx_bool setupOutput(vx_uint32 n, vx_uint32 p, vx_reference* vref, vx_meta_format* meta,
                            vx_status* status, vx_uint32* num_errors);

    /**
     * @brief Post process output data type
     *
     * @param n            index of node
     * @param p            index of parameter
     * @param item         item to post process
     * @param vref         reference of the parameter
     * @param meta         parameter meta info
     * @param status       status
     * @param num_errors   num errors
     * @return vx_bool
     * @ingroup group_int_graph
     */
    vx_bool postprocessOutputDataType(vx_uint32 n, vx_uint32 p, vx_reference* item, vx_reference* vref, vx_meta_format meta,
                                  vx_status* status, vx_uint32* num_errors);

    /**
     * @brief Post process output
     *
     * @param n            index of node
     * @param p            index of parameter
     * @param vref         reference of the parameter
     * @param meta         parameter meta info
     * @param status       status
     * @param num_errors   num errors
     * @return vx_bool
     * @ingroup group_int_graph
     */
    vx_bool postprocessOutput(vx_uint32 n, vx_uint32 p, vx_reference* vref, vx_meta_format meta,
                                  vx_status* status, vx_uint32* num_errors);

    /*! \brief Clears visited flag.
     * \ingroup group_int_graph
     */
    void clearVisitation();

    /*! \brief Clears execution flag.
     * \ingroup group_int_graph
     */
    void clearExecution();

    /**
     * @brief Find nodes using this reference as input or output parameter.
     * This function starts on the next node in the list and loops until we
     * hit the original node again. Parse over the nodes in circular fashion.
     * @param ref       The reference to search for
     * @param refnodes  The nodes to search within
     * @param count     Count of nodes found using ref
     * @param reftype   The reference type
     * @return vx_status
     * @ingroup group_int_graph
     */
    vx_status findNodesWithReference(
                                    vx_reference ref,
                                    vx_uint32 refnodes[],
                                    vx_uint32 *count,
                                    vx_enum reftype);

    /*! \brief Given a set of last nodes, this function will determine the next
     * set of nodes which are capable of being run. Nodes which are encountered but
     * can't be run will be placed in the left nodes list.
     * \param [in] last_nodes The last list of nodes executed.
     * \param [in] numLast The number of nodes in the last_nodes list which are valid.
     * \param [out] next_nodes The list of nodes next to be executed.
     * \param [in] numNext The number of nodes in the next_nodes list which are valid.
     * \param [out] left_nodes The list of nodes which are next, but can't be executed.
     * \param [in] numLeft The number of nodes in the left_nodes list which are valid.
     * \ingroup group_int_graph
     */
    void findNextNodes(vx_uint32 last_nodes[VX_INT_MAX_REF], vx_uint32 numLast,
                       vx_uint32 next_nodes[VX_INT_MAX_REF], vx_uint32 *numNext,
                       vx_uint32 left_nodes[VX_INT_MAX_REF], vx_uint32 *numLeft);

    /**
     * @brief Traverse graph
     *
     * @param parentIndex
     * @param childIndex
     * @return vx_status
     * @ingroup group_int_graph
     */
    vx_status traverseGraph(vx_uint32 parentIndex,
                            vx_uint32 childIndex);

    /**
     * @brief Get the graph performance
     *
     * @return vx_perf_t The performance structure
     * @ingroup group_int_graph
     */
    vx_perf_t performance() const;

    /**
     * @brief Get the Graph state
     *
     * @return vx_enum The graph state
     * @ingroup group_int_graph
     */
    vx_enum getState() const;

    /**
     * @brief Get the number of nodes in the graph
     *
     * @return vx_uint32 The number of nodes in the graph
     * @ingroup group_int_graph
     */
    vx_uint32 getNumNodes() const;

    /**
     * @brief Get the number of parameters of the graph
     *
     * @return vx_uint32 The number of graph parameters
     * @ingroup group_int_graph
     */
    vx_uint32 getNumParams() const;

    /**
     * @brief Is graph verified
     *
     * @return vx_bool true if verified, false otherwise
     * @ingroup group_int_graph
     */
    vx_bool isVerified();

    /**
     * @brief Verify the graph
     *
     * @return vx_status VX_SUCCESS if successful, otherwise an return status with error code.
     * @ingroup group_int_graph
     */
    vx_status verify();

    /**
     * @brief Execute the graph
     *
     * @param depth      Optional count of executions
     * @return vx_status VX_SUCCESS if successful, otherwise return status with error code.
     * @ingroup group_int_graph
     */
    vx_status executeGraph(vx_uint32 depth);

    /**
     * @brief Schedule the graph
     *
     * @return vx_status VX_SUCCESS if successful, otherwise return status with error code.
     * @ingroup group_int_graph
     */
    vx_status schedule();

    /**
     * @brief Wait on the graph to complete
     *
     * @return vx_status VX_SUCCESS if successful, otherwise return status with error code.
     * @ingroup group_int_graph
     */
    vx_status wait();

    /**
     * @brief Process the graph
     *
     * @return vx_status VX_SUCCESS if successful, otherwise return status with error code.
     * @ingroup group_int_graph
     */
    vx_status processGraph();

    /**
     * @brief Add a graph paramter
     *
     * @param param      The parameter to add to the graph.
     * @return vx_status VX_SUCCESS if successful, otherwise a status with error code.
     * @ingroup group_int_graph
     */
    vx_status addParameter(vx_parameter param);

    /**
     * @brief Set the graph parameter by index
     *
     * @param index      The graph parameter index
     * @param value      The reference to set
     * @return vx_status VX_SUCCESS on success, otherwise an error
     * @ingroup group_int_graph
     */
    vx_status setParameterByIndex(vx_uint32 index, vx_reference value);

    /**
     * @brief Get the parameter object by index
     *
     * @param index         The graph parameter index
     * @return vx_parameter A valid parameter object on success
     * @ingroup group_int_graph
     */
    vx_parameter getParameterByIndex(vx_uint32 index);

    /**
     * @brief Validate the graph parameters queue references list
     *
     * @param graph_parameters_queue_param
     * @return vx_status
     * @ingroup group_int_graph
     */
    vx_status pipelineValidateRefsList(
        const vx_graph_parameter_queue_params_t graph_parameters_queue_param);

    /**
     * @brief Streaming loop function
     *
     */
    void streamingLoop();

    /**
     * @brief Destruct function for the Graph object
     * @ingroup group_int_graph
     */
    void destruct() override final;

    /*! \brief The array of all nodes in this graph */
    vx_node        nodes[VX_INT_MAX_REF];
    /*! \brief The performance logging variable. */
    vx_perf_t      perf;
    /*! \brief The number of nodes actively allocated in this graph. */
    vx_uint32      numNodes;
    /*! \brief The array of all starting node indexes in the graph. */
    vx_uint32      heads[VX_INT_MAX_REF];
    /*! \brief The number of all nodes in heads list */
    vx_uint32      numHeads;
    /*! \brief The state of the graph (vx_graph_state_e) */
    vx_enum        state;
    /*! \brief This indicates that the graph has been verified. */
    vx_bool        verified;
    /*! \brief This indicates that the graph has been verified earlier, but invalidated latter and is need for verification again. */
    vx_bool        reverify;
    /*! \brief This lock is used to prevent multiple schedulings (data overwrite) */
    vx_sem_t       lock;
    /*! \brief The list of graph parameters. */
    struct {
        /*! \brief The reference to the node which has the parameter */
        vx_node node;
        /*! \brief The index to the parameter on the node. */
        vx_uint32  index;
#ifdef OPENVX_USE_PIPELINING
        /*! \brief Set to an enum value in \ref vx_type_e. */
        vx_enum type;
        /*! \brief the max buffers that can be enqueued */
        vx_uint32 numBufs;
        /*! \brief The internal data ref queue */
        ExecutionQueue<vx_reference, VX_INT_MAX_PARAM_QUEUE_DEPTH> queue;
        /*! \brief references that can be queued into data ref queue */
        vx_reference refs_list[VX_INT_MAX_PARAM_QUEUE_DEPTH];
#endif
    } parameters[VX_INT_MAX_PARAMS];
    /*! \brief The number of graph parameters. */
    vx_uint32      numParams;
    /*! \brief A switch to turn off SMP mode */
    vx_bool        shouldSerialize;
    /*! \brief [hidden] If non-NULL, the parent graph, for scope handling. */
    vx_graph       parentGraph;
    /*! \brief The array of all delays in this graph */
    vx_delay delays[VX_INT_MAX_REF];
#ifdef OPENVX_USE_PIPELINING
    /*! \brief The number of enqueable parameters */
    vx_uint32 numEnqueableParams;
    /*! \brief The number of times to schedule a graph  */
    vx_size scheduleCount;
#endif
#ifdef OPENVX_USE_STREAMING
    /*! \brief This indicates that the graph is streaming enabled */
    std::atomic<vx_bool> isStreamingEnabled;
    /*! \brief This indicates that the graph is currently streaming */
    std::atomic<vx_bool> isStreaming;
    /*! \brief The index of the trigger node */
    vx_uint32 triggerNodeIndex;
    /*! \brief The thread used for streaming */
    vx_thread streamingThread;
#endif
    /*! \brief The graph scheduling mode */
    vx_graph_schedule_mode_type_e scheduleMode;
};

} // namespace corevx

#endif /* VX_GRAPH_H */