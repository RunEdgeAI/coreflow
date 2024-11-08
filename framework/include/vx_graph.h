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

#include <VX/vx.h>

#include "vx_reference.h"

/*!
 * \file
 * \brief The internal graph implementation
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_graph Internal Graph API
 * \ingroup group_internal
 * \brief The Internal Graph API
 */

/*! \brief The internal representation of a graph.
 * \ingroup group_int_graph
 */
class Graph : public Reference
{
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
     * @brief Destroy the Graph object
     * @ingroup group_int_graph
     */
    ~Graph();

    /**
     * @brief Destruct function for the Graph object
     * @ingroup group_int_graph
     */
    void destructGraph();

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
    } parameters[VX_INT_MAX_PARAMS];
    /*! \brief The number of graph parameters. */
    vx_uint32      numParams;
    /*! \brief A switch to turn off SMP mode */
    vx_bool        should_serialize;
    /*! \brief [hidden] If non-NULL, the parent graph, for scope handling. */
    vx_graph       parentGraph;
    /*! \brief The array of all delays in this graph */
    vx_delay       delays[VX_INT_MAX_REF];
};

/*! \brief Clears visited flag.
 * \param [in] graph The graph to clear.
 * \ingroup group_int_graph
 */
void ownClearVisitation(vx_graph graph);

/*! \brief Clears execution flag.
 * \param [in] graph The graph to clear.
 * \ingroup group_int_graph
 */
void ownClearExecution(vx_graph graph);

/*! \brief Given a set of last nodes, this function will determine the next
 * set of nodes which are capable of being run. Nodes which are encountered but
 * can't be run will be placed in the left nodes list.
 * \param [in] graph The graph structure.
 * \param [in] last_nodes The last list of nodes executed.
 * \param [in] numLast The number of nodes in the last_nodes list which are valid.
 * \param [out] next_nodes The list of nodes next to be executed.
 * \param [in] numNext The number of nodes in the next_nodes list which are valid.
 * \param [out] left_nodes The list of nodes which are next, but can't be executed.
 * \param [in] numLeft The number of nodes in the left_nodes list which are valid.
 * \ingroup group_int_graph
 */
void ownFindNextNodes(vx_graph graph,
                     vx_uint32 last_nodes[VX_INT_MAX_REF], vx_uint32 numLast,
                     vx_uint32 next_nodes[VX_INT_MAX_REF], vx_uint32 *numNext,
                     vx_uint32 left_nodes[VX_INT_MAX_REF], vx_uint32 *numLeft);

/**
 * @brief Find nodes using this reference as input or output parameter
 *
 * @param graph     The graph to traverse
 * @param ref       The reference to search for
 * @param refnodes  The nodes to search within
 * @param count     Count of nodes found using ref
 * @param reftype   The reference type
 * @return vx_status
 * @ingroup group_int_graph
 */
vx_status ownFindNodesWithReference(vx_graph graph,
                                   vx_reference ref,
                                   vx_uint32 refnodes[],
                                   vx_uint32 *count,
                                   vx_enum reftype);

#endif /* VX_GRAPH_H */
