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
#include "vx_graph.h"

#include <algorithm>
#include <queue>

#include "vx_internal.h"

static vx_value_set_t graph_queue[500000];
static vx_size numGraphsQueued = 0ul;

/******************************************************************************/
/* STATIC FUNCTIONS */
/******************************************************************************/
static vx_uint32 vxNextNode(vx_graph graph, vx_uint32 index)
{
    return ((index + 1) % graph->numNodes);
}

static vx_reference vxLocateBaseLocation(vx_reference ref, vx_size* start, vx_size* end)
{
    if (ref->type == VX_TYPE_IMAGE)
    {
        start[0] = start[1] = 0;
        end[0] = ((vx_image)ref)->width;
        end[1] = ((vx_image)ref)->height;
    }
    else
    {
        for (vx_uint32 i = 0; i < VX_MAX_TENSOR_DIMENSIONS; i++)
        {
            start[i] = 0;
            end[i] = ((vx_tensor)ref)->dimensions[i];
        }
    }
    while ((ref->type == VX_TYPE_IMAGE && ((vx_image)ref)->parent && ((vx_image)ref)->parent != ((vx_image)ref))
        ||
        (ref->type == VX_TYPE_TENSOR && ((vx_tensor)ref)->parent && ((vx_tensor)ref)->parent != ((vx_tensor)ref))
        )
    {
        if (ref->type == VX_TYPE_IMAGE)
        {
            vx_image img = (vx_image)ref;
            vx_size plane_offset = img->memory.ptrs[0] - img->parent->memory.ptrs[0];
            vx_uint32 dy = (vx_uint32)(plane_offset * img->scale[0][VX_DIM_Y] / img->memory.strides[0][VX_DIM_Y]);
            vx_uint32 dx = (vx_uint32)((plane_offset - (dy * img->memory.strides[0][VX_DIM_Y] / img->scale[0][VX_DIM_Y])) * img->scale[0][VX_DIM_X] / img->memory.strides[0][VX_DIM_X]);
            start[0] += dx;
            end[0] += dx;
            start[1] += dy;
            end[1] += dy;
            ref = (vx_reference)img->parent;
        }
        else
        {
            vx_tensor tensor = (vx_tensor)ref;
            vx_uint32 offset = 0;
            for (vx_int32 i = tensor->number_of_dimensions - 1; i >= 0; i--)
            {
                start[i] = ((vx_uint8*)tensor->addr - (vx_uint8*)tensor->parent->addr - offset) / tensor->stride[i];
                end[i] = start[i] + tensor->dimensions[i];
                offset += (vx_uint32)(start[i] * tensor->stride[i]);
            }
            ref = (vx_reference)tensor->parent;
        }
    }
    return ref;
}

static vx_tensor vxLocateView(vx_tensor mddata, vx_size* start, vx_size* end)
{
    for (vx_uint32 i = 0; i < VX_MAX_TENSOR_DIMENSIONS; i++)
    {
        start[i] = 0;
        end[i] = mddata->dimensions[i];
    }
    while (mddata->parent && mddata->parent != mddata)
    {
        size_t offset = 0;
        for (vx_int32 i = mddata->number_of_dimensions-1; i >= 0; i--)
        {
            start[i] = ((vx_uint8*)mddata->addr - (vx_uint8*)mddata->parent->addr - offset) / mddata->stride[i];
            end[i] = start[i] + mddata->dimensions[i];
            offset += start[i] * mddata->stride[i];
        }
        mddata = mddata->parent;
    }
    return mddata;
}

static vx_bool vxCheckWriteDependency(vx_reference ref1, vx_reference ref2)
{
    if (!ref1 || !ref2) /* garbage input */
        return vx_false_e;

    if (ref1 == ref2)
    {
        VX_PRINT(VX_ZONE_API, "returned true - equal refs\n");
        return vx_true_e;
    }

    /* write to layer then read pyramid */
    if (ref1->type == VX_TYPE_PYRAMID && ref2->type == VX_TYPE_IMAGE)
    {
        vx_image img = (vx_image)ref2;
        while (img->parent && img->parent != img) img = img->parent;
        if (img->scope == ref1)
            return vx_true_e;
    }

    /* write to pyramid then read a layer */
    if (ref2->type == VX_TYPE_PYRAMID && ref1->type == VX_TYPE_IMAGE)
    {
        vx_image img = (vx_image)ref1;
        while (img->parent && img->parent != img) img = img->parent;
        if (img->scope == ref2)
            return vx_true_e;
    }

    /* two images or ROIs */
    if (ref1->type == VX_TYPE_IMAGE && ref2->type == VX_TYPE_IMAGE)
    {
        vx_size rr_start[VX_MAX_TENSOR_DIMENSIONS], rw_start[VX_MAX_TENSOR_DIMENSIONS], rr_end[VX_MAX_TENSOR_DIMENSIONS], rw_end[VX_MAX_TENSOR_DIMENSIONS];
        vx_reference refr = vxLocateBaseLocation(ref1, rr_start, rr_end);
        vx_reference refw = vxLocateBaseLocation(ref2, rw_start, rw_end);
        if (refr == refw)
        {
            if (refr->type == VX_TYPE_IMAGE)
            {
                /* check for ROI intersection */
                if (rr_start[0] < rw_end[0] && rr_end[0] > rw_start[0] && rr_start[1] < rw_end[1] && rr_end[1] > rw_start[1])
                {
                    return vx_true_e;
                }
            }
            else
            {
                if (refr->type == VX_TYPE_TENSOR)
                {
                    for (vx_uint32 i = 0; i < ((vx_tensor)refr)->number_of_dimensions; i++)
                    {
                        if ((rr_start[i] >= rw_end[i]) ||
                            (rw_start[i] >= rr_end[i]))
                        {
                            return vx_false_e;
                        }
                    }
                    return vx_true_e;
                }
            }
        }
    }
    if (ref1->type == VX_TYPE_TENSOR && ref2->type == VX_TYPE_TENSOR)
    {
        vx_size rr_start[VX_MAX_TENSOR_DIMENSIONS], rw_start[VX_MAX_TENSOR_DIMENSIONS], rr_end[VX_MAX_TENSOR_DIMENSIONS], rw_end[VX_MAX_TENSOR_DIMENSIONS];
        vx_tensor datar = vxLocateView((vx_tensor)ref1, rr_start, rr_end);
        vx_tensor dataw = vxLocateView((vx_tensor)ref2, rw_start, rw_end);
        if (datar == dataw)
        {
            for (vx_uint32 i = 0; i < datar->number_of_dimensions; i++)
            {
                if ((rr_start[i] >= rw_end[i]) ||
                    (rw_start[i] >= rr_end[i]))
                {
                    return vx_false_e;
                }
            }
            return vx_true_e;
        }
    }

    return vx_false_e;
}

void vxContaminateGraphs(vx_reference ref)
{
    if (Reference::isValidReference(ref) == vx_true_e)
    {
        vx_uint32 r;
        vx_context context = ref->context;
        /*! \internal Scan the entire context for graphs which may contain
         * this reference and mark them as unverified.
         */
        Osal::semWait(&context->lock);
        for (r = 0u; r < context->num_references; r++)
        {
            if (context->reftable[r] == nullptr)
                continue;
            if (context->reftable[r]->type == VX_TYPE_GRAPH)
            {
                vx_uint32 n;
                vx_bool found = vx_false_e;
                vx_graph graph = (vx_graph)context->reftable[r];
                for (n = 0u; n < (graph->numNodes) && (found == vx_false_e); n++)
                {
                    vx_uint32 p;
                    for (p = 0u; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
                    {
                        if (graph->nodes[n]->kernel->signature.directions[p] == VX_OUTPUT)
                        {
                            continue;
                        }
                        if (graph->nodes[n]->parameters[p] == ref)
                        {
                            found = vx_true_e;
                            graph->reverify = graph->verified;
                            graph->verified = vx_false_e;
                            graph->state = VX_GRAPH_STATE_UNVERIFIED;
                            break;
                        }
                    }
                }
            }
        }
        Osal::semPost(&context->lock);
    }
}

/******************************************************************************/
/* INTERNAL FUNCTIONS */
/******************************************************************************/

Graph::Graph(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_GRAPH, scope),
nodes(),
perf(),
numNodes(0),
heads(),
numHeads(0),
state(VX_FAILURE),
verified(vx_false_e),
reverify(vx_false_e),
lock(),
parameters(),
numParams(0),
shouldSerialize(vx_false_e),
parentGraph(nullptr),
delays()
{
}

Graph::~Graph()
{
}

void Graph::clearVisitation()
{
    vx_uint32 n = 0;
    for (n = 0; n < numNodes; n++)
        nodes[n]->visited = vx_false_e;
}

void Graph::clearExecution()
{
    vx_uint32 n = 0;
    for (n = 0; n < numNodes; n++)
        nodes[n]->executed = vx_false_e;
}

vx_status Graph::findNodesWithReference(
                                   vx_reference ref,
                                   vx_uint32 refnodes[],
                                   vx_uint32 *count,
                                   vx_enum reftype)
{
    vx_uint32 n, p, nc = 0, max;
    vx_status status = VX_ERROR_INVALID_LINK;

    /* save the maximum number of nodes to find */
    max = *count;

    /* reset the current count to zero */
    *count = 0;

    VX_PRINT(VX_ZONE_GRAPH,"Find nodes with reference " VX_FMT_REF " type %d over %u nodes upto %u finds\n", ref, reftype, numNodes, max);
    for (n = 0; n < numNodes; n++)
    {
        for (p = 0; p < nodes[n]->kernel->signature.num_parameters; p++)
        {
            vx_enum dir = nodes[n]->kernel->signature.directions[p];
            vx_reference thisref = nodes[n]->parameters[p];

            VX_PRINT(VX_ZONE_GRAPH,"\tchecking node[%u].parameter[%u] dir = %d ref = " VX_FMT_REF " (=?%d:" VX_FMT_REF ")\n", n, p, dir, thisref, reftype, ref);
            if ((dir == reftype) && vxCheckWriteDependency(thisref, ref))
            {
                if (nc < max)
                {
                    VX_PRINT(VX_ZONE_GRAPH, "match at node[%u].parameter[%u]\n", n, p);
                    if (refnodes)
                        refnodes[nc] = n;
                    nc++;
                    status = VX_SUCCESS;
                }
                else
                {
                    VX_PRINT(VX_ZONE_ERROR, "ERROR: Overflow in refnodes[]\n");
                }
            }
        }
    }
    *count = nc;
    VX_PRINT(VX_ZONE_GRAPH, "Found %u nodes with reference " VX_FMT_REF " status = %d\n", nc, ref, status);
    return status;
}

void Graph::findNextNodes(
                     vx_uint32 last_nodes[VX_INT_MAX_REF], vx_uint32 numLast,
                     vx_uint32 next_nodes[VX_INT_MAX_REF], vx_uint32 *numNext,
                     vx_uint32 left_nodes[VX_INT_MAX_REF], vx_uint32 *numLeft)
{
    vx_uint32 poss_next[VX_INT_MAX_REF];
    vx_uint32 i,n,p,n1,numPoss = 0;

    VX_PRINT(VX_ZONE_GRAPH, "Entering with %u left nodes\n", *numLeft);
    for (n = 0; n < *numLeft; n++)
    {
        VX_PRINT(VX_ZONE_GRAPH, "leftover: node[%u] = %s\n", left_nodes[n], nodes[left_nodes[n]]->kernel->name);
    }

    numPoss = 0;
    *numNext = 0;

    /* for each last node, add all output to input nodes to the list of possible. */
    for (i = 0; i < numLast; i++)
    {
        n = last_nodes[i];
        for (p = 0; p < nodes[n]->kernel->signature.num_parameters; p++)
        {
            vx_enum dir = nodes[n]->kernel->signature.directions[p];
            vx_reference ref =  nodes[n]->parameters[p];
            if (((dir == VX_OUTPUT) || (dir == VX_BIDIRECTIONAL)) && (ref != nullptr))
            {
                /* send the max possible nodes */
                n1 = dimof(poss_next) - numPoss;
                if (findNodesWithReference(ref, &poss_next[numPoss], &n1, VX_INPUT) == VX_SUCCESS)
                {
                    VX_PRINT(VX_ZONE_GRAPH, "Adding %u nodes to possible list\n", n1);
                    numPoss += n1;
                }
            }
        }
    }

    VX_PRINT(VX_ZONE_GRAPH, "There are %u possible nodes\n", numPoss);

    /* add back all the left over nodes (making sure to not include duplicates) */
    for (i = 0; i < *numLeft; i++)
    {
        vx_uint32 j;
        vx_bool match = vx_false_e;
        for (j = 0; j < numPoss; j++)
        {
            if (left_nodes[i] == poss_next[j])
            {
                match = vx_true_e;
            }
        }
        if (match == vx_false_e)
        {
            VX_PRINT(VX_ZONE_GRAPH, "Adding back left over node[%u] %s\n", left_nodes[i], nodes[left_nodes[i]]->kernel->name);
            poss_next[numPoss++] = left_nodes[i];
        }
    }
    *numLeft = 0;

    /* now check all possible next nodes to see if the parent nodes are visited. */
    for (i = 0; i < numPoss; i++)
    {
        vx_uint32 poss_params[VX_INT_MAX_PARAMS];
        vx_uint32 pi, numPossParam = 0;
        vx_bool ready = vx_true_e;

        n = poss_next[i];
        VX_PRINT(VX_ZONE_GRAPH, "possible: node[%u] = %s\n", n, nodes[n]->kernel->name);
        for (p = 0; p < nodes[n]->kernel->signature.num_parameters; p++)
        {
            if (nodes[n]->kernel->signature.directions[p] == VX_INPUT)
            {
                VX_PRINT(VX_ZONE_GRAPH,"nodes[%u].parameter[%u] predicate needs to be checked\n", n, p);
                poss_params[numPossParam] = p;
                numPossParam++;
            }
        }

        /* now check to make sure all possible input parameters have their */
        /* parent nodes executed. */
        for (pi = 0; pi < numPossParam; pi++)
        {
            vx_uint32 predicate_nodes[VX_INT_MAX_REF];
            vx_uint32 predicate_count = 0;
            vx_uint32 predicate_index = 0;
            vx_uint32 refIdx = 0;
            vx_reference ref = 0;
            vx_enum reftype[2] = {VX_OUTPUT, VX_BIDIRECTIONAL};

            p = poss_params[pi];
            ref = nodes[n]->parameters[p];
            VX_PRINT(VX_ZONE_GRAPH, "checking node[%u].parameter[%u] = " VX_FMT_REF "\n", n, p, ref);

            for(refIdx = 0; refIdx < dimof(reftype); refIdx++)
            {
                /* set the size of predicate nodes going in */
                predicate_count = dimof(predicate_nodes);
                if (findNodesWithReference(ref, predicate_nodes, &predicate_count, reftype[refIdx]) == VX_SUCCESS)
                {
                    /* check to see of all of the predicate nodes are executed */
                    for (predicate_index = 0;
                         predicate_index < predicate_count;
                         predicate_index++)
                    {
                        n1 = predicate_nodes[predicate_index];
                        if (nodes[n1]->executed == vx_false_e)
                        {
                            VX_PRINT(VX_ZONE_GRAPH, "predicated: node[%u] = %s\n", n1, nodes[n1]->kernel->name);
                            ready = vx_false_e;
                            break;
                        }
                    }
                }
                if(ready == vx_false_e)
                {
                    break;
                }
            }
        }
        if (ready == vx_true_e)
        {
            /* make sure we don't schedule this node twice */
            if (nodes[n]->visited == vx_false_e)
            {
                next_nodes[(*numNext)++] = n;
                nodes[n]->visited = vx_true_e;
            }
        }
        else
        {
            /* put the node back into the possible list for next time */
            left_nodes[(*numLeft)++] = n;
            VX_PRINT(VX_ZONE_GRAPH, "notready: node[%u] = %s\n", n, nodes[n]->kernel->name);
        }
    }

    VX_PRINT(VX_ZONE_GRAPH, "%u Next Nodes\n", *numNext);
    for (i = 0; i < *numNext; i++)
    {
        n = next_nodes[i];
        VX_PRINT(VX_ZONE_GRAPH, "next: node[%u] = %s\n", n, nodes[n]->kernel->name);
    }
    VX_PRINT(VX_ZONE_GRAPH, "%u Left Nodes\n", *numLeft);
    for (i = 0; i < *numLeft; i++)
    {
        n = left_nodes[i];
        VX_PRINT(VX_ZONE_GRAPH, "left: node[%u] = %s\n", n, nodes[n]->kernel->name);
    }
}

vx_status Graph::traverseGraph(vx_uint32 parentIndex,
                               vx_uint32 childIndex)
{
    /* this is expensive, but needed in order to know who references a parameter */
    static vx_uint32 refNodes[VX_INT_MAX_REF];
    /* this keeps track of the available starting point in the static buffer */
    static vx_uint32 refStart = 0;
    /* this makes sure we don't have any odd conditions about infinite depth */
    static vx_uint32 depth = 0;

    vx_uint32 refCount = 0;
    vx_uint32 refIndex = 0;
    vx_uint32 thisIndex = 0;
    vx_status status = VX_SUCCESS;
    vx_uint32 p = 0;

    VX_PRINT(VX_ZONE_GRAPH, "refStart = %u\n", refStart);

    if (parentIndex == childIndex && parentIndex != VX_INT_MAX_NODES)
    {
        VX_PRINT(VX_ZONE_ERROR, "################################\n");
        VX_PRINT(VX_ZONE_ERROR, "ERROR: CYCLE DETECTED! node[%u]\n", parentIndex);
        VX_PRINT(VX_ZONE_ERROR, "################################\n");
        /* there's a cycle in the graph */
        status = VX_ERROR_INVALID_GRAPH;
    }
    else if (depth > numNodes) /* should be impossible under normal circumstances */
    {
        /* there's a cycle in the graph */
        status = VX_ERROR_INVALID_GRAPH;
    }
    else
    {
        /* if the parent is an invalid index, then we assume we're processing a
         * head of a graph which has no parent index.
         */
        if (parentIndex == VX_INT_MAX_NODES)
        {
            parentIndex = childIndex;
            thisIndex = parentIndex;
            VX_PRINT(VX_ZONE_GRAPH, "Starting head-first traverse of graph from node[%u]\n", thisIndex);
        }
        else
        {
            thisIndex = childIndex;
            VX_PRINT(VX_ZONE_GRAPH, "continuing traverse of graph from node[%u] on node[%u] start=%u\n", parentIndex, thisIndex, refStart);
        }

        for (p = 0; p < nodes[thisIndex]->kernel->signature.num_parameters; p++)
        {
            vx_enum dir = nodes[thisIndex]->kernel->signature.directions[p];
            vx_reference ref = nodes[thisIndex]->parameters[p];

            if (dir != VX_INPUT && ref != nullptr)
            {
                VX_PRINT(VX_ZONE_GRAPH, "[traverse] node[%u].parameter[%u] = " VX_FMT_REF "\n", thisIndex, p, ref);
                /* send the maximum number of possible nodes to find */
                refCount = dimof(refNodes) - refStart;
                status = findNodesWithReference(ref, &refNodes[refStart], &refCount, VX_INPUT);
                VX_PRINT(VX_ZONE_GRAPH, "status = %d at node[%u] start=%u count=%u\n", status, thisIndex, refStart, refCount);
                if (status == VX_SUCCESS)
                {
                    vx_uint32 refStop = refStart + refCount;
                    VX_PRINT(VX_ZONE_GRAPH, "Looping from %u to %u\n", refStart, refStop);
                    for (refIndex = refStart; refIndex < refStop; refIndex++)
                    {
                        vx_status child_status = VX_SUCCESS;
                        VX_PRINT(VX_ZONE_GRAPH, "node[%u] => node[%u]\n", thisIndex, refNodes[refIndex]);
                        refStart += refCount;
                        depth++; /* go one more level in */
                        child_status = traverseGraph(thisIndex, refNodes[refIndex]);
                        if (child_status != VX_SUCCESS)
                            status = child_status;
                        depth--; /* pull out one level */
                        refStart -= refCount;
                        VX_PRINT(VX_ZONE_GRAPH, "status = %d at node[%u]\n", status, thisIndex);
                    }
                }
                if (status == VX_ERROR_INVALID_LINK) /* no links at all */
                {
                    VX_PRINT(VX_ZONE_GRAPH, "[Ok] No link found for node[%u].parameter[%u]\n", thisIndex, p);
                    status = VX_SUCCESS;
                }
            }
            else
            {
                VX_PRINT(VX_ZONE_GRAPH, "[ ignore ] node[%u].parameter[%u] = " VX_FMT_REF " type %d\n", childIndex, p, ref, dir);
            }
            if (status == VX_ERROR_INVALID_GRAPH)
                break;
        }

        if (status == VX_SUCCESS)
        {
            /* mark it visited for the next check to pass */
            nodes[thisIndex]->visited = vx_true_e;
        }
    }
    VX_PRINT(VX_ZONE_GRAPH, "returning status %d\n", status);
    return status;
}

void Graph::topologicalSort(vx_node *list, vx_uint32 nnodes)
{
   /* Knuth TAoCP algorithm 2.2.3 T.
      Nodes and their parameters are the "objects" which have partial-order
      relations (with "<" noting the similar-looking general symbol for
      relational order, not the specific less-than relation), and it's
      always pair-wise: node < parameter for outputs,
      parameter < node for inputs. */
    vx_uint32 nobjects;         /* Number of objects; "n" in TAoCP. */
    vx_uint32 nremain;          /* Number of remaining objects to be "output"; "N" in TAoCP. */
    vx_uint32 objectno;         /* Running count, 1-based. */
    vx_uint32 j, k, n, r, f;
    vx_uint32 outputnr;         /* Running count of nodes as they're "output". */

    struct direct_successor {
        vx_uint32 suc;
        struct direct_successor *next;
    };

    struct object_relations {
        union {
            vx_uint32 count;
            vx_uint32 qlink;
        } u;
        struct direct_successor *top;
        vx_reference ref;
    };

    std::unique_ptr<object_relations[]> x;
    std::unique_ptr<direct_successor[]> suc_next_table;
    struct direct_successor *avail;

    /* Visit each node in the list and its in- and out-parameters,
       clearing all indices. Find upper bound for nobjects, for use when
       allocating x (X in the algorithm). This number is also the exact
       number of relations, for use with suc_next_table (the unnamed
       "suc, next" table in the algorithm). */
    vx_uint32 max_n_objects_relations = nnodes;

    for (n = 0; n < nnodes; n++)
    {
        vx_uint32 parmno;

        max_n_objects_relations += list[n]->kernel->signature.num_parameters;

        for (parmno = 0; parmno < list[n]->kernel->signature.num_parameters; parmno++)
        {
            /* Pick the parent object in case of su-objects (e.g., ROI) */
            vx_reference ref = list[n]->parameters[parmno];
            while ( (ref != nullptr) &&
                    (ref->scope != nullptr) &&
                    (ref->scope != (vx_reference)this) &&
                    (ref->scope != (vx_reference)ref->context)
                    )
            {
                ref = ref->scope;
            }

            if (ref != nullptr)
            {
                ref->index = 0;
            }
            else
            {
                /* Ignore nullptr (optional) parameters. */
                max_n_objects_relations--;
            }
        }
    }

    /* Visit each node and its parameters, setting all indices. Allocate
       and initialize the node + parameters-list. The x table is
       1-based; index 0 is a sentinel.
       (This is step T1.) */
    x = std::make_unique<object_relations[]>(max_n_objects_relations + 1);
    suc_next_table = std::make_unique<direct_successor[]>(max_n_objects_relations);

    avail = suc_next_table.get();

    for (objectno = 1; objectno <= nnodes; objectno++)
    {
        vx_node node = list[objectno - 1];
        node->index = objectno;
        x[objectno].ref = (vx_reference)node;
    }

    /* While we visit the parameters (setting their index if 0), we
       "input" the relation. We don't have to iterate separately after
       all parameters are "indexed", as all nodes are already in place
       (and "indexed") and a parameter doesn't have a direct relation
       with another parameter.
       (Steps T2 and T3). */
    for (n = 0; n < nnodes; n++)
    {
        vx_uint32 parmno;

        for (parmno = 0; parmno < list[n]->kernel->signature.num_parameters; parmno++)
        {
            vx_reference ref = list[n]->parameters[parmno];
            struct direct_successor *p;

            /* Pick the parent object in case of su-objects (e.g., ROI) */
            while ( (ref != nullptr) &&
                    (ref->scope != nullptr) &&
                    (ref->scope != (vx_reference)this) &&
                    (ref->scope != (vx_reference)ref->context)
                    )
            {
                ref = ref->scope;
            }

            if (ref == nullptr)
            {
                continue;
            }

            if (ref->index == 0)
            {
                x[objectno].ref = ref;
                ref->index = objectno++;
            }

            /* Step T2. */
            if (list[n]->kernel->signature.directions[parmno] == VX_INPUT)
            {
                /* parameter < node */
                j = ref->index;
                k = n + 1;
            }
            else
            {
                /* node < parameter */
                k = ref->index;
                j = n + 1;
            }

            /* Step T3. */
            x[k].u.count++;
            p = avail++;
            p->suc = k;
            p->next = x[j].top;
            x[j].top = p;
        }
    }

    /* With a 1-based index, we need to back-off one to get the number of objects. */
    nobjects = objectno - 1;
    nremain = nobjects;

    /* At this point, we could visit all the nodes in
       x[1..graph->numNodes] (all the first graph->numNodes in x are
       nodes) and put those with
       count <= node->kernel->signature.num_parameters in graph->heads
       (as a node is always dependent on its input parameters and all
       nodes have inputs, but some may be nullptr), avoiding the
       O(numNodes**2) loop later in our caller. */

    /* Step T4. Note that we're not zero-based but 1-based; 0 and x[0]
       are sentinels. */
    r = 0;
    x[0].u.qlink = 0;
    for (k = 1; k <= nobjects; k++)
    {
        if (x[k].u.count == 0)
        {
            x[r].u.qlink = k;
            r = k;
        }
    }

    f = x[0].u.qlink;
    outputnr = 0;

    /* Step T5. (We don't output a final 0 as present in the algorithm.) */
    while (f != 0)
    {
        struct direct_successor *p;

        /* This is our "output". Nodes only; we don't otherwise make
           use the order in which parameters are being processed. */
        if (x[f].ref->type == VX_TYPE_NODE)
            list[outputnr++] = (vx_node)x[f].ref;
        nremain--;
        p = x[f].top;

        /* Step T6. */
        while (p != nullptr)
        {
            if (--x[p->suc].u.count == 0)
            {
                x[r].u.qlink = p->suc;
                r = p->suc;
            }
            p = p->next;
        }

        /* Step T7 */
        f = x[f].u.qlink;
    }

    /* Step T8.
       At this point, we could inspect nremain and if non-zero, we have
       a cycle. To wit, we can avoid the O(numNodes**2) loop later in
       our caller. We have to do check for cycles anyway, because if
       there was a cycle we need to restore *all* the nodes into list[],
       or else they can't be disposed of properly. We use the original
       order, both for simplicity and because the caller might be making
       invalid assumptions; having passed an incorrect graph is cause
       for concern about integrity. */
    if (nremain != 0)
        for (n = 0; n < nnodes; n++)
            list[n] = (vx_node)x[n+1].ref;
}

vx_bool Graph::setupOutput(vx_uint32 n, vx_uint32 p, vx_reference* vref, vx_meta_format* meta,
                            vx_status* status, vx_uint32* num_errors)
{
    *vref = nodes[n]->parameters[p];
    *meta = vxCreateMetaFormat(context);

    /* check to see if the reference is virtual */
    if ((*vref)->is_virtual == vx_false_e)
    {
        *vref = nullptr;
    }
    else
    {
        VX_PRINT(VX_ZONE_GRAPH, "Virtual Reference detected at kernel %s parameter %u\n",
                nodes[n]->kernel->name,
                p);
        if ((*vref)->scope->type == VX_TYPE_GRAPH &&
            (*vref)->scope != (vx_reference)this &&
            /* We check only one level up; we make use of the
               knowledge that this implementation has no more
               than one level of child-graphs. (Nodes are only
               one level; no child-graph-using node is composed
               from other child-graph-using nodes.) We need
               this check (for example) for a virtual image
               being an output parameter to a node for which
               this graph is the child-graph implementation,
               like in vx_bug13517.c. */
            (*vref)->scope != (vx_reference)parentGraph)
        {
            /* major fault! */
            *status = VX_ERROR_INVALID_SCOPE;
            vxAddLogEntry((vx_reference)(*vref), *status, "Virtual Reference is in the wrong scope, created from another graph!\n");
            (*num_errors)++;
            return vx_false_e; /* break; */
        }
        /* ok if context, pyramid or this graph */
    }

    /* the type of the parameter is known by the system, so let the system set it by default. */
    (*meta)->type = nodes[n]->kernel->signature.types[p];

    return vx_true_e;
}

/*
 * Parameters:
 *   n    - index of node
 *   p    - index of parameter
 *   vref - reference of the parameter
 *   meta - parameter meta info
 */
vx_bool Graph::postprocessOutputDataType(vx_uint32 n, vx_uint32 p, vx_reference* item, vx_reference* vref, vx_meta_format meta,
                                  vx_status* status, vx_uint32* num_errors)
{
    if (Context::isValidType(meta->type) == vx_false_e)
    {
        *status = VX_ERROR_INVALID_TYPE;
        vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
            "Node: %s: parameter[%u] is not a valid type %d!\n",
            nodes[n]->kernel->name, p, meta->type);
        (*num_errors)++;
        return vx_false_e; /* exit on error */
    }

    if (meta->type == VX_TYPE_IMAGE)
    {
        vx_image img = (vx_image)*item;
        VX_PRINT(VX_ZONE_GRAPH, "meta: type 0x%08x, %ux%u\n", meta->type, meta->dim.image.width, meta->dim.image.height);
        if (*vref == (vx_reference)img)
        {
            VX_PRINT(VX_ZONE_GRAPH, "Creating Image From Meta Data!\n");
            /*! \todo need to worry about images that have a format, but no dimensions too */
            if (img->format == VX_DF_IMAGE_VIRT || img->format == meta->dim.image.format)
            {
                img->format = meta->dim.image.format;
                img->width = meta->dim.image.width;
                img->height = meta->dim.image.height;
                /* we have to go set all the other dimensional information up. */
                img->initImage(img->width, img->height, img->format);
                Image::printImage(img); /* show that it's been created. */
            }
            else
            {
                *status = VX_ERROR_INVALID_FORMAT;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                    "Node: %s: parameter[%u] has invalid format %08x!\n",
                    nodes[n]->kernel->name, p, img->format);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has invalid format %08x!\n",
                    nodes[n]->kernel->name, p, img->format);
                (*num_errors)++;
                return vx_false_e; /* exit on error */
            }
        }
        else
        {
            /* check the data that came back from the output validator against the object */
            if ((img->width != meta->dim.image.width) ||
                (img->height != meta->dim.image.height))
            {
                *status = VX_ERROR_INVALID_DIMENSION;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                    "Node: %s: parameter[%u] is an invalid dimension %ux%u!\n",
                    nodes[n]->kernel->name, p, img->width, img->height);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] is an invalid dimension %ux%u!\n",
                    nodes[n]->kernel->name, p, img->width, img->height);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: expected dimension %ux%u with format %08x!\n",
                    nodes[n]->kernel->name, meta->dim.image.width, meta->dim.image.height, meta->dim.image.format);
                (*num_errors)++;
                return vx_false_e; /* exit on error */
            }
            if (img->format != meta->dim.image.format)
            {
                *status = VX_ERROR_INVALID_FORMAT;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                    "Node: %s: parameter[%u] is an invalid format %08x!\n",
                    nodes[n]->kernel->name, p, img->format);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has invalid format %08x!\n",
                    nodes[n]->kernel->name, p, img->format);
                (*num_errors)++;
                return vx_false_e; /* exit on error */
            }
        }

        if (nullptr != meta->set_valid_rectangle_callback)
            nodes[n]->attributes.valid_rect_reset = vx_false_e;

        if (vx_false_e == nodes[n]->attributes.valid_rect_reset &&
            nullptr != meta->set_valid_rectangle_callback)
        {
            /* calculate image valid rectangle through callback */

            vx_uint32 i;
            vx_uint32 nparams = 0;
            vx_uint32 num_in_images = 0;
            vx_rectangle_t** in_rect = nullptr;
            vx_rectangle_t* out_rect[1] = { nullptr };
            vx_node node = nodes[n];

            /* assume no errors */
            vx_bool res = vx_true_e;

            if (VX_SUCCESS != vxQueryNode(node, VX_NODE_PARAMETERS, &nparams, sizeof(nparams)))
            {
                *status = VX_FAILURE;
                return vx_false_e;
            }

            /* compute num of input images */
            for (i = 0; i < nparams; i++)
            {
                if (VX_INPUT == node->kernel->signature.directions[i] &&
                    VX_TYPE_IMAGE == node->parameters[i]->type)
                {
                    num_in_images++;
                }
            }

            /* allocate array of pointers to input images valid rectangles */
            in_rect = new vx_rectangle_t*[num_in_images]();
            if (nullptr == in_rect)
            {
                *status = VX_FAILURE;
                return vx_false_e;
            }

            for (i = 0; i < nparams; i++)
            {
                if (VX_INPUT == node->kernel->signature.directions[i] &&
                    VX_TYPE_IMAGE == node->parameters[i]->type)
                {
                    in_rect[i] = new vx_rectangle_t();
                    if (nullptr == in_rect[i])
                    {
                        *status = VX_FAILURE;
                        res = vx_false_e;
                        break;
                    }

                    /* collect input images valid rectagles in array */
                    if (VX_SUCCESS != vxGetValidRegionImage((vx_image)node->parameters[i], in_rect[i]))
                    {
                        *status = VX_FAILURE;
                        res = vx_false_e;
                        break;
                    }
                }
            }

            if (vx_false_e != res)
            {
                out_rect[0] = new vx_rectangle_t();
                if (nullptr == out_rect[0])
                {
                    *status = VX_FAILURE;
                    res = vx_false_e;
                }

                /* calculate output image valid rectangle */
                if (VX_SUCCESS == meta->set_valid_rectangle_callback(nodes[n], p, (const vx_rectangle_t* const*)in_rect,
                                                                     (vx_rectangle_t* const*)out_rect))
                {
                    /* set output image valid rectangle */
                    if (VX_SUCCESS != vxSetImageValidRectangle(img, (const vx_rectangle_t*)out_rect[0]))
                    {
                        *status = VX_FAILURE;
                        res = vx_false_e;
                    }
                }
                else
                {
                    *status = VX_FAILURE;
                    res = vx_false_e;
                }
            }

            /* deallocate arrays memory */
            for (i = 0; i < num_in_images; i++)
            {
                if (nullptr != in_rect[i])
                {
                    delete(in_rect[i]);
                }
            }

            if (nullptr != in_rect)
                delete[](in_rect);
            if (nullptr != out_rect[0])
                delete(out_rect[0]);


            return res;
        }

        if (vx_true_e == nodes[n]->attributes.valid_rect_reset)
        {
            /* reset image valid rectangle */
            vx_rectangle_t out_rect;
            out_rect.start_x = 0;
            out_rect.start_y = 0;
            out_rect.end_x   = img->width;
            out_rect.end_y   = img->height;

            if (VX_SUCCESS != vxSetImageValidRectangle(img, &out_rect))
            {
                *status = VX_FAILURE;
                return vx_false_e;
            }
        }
    } /* VX_TYPE_IMAGE */
    else if (meta->type == VX_TYPE_ARRAY)
    {
        vx_array arr = (vx_array)*item;
        VX_PRINT(VX_ZONE_GRAPH, "meta: type 0x%08x, 0x%08x " VX_FMT_SIZE "\n", meta->type, meta->dim.array.item_type, meta->dim.array.capacity);
        if (*vref == (vx_reference)arr)
        {
            VX_PRINT(VX_ZONE_GRAPH, "Creating Array From Meta Data %x and " VX_FMT_SIZE "!\n", meta->dim.array.item_type, meta->dim.array.capacity);
            if (arr->initVirtualArray(meta->dim.array.item_type, meta->dim.array.capacity) != vx_true_e)
            {
                *status = VX_ERROR_INVALID_DIMENSION;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_DIMENSION,
                    "Node: %s: meta[%u] has an invalid item type 0x%08x or capacity " VX_FMT_SIZE "\n",
                    nodes[n]->kernel->name, p, meta->dim.array.item_type, meta->dim.array.capacity);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: meta[%u] has an invalid item type 0x%08x or capacity " VX_FMT_SIZE "\n",
                    nodes[n]->kernel->name, p, meta->dim.array.item_type, meta->dim.array.capacity);
                (*num_errors)++;
                return vx_false_e; //break;
            }
        }
        else
        {
            if (arr->validateArray(meta->dim.array.item_type, meta->dim.array.capacity) != vx_true_e)
            {
                *status = VX_ERROR_INVALID_DIMENSION;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_DIMENSION,
                    "Node: %s: parameter[%u] has an invalid item type 0x%08x or capacity " VX_FMT_SIZE "\n",
                    nodes[n]->kernel->name, p, arr->item_type, arr->capacity);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has an invalid item type 0x%08x or capacity " VX_FMT_SIZE "\n",
                    nodes[n]->kernel->name, p, arr->item_type, arr->capacity);
                (*num_errors)++;
                return vx_false_e; //break;
            }
        }
    }
    else if (meta->type == VX_TYPE_PYRAMID)
    {
        vx_pyramid pyramid = (vx_pyramid)*item;

        vx_uint32 i;
        vx_bool res = vx_true_e;

        VX_PRINT(VX_ZONE_GRAPH, "meta: type 0x%08x, %ux%u:%u:%lf\n",
            meta->type,
            meta->dim.pyramid.width,
            meta->dim.pyramid.height,
            meta->dim.pyramid.levels,
            meta->dim.pyramid.scale);
        VX_PRINT(VX_ZONE_GRAPH, "Nodes[%u] %s parameters[%u]\n", n, nodes[n]->kernel->name, p);

        if ((pyramid->numLevels != meta->dim.pyramid.levels) ||
            (pyramid->scale != meta->dim.pyramid.scale))
        {
            *status = VX_ERROR_INVALID_VALUE;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status, "Either levels (%u?=%u) or scale (%lf?=%lf) are invalid\n",
                pyramid->numLevels, meta->dim.pyramid.levels,
                pyramid->scale, meta->dim.pyramid.scale);
            (*num_errors)++;
            return vx_false_e; //break;
        }

        if ((pyramid->format != VX_DF_IMAGE_VIRT) &&
            (pyramid->format != meta->dim.pyramid.format))
        {
            *status = VX_ERROR_INVALID_FORMAT;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status, "Invalid pyramid format %x, needs %x\n",
                pyramid->format,
                meta->dim.pyramid.format);
            (*num_errors)++;
            return vx_false_e; //break;
        }

        if (((pyramid->width != 0) &&
            (pyramid->width != meta->dim.pyramid.width)) ||
            ((pyramid->height != 0) &&
            (pyramid->height != meta->dim.pyramid.height)))
        {
            *status = VX_ERROR_INVALID_DIMENSION;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status, "Invalid pyramid dimensions %ux%u, needs %ux%u\n",
                pyramid->width, pyramid->height,
                meta->dim.pyramid.width, meta->dim.pyramid.height);
            (*num_errors)++;
            return vx_false_e; //break;
        }

        /* check to see if the pyramid is virtual */
        if (*vref == (vx_reference)pyramid)
        {
            pyramid->initPyramid(
                meta->dim.pyramid.levels,
                meta->dim.pyramid.scale,
                meta->dim.pyramid.width,
                meta->dim.pyramid.height,
                meta->dim.pyramid.format);
        }

        if (nullptr != meta->set_valid_rectangle_callback)
            nodes[n]->attributes.valid_rect_reset = vx_false_e;

        if (vx_false_e == nodes[n]->attributes.valid_rect_reset &&
            nullptr != meta->set_valid_rectangle_callback)
        {
            /* calculate pyramid levels valid rectangles */

            vx_uint32 nparams = 0;
            vx_uint32 num_in_images = 0;
            vx_rectangle_t** in_rect = nullptr;
            vx_rectangle_t** out_rect = nullptr;

            vx_node node = nodes[n];

            if (VX_SUCCESS != vxQueryNode(node, VX_NODE_PARAMETERS, &nparams, sizeof(nparams)))
            {
                *status = VX_FAILURE;
                return vx_false_e;
            }

            /* compute num of input images */
            for (i = 0; i < nparams; i++)
            {
                if (VX_INPUT == node->kernel->signature.directions[i] &&
                    VX_TYPE_IMAGE == node->parameters[i]->type)
                {
                    num_in_images++;
                }
            }

            in_rect = new vx_rectangle_t* [num_in_images]();
            if (nullptr == in_rect)
            {
                *status = VX_FAILURE;
                return vx_false_e;
            }

            for (i = 0; i < num_in_images; i++)
                in_rect[i] = nullptr;

            for (i = 0; i < nparams; i++)
            {
                if (VX_INPUT == node->kernel->signature.directions[i] &&
                    VX_TYPE_IMAGE == node->parameters[i]->type)
                {
                    in_rect[i] = new vx_rectangle_t();
                    if (nullptr == in_rect[i])
                    {
                        *status = VX_FAILURE;
                        res = vx_false_e;
                        break;
                    }

                    if (VX_SUCCESS != vxGetValidRegionImage((vx_image)node->parameters[i], in_rect[i]))
                    {
                        *status = VX_FAILURE;
                        res = vx_false_e;
                        break;
                    }
                }
            }

            if (vx_false_e != res)
            {
                out_rect = new vx_rectangle_t*[meta->dim.pyramid.levels]();
                if (nullptr != out_rect)
                {
                    vx_uint32 k;
                    for (k = 0; k < meta->dim.pyramid.levels; k++)
                        out_rect[k] = nullptr;

                    for (i = 0; i < meta->dim.pyramid.levels; i++)
                    {
                        out_rect[i] = new vx_rectangle_t();
                        if (nullptr == out_rect[i])
                        {
                            *status = VX_FAILURE;
                            res = vx_false_e;
                            break;
                        }
                    }
                }
                else
                {
                    *status = VX_FAILURE;
                    res = vx_false_e;
                }
            }

            if (vx_false_e != res)
            {
                /* calculate pyramid levels valid rectangles */
                if (VX_SUCCESS == meta->set_valid_rectangle_callback(nodes[n], p, (const vx_rectangle_t* const*)in_rect, out_rect))
                {
                    for (i = 0; i < meta->dim.pyramid.levels; i++)
                    {
                        vx_image img = vxGetPyramidLevel(pyramid, i);

                        if (vx_false_e == Reference::isValidReference((vx_reference)img, VX_TYPE_IMAGE))
                        {
                            *status = VX_FAILURE;
                            res = vx_false_e;
                            vxReleaseImage(&img); /* already on error path, ignore additional errors */
                            break;
                        }

                        if (VX_SUCCESS != vxSetImageValidRectangle(img, out_rect[i]))
                        {
                            *status = VX_FAILURE;
                            res = vx_false_e;
                            vxReleaseImage(&img); /* already on error path, ignore additional errors */
                            break;
                        }

                        if (VX_SUCCESS != vxReleaseImage(&img))
                        {
                            *status = VX_FAILURE;
                            res = vx_false_e;
                            break;
                        }
                    } /* for pyramid levels */
                }
                else
                {
                    *status = VX_FAILURE;
                    res = vx_false_e;
                }
            } /* if successful memory allocation */

            /* deallocate rectangle arrays */
            for (i = 0; i < num_in_images; i++)
            {
                if (nullptr != in_rect && nullptr != in_rect[i])
                {
                    delete(in_rect[i]);
                }
            }

            if (nullptr != in_rect)
                delete[](in_rect);

            for (i = 0; i < meta->dim.pyramid.levels; i++)
            {
                if (nullptr != out_rect && nullptr != out_rect[i])
                    delete(out_rect[i]);
            }

            if (nullptr != out_rect)
                delete[](out_rect);

            return res;
        }

        if (vx_true_e == nodes[n]->attributes.valid_rect_reset)
        {
            /* reset output pyramid levels valid rectangles */

            vx_bool res = vx_true_e;

            for (i = 0; i < meta->dim.pyramid.levels; i++)
            {
                vx_uint32 width = 0;
                vx_uint32 height = 0;
                vx_rectangle_t out_rect;

                vx_image img = vxGetPyramidLevel(pyramid, i);

                if (vx_false_e == Reference::isValidReference((vx_reference)img, VX_TYPE_IMAGE))
                {
                    *status = VX_FAILURE;
                    return vx_false_e;
                }

                if (VX_SUCCESS != vxQueryImage(img, VX_IMAGE_WIDTH, &width, sizeof(width)))
                {
                    *status = VX_FAILURE;
                    res = vx_false_e;
                    vxReleaseImage(&img); /* already on error path, ignore additional errors */
                    break;
                }

                if (VX_SUCCESS != vxQueryImage(img, VX_IMAGE_HEIGHT, &height, sizeof(height)))
                {
                    *status = VX_FAILURE;
                    res = vx_false_e;
                    vxReleaseImage(&img); /* already on error path, ignore additional errors */
                    break;
                }

                if (vx_false_e != res)
                {
                    out_rect.start_x = 0;
                    out_rect.start_y = 0;
                    out_rect.end_x   = width;
                    out_rect.end_y   = height;

                    /* pyramid level valid rectangle is a whole image */
                    if (VX_SUCCESS != vxSetImageValidRectangle(img, &out_rect))
                    {
                        *status = VX_FAILURE;
                        res = vx_false_e;
                    }
                }

                if (VX_SUCCESS != vxReleaseImage(&img))
                {
                    *status = VX_FAILURE;
                    res = vx_false_e;
                }
            } /* for pyramid levels */

            return res;
        }
    } /* VX_TYPE_PYRAMID */
    else if (meta->type == VX_TYPE_SCALAR)
    {
        vx_scalar scalar = (vx_scalar)*item;
        if (scalar->data_type != meta->dim.scalar.type)
        {
            *status = VX_ERROR_INVALID_TYPE;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_TYPE,
                "Scalar contains invalid typed objects for node %s\n", nodes[n]->kernel->name);
            (*num_errors)++;
            return vx_false_e; //break;
        }
    } /* VX_TYPE_SCALAR */
    else if (meta->type == VX_TYPE_MATRIX)
    {
        vx_matrix matrix = (vx_matrix)*item;
        if (matrix->data_type != meta->dim.matrix.type)
        {
            *status = VX_ERROR_INVALID_TYPE;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_TYPE,
                "Node: %s: parameter[%u] has an invalid data type 0x%08x\n",
                nodes[n]->kernel->name, p, matrix->data_type);
            (*num_errors)++;
            return vx_false_e; //break;
        }

        if (matrix->columns != meta->dim.matrix.cols || matrix->rows != meta->dim.matrix.rows)
        {
            *status = VX_ERROR_INVALID_DIMENSION;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_DIMENSION,
                "Node: %s: parameter[%u] has an invalid matrix dimention %ux%u\n",
                nodes[n]->kernel->name, p, matrix->data_type, matrix->rows, matrix->columns);
            (*num_errors)++;
            return vx_false_e; //break;
        }
    } /* VX_TYPE_MATRIX */
    else if (meta->type == VX_TYPE_DISTRIBUTION)
    {
        vx_distribution distribution = (vx_distribution)*item;
        //fix
        if (distribution->offset_x != meta->dim.distribution.offset ||
            distribution->range_x != meta->dim.distribution.range ||
            distribution->memory.dims[0][VX_DIM_X] != meta->dim.distribution.bins)
        {
            *status = VX_ERROR_INVALID_VALUE;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_VALUE,
                "Node: %s: parameter[%u] has an invalid offset %u, number of bins %u or range %u\n",
                nodes[n]->kernel->name, p, distribution->offset_x,
                distribution->memory.dims[0][VX_DIM_X], distribution->range_x);
            (*num_errors)++;
            return vx_false_e; //break;
        }
    } /* VX_TYPE_DISTRIBUTION */
    else if (meta->type == VX_TYPE_REMAP)
    {
        vx_remap remap = (vx_remap)*item;
        if (remap->src_width != meta->dim.remap.src_width || remap->src_height != meta->dim.remap.src_height)
        {
            *status = VX_ERROR_INVALID_DIMENSION;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_DIMENSION,
                "Node: %s: parameter[%u] has an invalid source dimention %ux%u\n",
                nodes[n]->kernel->name, p);
            (*num_errors)++;
            return vx_false_e; //break;
        }

        if (remap->dst_width != meta->dim.remap.dst_width || remap->dst_height != meta->dim.remap.dst_height)
        {
            *status = VX_ERROR_INVALID_DIMENSION;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_DIMENSION,
                "Node: %s: parameter[%u] has an invalid destination dimention %ux%u",
                nodes[n]->kernel->name, p);
            (*num_errors)++;
            return vx_false_e; //break;
        }
    } /* VX_TYPE_REMAP */
    else if (meta->type == VX_TYPE_LUT)
    {
        vx_lut_t lut = (vx_lut_t)*item;
        if (lut->item_type != meta->dim.lut.type || lut->num_items != meta->dim.lut.count)
        {
            *status = VX_ERROR_INVALID_DIMENSION;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_DIMENSION,
                "Node: %s: parameter[%u] has an invalid item type 0x%08x or count " VX_FMT_SIZE "\n",
                nodes[n]->kernel->name, p, lut->item_type, lut->num_items);
            (*num_errors)++;
            return vx_false_e; //break;
        }
    } /* VX_TYPE_LUT */
    else if (meta->type == VX_TYPE_THRESHOLD)
    {
        vx_threshold threshold = (vx_threshold)*item;
        if (threshold->thresh_type != meta->dim.threshold.type)
        {
            *status = VX_ERROR_INVALID_TYPE;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_TYPE,
                "Threshold contains invalid typed objects for node %s\n", nodes[n]->kernel->name);
            (*num_errors)++;
            return vx_false_e; //break;
        }
    } /* VX_TYPE_THRESHOLD */
    else if (meta->type == VX_TYPE_TENSOR)
    {
        vx_tensor tensor = (vx_tensor)*item;
        if (*vref == (vx_reference)tensor)
        {
            VX_PRINT(VX_ZONE_GRAPH, "Creating Tensor From Meta Data!\n");
            if ((tensor->data_type != VX_TYPE_INVALID) &&
                    (tensor->data_type != meta->dim.tensor.data_type || tensor->fixed_point_position != meta->dim.tensor.fixed_point_position))
            {
                *status = VX_ERROR_INVALID_FORMAT;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                    "Node: %s: parameter[%u] has invalid data type %08x or fixed point position %d!\n",
                    nodes[n]->kernel->name, p, tensor->data_type, tensor->fixed_point_position);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has invalid data type %08x or fixed point position %d!\n",
                    nodes[n]->kernel->name, p, tensor->data_type, tensor->fixed_point_position);
                (*num_errors)++;
                return vx_false_e; /* exit on error */
            }
            if (tensor->number_of_dimensions != 0)
            {
               for (unsigned i = 0; i < tensor->number_of_dimensions; i++)
               {
                   if (tensor->dimensions[i] != 0 && tensor->dimensions[i] != meta->dim.tensor.dimensions[i])
                   {
                       *status = VX_ERROR_INVALID_DIMENSION;
                       vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                           "Node: %s: parameter[%u] has invalid dimension size %d in dimension %d!\n",
                           nodes[n]->kernel->name, p, tensor->dimensions[i], i);
                       VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has invalid dimension size %d in dimension %d!\n",
                           nodes[n]->kernel->name, p, tensor->dimensions[i], i);
                       (*num_errors)++;
                       return vx_false_e; /* exit on error */
                   }
               }
            }
            else if (tensor->number_of_dimensions != meta->dim.tensor.number_of_dimensions)
            {
                *status = VX_ERROR_INVALID_DIMENSION;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                    "Node: %s: parameter[%u] has invalid dimension  %d!\n",
                    nodes[n]->kernel->name, p, tensor->number_of_dimensions);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has invalid dimension %d!\n",
                    nodes[n]->kernel->name, p, tensor->number_of_dimensions);
                (*num_errors)++;
                return vx_false_e; /* exit on error */
            }
            tensor->initTensor(meta->dim.tensor.dimensions, meta->dim.tensor.number_of_dimensions, meta->dim.tensor.data_type, meta->dim.tensor.fixed_point_position);
            tensor->allocateTensorMemory();
        }
        else
        {
            if (tensor->number_of_dimensions != meta->dim.tensor.number_of_dimensions)
            {
                *status = VX_ERROR_INVALID_DIMENSION;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                    "Node: %s: parameter[%u] is an invalid number of dimensions %u!\n",
                    nodes[n]->kernel->name, p, tensor->number_of_dimensions);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] is an invalid number of dimensions %u!\n",
                    nodes[n]->kernel->name, p, tensor->number_of_dimensions);
                (*num_errors)++;
                return vx_false_e; /* exit on error */
            }
            for (unsigned i = 0; i < tensor->number_of_dimensions; i++)
            {
                if (tensor->dimensions[i] != meta->dim.tensor.dimensions[i])
                {
                    *status = VX_ERROR_INVALID_DIMENSION;
                    vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                        "Node: %s: parameter[%u] has an invalid dimension %u!\n",
                        nodes[n]->kernel->name, p, tensor->dimensions[i]);
                    VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has an invalid dimension %u!\n",
                        nodes[n]->kernel->name, p, tensor->dimensions[i]);
                    (*num_errors)++;
                    return vx_false_e; /* exit on error */
                }
            }
            if (tensor->data_type != meta->dim.tensor.data_type)
            {
                *status = VX_ERROR_INVALID_FORMAT;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                    "Node: %s: parameter[%u] is an invalid data type %08x!\n",
                    nodes[n]->kernel->name, p, tensor->data_type);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has invalid data type %08x!\n",
                    nodes[n]->kernel->name, p, tensor->data_type);
                (*num_errors)++;
                return vx_false_e; /* exit on error */
            }
            if (tensor->fixed_point_position != meta->dim.tensor.fixed_point_position)
            {
                *status = VX_ERROR_INVALID_FORMAT;
                vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
                    "Node: %s: parameter[%u] has an invalid fixed point position %08x!\n",
                    nodes[n]->kernel->name, p, tensor->fixed_point_position);
                VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has invalid fixed point position  %08x!\n",
                    nodes[n]->kernel->name, p, tensor->fixed_point_position);
                (*num_errors)++;
                return vx_false_e; /* exit on error */
            }
        }
    } /* VX_TYPE_TENSOR */
    /*! \todo support other output types for safety checks in graph verification parameters phase */
    else
    {
        VX_PRINT(VX_ZONE_GRAPH, "Returned Meta type %x\n", meta->type);
    }

    return vx_true_e;
} /* postprocessOutputDataType() */

/*
 * Parameters:
 *   n    - index of node
 *   p    - index of parameter
 *   vref - reference of the parameter
 *   meta - parameter meta info
 */
vx_bool Graph::postprocessOutput(vx_uint32 n, vx_uint32 p, vx_reference* vref, vx_meta_format meta,
                                  vx_status* status, vx_uint32* num_errors)
{
    if (Context::isValidType(meta->type) == vx_false_e)
    {
        *status = VX_ERROR_INVALID_TYPE;
        vxAddLogEntry(reinterpret_cast<vx_reference>(this), *status,
            "Node: %s: parameter[%u] is not a valid type %d!\n",
            nodes[n]->kernel->name, p, meta->type);
        (*num_errors)++;
        return vx_false_e; /* exit on error */
    }

    if (meta->type == VX_TYPE_OBJECT_ARRAY)
    {
        vx_object_array objarr = (vx_object_array)nodes[n]->parameters[p];
        VX_PRINT(VX_ZONE_GRAPH, "meta: type 0x%08x, 0x%08x " VX_FMT_SIZE "\n", meta->type, meta->dim.object_array.item_type, meta->dim.object_array.num_items);

        if (ObjectArray::isValidObjectArray(objarr, meta->dim.object_array.item_type, meta->dim.object_array.num_items) != vx_true_e)
        {
            *status = VX_ERROR_INVALID_DIMENSION;
            vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_DIMENSION,
                "Node: %s: parameter[%u] has an invalid item type 0x%08x or num_items " VX_FMT_SIZE "\n",
                nodes[n]->kernel->name, p, objarr->item_type, objarr->num_items);
            VX_PRINT(VX_ZONE_ERROR, "Node: %s: parameter[%u] has an invalid item type 0x%08x or num_items " VX_FMT_SIZE "\n",
                nodes[n]->kernel->name, p, objarr->item_type, objarr->num_items);
            VX_PRINT(VX_ZONE_ERROR, "Node: %s: Expected item type 0x%08x or num_items " VX_FMT_SIZE "\n",
                nodes[n]->kernel->name, meta->dim.object_array.item_type, meta->dim.object_array.num_items);
            (*num_errors)++;
            return vx_false_e; //break;
        }

        if (vref == (vx_reference*)&objarr)
        {
            VX_PRINT(VX_ZONE_GRAPH, "Creating Object Array From Meta Data %x and " VX_FMT_SIZE "!\n", meta->dim.object_array.item_type, meta->dim.object_array.num_items);
            for (vx_uint32 i = 0; i < meta->dim.object_array.num_items; i++)
            {
                vx_reference item = vxGetObjectArrayItem(objarr, i);

                if (!postprocessOutputDataType(n, p, &item, vref, meta, status, num_errors))
                {
                    vxReleaseReference(&item);
                    *status = VX_ERROR_INVALID_PARAMETERS;
                    vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_PARAMETERS,
                        "Node: %s: meta[%u] has an invalid meta of exemplar\n",
                        nodes[n]->kernel->name, p);
                    VX_PRINT(VX_ZONE_ERROR, "Node: %s: meta[%u] has an invalid meta of exemplar\n",
                        nodes[n]->kernel->name, p);
                    (*num_errors)++;

                    return vx_false_e; //break;
                }

                vxReleaseReference(&item);
            }
        }
        else
        {
            /* check the data that came back from the output validator against the object */
            for (vx_uint32 i = 0; i < meta->dim.object_array.num_items; i++)
            {
                vx_reference item = vxGetObjectArrayItem(objarr, i);
                vx_reference itemref = vxGetObjectArrayItem((vx_object_array)*vref, i);

                if (!postprocessOutputDataType(n, p, &item, &itemref, meta, status, num_errors))
                {
                    vxReleaseReference(&item);
                    *status = VX_ERROR_INVALID_PARAMETERS;
                    vxAddLogEntry(reinterpret_cast<vx_reference>(this), VX_ERROR_INVALID_PARAMETERS,
                        "Node: %s: meta[%u] has an invalid meta of exemplar\n",
                        nodes[n]->kernel->name, p);
                    VX_PRINT(VX_ZONE_ERROR, "Node: %s: meta[%u] has an invalid meta of exemplar\n",
                        nodes[n]->kernel->name, p);
                    (*num_errors)++;

                    return vx_false_e; //break;
                }

                vxReleaseReference(&item);
            }
        }
    }
    else
    {
        return postprocessOutputDataType(n, p, &nodes[n]->parameters[p], vref, meta, status, num_errors);
    }

    return vx_true_e;
} /* postprocessOutput() */

vx_status Graph::pipelineValidateRefsList(
    const vx_graph_parameter_queue_params_t graph_parameters_queue_param)
{
    vx_status status = VX_SUCCESS;
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

                if (Reference::isValidReference(meta, VX_TYPE_META_FORMAT) == vx_true_e)
                {
                    status |= vxReleaseMetaFormat(&meta);
                    if (VX_SUCCESS != status)
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Failed to release meta format object \n");
                    }
                }

                if (VX_SUCCESS != status)
                {
                    break;
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
        status |= vxReleaseMetaFormat(&meta_base);
        if (VX_SUCCESS != status)
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to release meta format object \n");
        }
    }

    return status;
}

void Graph::destruct()
{
    while (numNodes)
    {
        vx_node node = nodes[0];
        /* Interpretation of spec is to release all external references of Nodes when vxReleaseGraph()
           is called AND all graph references count == 0 (garbage collection).
           However, it may be possible that the user would have already released its external reference
           so we need to check. */
        if (nullptr != node)
        {
            if (node->external_count)
            {
                Reference::releaseReference((vx_reference*)&node, VX_TYPE_NODE, VX_EXTERNAL, nullptr);
            }

            if (node)
            {
                node->removeNode();
            }
        }
    }
}

/******************************************************************************/
/* PUBLIC FUNCTIONS */
/******************************************************************************/

VX_API_ENTRY vx_graph VX_API_CALL vxCreateGraph(vx_context context)
{
    vx_graph graph = nullptr;

    if (Context::isValidContext(context) == vx_true_e)
    {
        graph = (vx_graph)Reference::createReference(context, VX_TYPE_GRAPH, VX_EXTERNAL, context);
        if (vxGetStatus((vx_reference)graph) == VX_SUCCESS && graph->type == VX_TYPE_GRAPH)
        {
            Osal::initPerf(&graph->perf);
            Osal::createSem(&graph->lock, 1);
            VX_PRINT(VX_ZONE_GRAPH,"Created Graph %p\n", graph);
            Reference::printReference((vx_reference)graph);
            graph->reverify = graph->verified;
            graph->verified = vx_false_e;
            graph->state = VX_GRAPH_STATE_UNVERIFIED;
        }
    }

    return (vx_graph)graph;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetGraphAttribute(vx_graph graph, vx_enum attribute, const void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    (void)attribute;
    (void)ptr;
    (void)size;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph), VX_TYPE_GRAPH) == vx_true_e)
    {
        /*! @todo there are no settable attributes in this implementation yet... */
        status = VX_ERROR_NOT_SUPPORTED;
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryGraph(vx_graph graph, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph)) == vx_true_e)
    {
        VX_PRINT(VX_ZONE_GRAPH,"INFO: Query:0x%x:%d\n", attribute, (attribute & VX_ATTRIBUTE_ID_MASK));

        switch (attribute)
        {
            case VX_GRAPH_PERFORMANCE:
                if (VX_CHECK_PARAM(ptr, size, vx_perf_t, 0x3))
                {
                    memcpy(ptr, &graph->perf, size);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_GRAPH_STATE:
                if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
                {
                    *(vx_status *)ptr = graph->state;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_GRAPH_NUMNODES:
                if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
                {
                    *(vx_uint32 *)ptr = graph->numNodes;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_GRAPH_NUMPARAMETERS:
                if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
                {
                    *(vx_uint32 *)ptr = graph->numParams;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            default:
                status = VX_ERROR_NOT_SUPPORTED;
                break;
        }
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseGraph(vx_graph *g)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (nullptr != g)
    {
        vx_graph graph = *(g);
        if (Reference::isValidReference(graph, VX_TYPE_GRAPH) == vx_true_e)
        {
            status = Reference::releaseReference((vx_reference*)g, VX_TYPE_GRAPH, VX_EXTERNAL, nullptr);
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxVerifyGraph(vx_graph graph)
{
    vx_status status = VX_SUCCESS;
    vx_uint32 num_errors = 0u;
    vx_bool first_time_verify = ((graph->verified == vx_false_e) && (graph->reverify == vx_false_e)) ? vx_true_e : vx_false_e;

    graph->verified = vx_false_e;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph)) == vx_true_e)
    {
        vx_uint32 h,n,p;
        vx_bool hasACycle = vx_false_e;

        /* lock the graph */
        Osal::semWait(&graph->lock);

        /* To properly deal with parameter dependence in the graph, the
          nodes have to be in topological order when their parameters
          are inspected and their dependent attributes -such as geometry
          and type- are propagated. */
        VX_PRINT(VX_ZONE_GRAPH,"###########################\n");
        VX_PRINT(VX_ZONE_GRAPH,"Topological Sort Phase\n");
        VX_PRINT(VX_ZONE_GRAPH,"###########################\n");
        graph->topologicalSort(graph->nodes, graph->numNodes);

        VX_PRINT(VX_ZONE_GRAPH,"###########################\n");
        VX_PRINT(VX_ZONE_GRAPH,"User Kernel Preprocess Phase! (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"###########################\n");

        for (n = 0; n < graph->numNodes; n++)
        {
            vx_node node = graph->nodes[n];
            if (node->kernel->user_kernel)
            {
                if (!first_time_verify) // re-verify
                {
                    if (node->kernel->deinitialize)
                    {
                        vx_status status;
                        if (node->local_data_set_by_implementation == vx_false_e)
                            node->local_data_change_is_enabled = vx_true_e;
                        status = node->kernel->deinitialize((vx_node)node,
                                                            (vx_reference *)node->parameters,
                                                            node->kernel->signature.num_parameters);
                        node->local_data_change_is_enabled = vx_false_e;
                        if (status != VX_SUCCESS)
                        {
                            VX_PRINT(VX_ZONE_ERROR,"Failed to de-initialize kernel %s!\n", node->kernel->name);
                            goto exit;
                        }
                    }

                    if (node->kernel->attributes.localDataSize == 0)
                    {
                        if (node->attributes.localDataPtr)
                        {
                            if (!first_time_verify && node->attributes.localDataPtr)
                            {
                                ::operator delete(node->attributes.localDataPtr);
                            }
                            node->attributes.localDataSize = 0;
                            node->attributes.localDataPtr = nullptr;
                        }
                    }
                    node->local_data_set_by_implementation = vx_false_e;
                }
            }
        }

        VX_PRINT(VX_ZONE_GRAPH,"###########################\n");
        VX_PRINT(VX_ZONE_GRAPH,"Parameter Validation Phase! (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"###########################\n");

        for (n = 0; n <graph->numNodes; n++)
        {
            /* check to make sure that a node has all required parameters */
            for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
            {
                if (graph->nodes[n]->kernel->signature.states[p] == VX_PARAMETER_STATE_REQUIRED)
                {
                    if (graph->nodes[n]->parameters[p] == nullptr)
                    {
                        vxAddLogEntry(reinterpret_cast<vx_reference>(graph), VX_ERROR_INVALID_PARAMETERS, "Node %s: Some parameters were not supplied!\n", graph->nodes[n]->kernel->name);
                        VX_PRINT(VX_ZONE_ERROR, "Node " VX_FMT_REF " (%s) Parameter[%u] was required and not supplied!\n",
                            graph->nodes[n],
                            graph->nodes[n]->kernel->name,p);
                        status = VX_ERROR_NOT_SUFFICIENT;
                        num_errors++;
                    }
                    else if (graph->nodes[n]->parameters[p]->internal_count == 0)
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Internal reference counts are wrong!\n");
                        DEBUG_BREAK();
                        num_errors++;
                    }
                }
            }
            if (status != VX_SUCCESS)
            {
                goto exit;
            }

            /* debugging, show that we can detect "constant" data or "unreferenced data" */
            for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
            {
                vx_reference ref = (vx_reference)graph->nodes[n]->parameters[p];
                if (ref)
                {
                    if (ref->external_count == 0)
                    {
                        VX_PRINT(VX_ZONE_INFO, "%s[%u] = " VX_FMT_REF " (CONSTANT) type:%08x\n", graph->nodes[n]->kernel->name, p, ref, ref->type);
                    }
                    else
                    {
                        VX_PRINT(VX_ZONE_INFO, "%s[%u] = " VX_FMT_REF " (MUTABLE) type:%08x count:%d\n", graph->nodes[n]->kernel->name, p, ref, ref->type, ref->external_count);
                    }
                }
            }

            /* check if new style validators are provided (see bug14654) */
            if (graph->nodes[n]->kernel->validate != nullptr)
            {
                VX_PRINT(VX_ZONE_GRAPH, "Using new style validators\n");

                vx_status validation_status = VX_SUCCESS;
                vx_reference vref[VX_INT_MAX_PARAMS];
                vx_meta_format metas[VX_INT_MAX_PARAMS];

                for (p = 0; p < dimof(metas); p++)
                {
                    metas[p] = nullptr;
                    vref[p] = nullptr;
                }

                for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
                {
                    if ((graph->nodes[n]->parameters[p] != nullptr) &&
                        (graph->nodes[n]->kernel->signature.directions[p] == VX_OUTPUT))
                    {
                        if (graph->setupOutput(n, p, &vref[p], &metas[p], &status, &num_errors) == vx_false_e)
                        {
                            break;
                        }
                    }
                }

                if (status == VX_SUCCESS)
                {
                    validation_status = graph->nodes[n]->kernel->validate((vx_node)graph->nodes[n],
                                                                          graph->nodes[n]->parameters,
                                                                          graph->nodes[n]->kernel->signature.num_parameters,
                                                                          metas);
                    if (validation_status != VX_SUCCESS)
                    {
                        status = validation_status;
                        vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Node[%u] %s: parameter(s) failed validation!\n",
                                      n, graph->nodes[n]->kernel->name);
                        VX_PRINT(VX_ZONE_GRAPH,"Failed on validation of parameter(s) of kernel %s in node #%d (status=%d)\n",
                                 graph->nodes[n]->kernel->name, n, status);
                        num_errors++;
                    }
                }

                if (status == VX_SUCCESS)
                {
                    for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
                    {
                        if ((graph->nodes[n]->parameters[p] != nullptr) &&
                            (graph->nodes[n]->kernel->signature.directions[p] == VX_OUTPUT))
                        {
                            if (graph->postprocessOutput(n, p, &vref[p], metas[p], &status, &num_errors) == vx_false_e)
                            {
                                break;
                            }
                        }
                    }
                }

                for (p = 0; p < dimof(metas); p++)
                {
                    if (metas[p])
                    {
                        vxReleaseMetaFormat(&metas[p]);
                    }
                }
            }
            else /* old style validators */
            {
                VX_PRINT(VX_ZONE_GRAPH, "Using old style validators\n");
                vx_meta_format metas[VX_INT_MAX_PARAMS] = {nullptr};

                /* first pass for inputs */
                for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
                {
                    if (((graph->nodes[n]->kernel->signature.directions[p] == VX_BIDIRECTIONAL) ||
                         (graph->nodes[n]->kernel->signature.directions[p] == VX_INPUT)) &&
                        (graph->nodes[n]->parameters[p] != nullptr))
                    {
                        vx_status input_validation_status = graph->nodes[n]->kernel->validate_input((vx_node)graph->nodes[n], p);
                        if (input_validation_status != VX_SUCCESS)
                        {
                            status = input_validation_status;
                            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Node[%u] %s: parameter[%u] failed input/bi validation!\n",
                                          n, graph->nodes[n]->kernel->name,
                                          p);
                            VX_PRINT(VX_ZONE_GRAPH,"Failed on validation of parameter %u of kernel %s in node #%d (status=%d)\n",
                                     p, graph->nodes[n]->kernel->name, n, status);
                            num_errors++;
                        }
                    }
                }
                /* second pass for bi/output (we may encounter "virtual" objects here,
                 * then we must reparse graph to replace with new objects)
                 */
                /*! \bug Bidirectional parameters currently break parsing. */
                for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
                {
                    vx_reference vref = nullptr;
                    if (graph->nodes[n]->parameters[p] == nullptr)
                        continue;

                    VX_PRINT(VX_ZONE_GRAPH,"Checking Node[%u].Parameter[%u]\n", n, p);
                    if (graph->nodes[n]->kernel->signature.directions[p] == VX_OUTPUT)
                    {
                        vx_status output_validation_status = VX_SUCCESS;
                        if (graph->setupOutput(n, p, &vref, &metas[p], &status, &num_errors) ==
                            vx_false_e)
                            break;
                        output_validation_status = graph->nodes[n]->kernel->validate_output(
                            (vx_node)graph->nodes[n], p, metas[p]);
                        if (output_validation_status == VX_SUCCESS)
                        {
                            if (graph->postprocessOutput(n, p, &vref, metas[p], &status,
                                                         &num_errors) == vx_false_e)
                            {
                                break;
                            }
                        }
                        else
                        {
                            status = output_validation_status;
                            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Node %s: parameter[%u] failed output validation! (status = %d)\n",
                                          graph->nodes[n]->kernel->name, p, status);
                            VX_PRINT(VX_ZONE_ERROR,"Failed on validation of output parameter[%u] on kernel %s, status=%d\n",
                                     p,
                                     graph->nodes[n]->kernel->name,
                                     status);
                        }
                    }
                }

                for (p = 0; p < dimof(metas); p++)
                {
                    if (metas[p])
                    {
                        vxReleaseMetaFormat(&metas[p]);
                    }
                }
            }
        }

        VX_PRINT(VX_ZONE_GRAPH,"####################\n");
        VX_PRINT(VX_ZONE_GRAPH,"Single Writer Phase! (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"####################\n");

        for (n = 0; (n < graph->numNodes) && (status == VX_SUCCESS); n++)
        {
            for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
            {
                if (graph->nodes[n]->parameters[p] &&
                    ((graph->nodes[n]->kernel->signature.directions[p] == VX_OUTPUT) ||
                     (graph->nodes[n]->kernel->signature.directions[p] == VX_BIDIRECTIONAL)))
                {
                    vx_uint32 n1, p1;
                    /* check for other output references to this parameter in the graph. */
                    for (n1 = vxNextNode(graph, n); n1 != n; n1=vxNextNode(graph, n1))
                    {
                        for (p1 = 0; p1 < graph->nodes[n]->kernel->signature.num_parameters; p1++)
                        {
                            if ((graph->nodes[n1]->kernel->signature.directions[p1] == VX_OUTPUT) ||
                                (graph->nodes[n1]->kernel->signature.directions[p1] == VX_BIDIRECTIONAL))
                            {
                                if (vx_true_e == vxCheckWriteDependency(graph->nodes[n]->parameters[p], graph->nodes[n1]->parameters[p1]))
                                {
                                    status = VX_ERROR_MULTIPLE_WRITERS;
                                    VX_PRINT(VX_ZONE_GRAPH, "Multiple Writer to a reference found, check log!\n");
                                    vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Node %s and Node %s are trying to output to the same reference " VX_FMT_REF "\n", graph->nodes[n]->kernel->name, graph->nodes[n1]->kernel->name, graph->nodes[n]->parameters[p]);
                                }
                            }
                        }
                    }
                }
            }
        }

        VX_PRINT(VX_ZONE_GRAPH,"########################\n");
        VX_PRINT(VX_ZONE_GRAPH,"Memory Allocation Phase! (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"########################\n");

        /* now make sure each parameter is backed by memory. */
        for (n = 0; (n < graph->numNodes) && (status == VX_SUCCESS); n++)
        {
            VX_PRINT(VX_ZONE_GRAPH,"Checking node %u\n",n);

            for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
            {
                if (graph->nodes[n]->parameters[p])
                {
                    VX_PRINT(VX_ZONE_GRAPH,"\tparameter[%u]=%p type %d sig type %d\n", p,
                                 graph->nodes[n]->parameters[p],
                                 graph->nodes[n]->parameters[p]->type,
                                 graph->nodes[n]->kernel->signature.types[p]);

                    if (graph->nodes[n]->parameters[p]->type == VX_TYPE_IMAGE)
                    {
                        if (static_cast<vx_image>(graph->nodes[n]->parameters[p])->allocateImage() == vx_false_e)
                        {
                            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), VX_ERROR_NO_MEMORY, "Failed to allocate image at node[%u] %s parameter[%u]\n",
                                n, graph->nodes[n]->kernel->name, p);
                            VX_PRINT(VX_ZONE_ERROR, "See log\n");
                        }
                    }
                    else if ((VX_TYPE_IS_SCALAR(graph->nodes[n]->parameters[p]->type)) ||
                             (graph->nodes[n]->parameters[p]->type == VX_TYPE_RECTANGLE) ||
                             (graph->nodes[n]->parameters[p]->type == VX_TYPE_THRESHOLD))
                    {
                        /* these objects don't need to be allocated */
                    }
                    else if (graph->nodes[n]->parameters[p]->type == VX_TYPE_LUT)
                    {
                        vx_lut_t lut = (vx_lut_t)graph->nodes[n]->parameters[p];
                        if (Memory::allocateMemory(graph->context, &lut->memory) == vx_false_e)
                        {
                            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), VX_ERROR_NO_MEMORY, "Failed to allocate lut at node[%u] %s parameter[%u]\n",
                                n, graph->nodes[n]->kernel->name, p);
                            VX_PRINT(VX_ZONE_ERROR, "See log\n");
                        }
                    }
                    else if (graph->nodes[n]->parameters[p]->type == VX_TYPE_DISTRIBUTION)
                    {
                        vx_distribution dist = (vx_distribution)graph->nodes[n]->parameters[p];
                        if (Memory::allocateMemory(graph->context, &dist->memory) == vx_false_e)
                        {
                            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), VX_ERROR_NO_MEMORY, "Failed to allocate distribution at node[%u] %s parameter[%u]\n",
                                n, graph->nodes[n]->kernel->name, p);
                            VX_PRINT(VX_ZONE_ERROR, "See log\n");
                        }
                    }
                    else if (graph->nodes[n]->parameters[p]->type == VX_TYPE_PYRAMID)
                    {
                        vx_pyramid pyr = (vx_pyramid)graph->nodes[n]->parameters[p];
                        vx_uint32 i = 0;
                        for (i = 0; i < pyr->numLevels; i++)
                        {
                            if ((pyr->levels[i]->allocateImage()) == vx_false_e)
                            {
                                vxAddLogEntry(reinterpret_cast<vx_reference>(graph), VX_ERROR_NO_MEMORY, "Failed to allocate pyramid image at node[%u] %s parameter[%u]\n",
                                    n, graph->nodes[n]->kernel->name, p);
                                VX_PRINT(VX_ZONE_ERROR, "See log\n");
                            }
                        }
                    }
                    else if ((graph->nodes[n]->parameters[p]->type == VX_TYPE_MATRIX) ||
                              (graph->nodes[n]->parameters[p]->type == VX_TYPE_CONVOLUTION))
                    {
                        vx_matrix mat = (vx_matrix)graph->nodes[n]->parameters[p];
                        if (Memory::allocateMemory(graph->context, &mat->memory) == vx_false_e)
                        {
                            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), VX_ERROR_NO_MEMORY, "Failed to allocate matrix (or subtype) at node[%u] %s parameter[%u]\n",
                                n, graph->nodes[n]->kernel->name, p);
                            VX_PRINT(VX_ZONE_ERROR, "See log\n");
                        }
                    }
                    else if (graph->nodes[n]->kernel->signature.types[p] == VX_TYPE_ARRAY)
                    {
                        if (static_cast<vx_array>(graph->nodes[n]->parameters[p])->allocateArray() == vx_false_e)
                        {
                            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), VX_ERROR_NO_MEMORY, "Failed to allocate array at node[%u] %s parameter[%u]\n",
                                n, graph->nodes[n]->kernel->name, p);
                            VX_PRINT(VX_ZONE_ERROR, "See log\n");
                        }
                    }
                    /*! \todo add other memory objects to graph auto-allocator as needed! */
                }
            }
        }

        VX_PRINT(VX_ZONE_GRAPH,"###############################\n");
        VX_PRINT(VX_ZONE_GRAPH,"Head Nodes Determination Phase! (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"###############################\n");

        memset(graph->heads, 0, sizeof(graph->heads));
        graph->numHeads = 0;

        /* now traverse the graph and put nodes with no predecessor in the head list */
        for (n = 0; (n < graph->numNodes) && (status == VX_SUCCESS); n++)
        {
            uint32_t n1,p1;
            vx_bool isAHead = vx_true_e; /* assume every node is a head until proven otherwise */

            for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters && isAHead == vx_true_e; p++)
            {
                if ((graph->nodes[n]->kernel->signature.directions[p] == VX_INPUT) &&
                    (graph->nodes[n]->parameters[p] != nullptr))
                {
                    /* ring loop over the node array, checking every node but this nth node. */
                    for (n1 = vxNextNode(graph, n);
                        (n1 != n) && (isAHead == vx_true_e);
                        n1 = vxNextNode(graph, n1))
                    {
                        for (p1 = 0; p1 < graph->nodes[n1]->kernel->signature.num_parameters && isAHead == vx_true_e; p1++)
                        {
                            if (graph->nodes[n1]->kernel->signature.directions[p1] != VX_INPUT)
                            {
                                VX_PRINT(VX_ZONE_GRAPH,"Checking input nodes[%u].parameter[%u] to nodes[%u].parameters[%u]\n", n, p, n1, p1);
                                /* if the parameter is referenced elsewhere */
                                if (vxCheckWriteDependency(graph->nodes[n]->parameters[p], graph->nodes[n1]->parameters[p1]))
                                {
                                    /* @TODO: this was added by AI; deep dive this logic */
                                    vx_reference refA = graph->nodes[n]->parameters[p];
                                    vx_reference refB = graph->nodes[n1]->parameters[p1];
                                    if (refA->type == refB->type && refA->delay && refB->delay &&
                                        refA->delay == refB->delay)
                                    {
                                        /* skip delay slot dependency for head node detection */
                                        continue;
                                    }
                                    VX_PRINT(VX_ZONE_GRAPH,"\tnodes[%u].parameter[%u] referenced in nodes[%u].parameter[%u]\n", n,p,n1,p1);
                                    isAHead = vx_false_e; /* this will cause all the loops to break too. */
                                }
                            }
                        }
                    }
                }
            }

            if (isAHead == vx_true_e)
            {
                VX_PRINT(VX_ZONE_GRAPH,"Found a head in node[%u] => %s\n", n, graph->nodes[n]->kernel->name);
                graph->heads[graph->numHeads++] = n;
            }
        }

        /* graph has a cycle as there are no starting points! */
        if ((graph->numHeads == 0) && (status == VX_SUCCESS))
        {
            status = VX_ERROR_INVALID_GRAPH;
            VX_PRINT(VX_ZONE_ERROR,"Graph has no heads!\n");
            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Cycle: Graph has no head nodes!\n");
        }

        VX_PRINT(VX_ZONE_GRAPH,"##############\n");
        VX_PRINT(VX_ZONE_GRAPH,"Cycle Checking (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"##############\n");

        graph->clearVisitation();

        /* cycle checking by traversal of the graph from heads to tails */
        for (h = 0; h < graph->numHeads; h++)
        {
            vx_status cycle_status = VX_SUCCESS;
            status = graph->traverseGraph(VX_INT_MAX_NODES, graph->heads[h]);
            if (cycle_status != VX_SUCCESS)
            {
                status = cycle_status;
                VX_PRINT(VX_ZONE_ERROR,"Cycle found in graph!");
                vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Cycle: Graph has a cycle!\n");
                goto exit;
            }
        }

        VX_PRINT(VX_ZONE_GRAPH,"############################\n");
        VX_PRINT(VX_ZONE_GRAPH,"Checking for Unvisited Nodes (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"############################\n");

        for (n = 0; (n < graph->numNodes) && (status == VX_SUCCESS); n++)
        {
            if (graph->nodes[n]->visited == vx_false_e)
            {
                VX_PRINT(VX_ZONE_ERROR, "UNVISITED: %s node[%u]\n", graph->nodes[n]->kernel->name, n);
                status = VX_ERROR_INVALID_GRAPH;
                vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Node %s: unvisited!\n", graph->nodes[n]->kernel->name);
            }
        }

        graph->clearVisitation();

        if (hasACycle == vx_true_e)
        {
            status = VX_ERROR_INVALID_GRAPH;
            vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Cycle: Graph has a cycle!\n");
            goto exit;
        }

        VX_PRINT(VX_ZONE_GRAPH,"#########################\n");
        VX_PRINT(VX_ZONE_GRAPH,"Target Verification Phase (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"#########################\n");

        for (n = 0; (n < graph->numNodes) && (status == VX_SUCCESS); n++)
        {
            vx_uint32 index = graph->nodes[n]->affinity;
            vx_target target = graph->context->targets[index];
            if (target)
            {
                vx_status target_verify_status = target->funcs.verify(target, graph->nodes[n]);
                if (target_verify_status != VX_SUCCESS)
                {
                    status = target_verify_status;
                    vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Target: %s Failed to Verify Node %s\n", target->name, graph->nodes[n]->kernel->name);
                }
            }
        }

        VX_PRINT(VX_ZONE_GRAPH,"#######################\n");
        VX_PRINT(VX_ZONE_GRAPH,"Kernel Initialize Phase (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"#######################\n");

        for (n = 0; (n < graph->numNodes) && (status == VX_SUCCESS); n++)
        {
            vx_node node = graph->nodes[n];
            if (node->kernel->initialize)
            {
                vx_status kernel_init_status = VX_FAILURE;

                /* call the kernel initialization routine */
                if ((node->kernel->user_kernel == vx_true_e) &&
                    (node->kernel->attributes.localDataSize == 0))
                    node->local_data_change_is_enabled = vx_true_e;

                kernel_init_status = node->kernel->initialize((vx_node)node,
                                                  (vx_reference *)node->parameters,
                                                  node->kernel->signature.num_parameters);
                node->local_data_change_is_enabled = vx_false_e;
                if (kernel_init_status != VX_SUCCESS)
                {
                    status = kernel_init_status;
                    vxAddLogEntry(reinterpret_cast<vx_reference>(graph), status, "Kernel: %s failed to initialize!\n", node->kernel->name);
                }
            }

            /* once the kernel has been initialized, create any local data for it */
            if ((node->attributes.localDataSize > 0) &&
                (node->attributes.localDataPtr == nullptr))
            {
                node->attributes.localDataPtr = new vx_char(node->attributes.localDataSize);
                if (node->kernel->user_kernel == vx_true_e)
                {
                    node->local_data_set_by_implementation = vx_true_e;
                }
                VX_PRINT(VX_ZONE_GRAPH, "Local Data Allocated " VX_FMT_SIZE " bytes for node into %p\n!",
                        node->attributes.localDataSize,
                        node->attributes.localDataPtr);
            }
        }

        VX_PRINT(VX_ZONE_GRAPH,"#######################\n");
        VX_PRINT(VX_ZONE_GRAPH,"COST CALCULATIONS (%d)\n", status);
        VX_PRINT(VX_ZONE_GRAPH,"#######################\n");
        for (n = 0; (n < graph->numNodes) && (status == VX_SUCCESS); n++)
        {
            graph->nodes[n]->costs.bandwidth = 0ul;
            for (p = 0; p < graph->nodes[n]->kernel->signature.num_parameters; p++)
            {
                vx_reference ref = graph->nodes[n]->parameters[p];
                if (ref)
                {
                    vx_uint32 i;
                    switch (ref->type)
                    {
                        case VX_TYPE_IMAGE:
                        {
                            vx_image image = (vx_image)ref;
                            for (i = 0; i < image->memory.nptrs; i++)
                                graph->nodes[n]->costs.bandwidth += Memory::computeMemorySize(&image->memory, i);
                            break;
                        }
                        case VX_TYPE_ARRAY:
                        {
                            vx_array array = (vx_array)ref;
                            graph->nodes[n]->costs.bandwidth += Memory::computeMemorySize(&array->memory, 0);
                            break;
                        }
                        case VX_TYPE_PYRAMID:
                        {
                            vx_pyramid pyr = (vx_pyramid)ref;
                            vx_uint32 j;
                            for (j = 0; j < pyr->numLevels; j++)
                            {
                                vx_image image = pyr->levels[j];
                                for (i = 0; i < image->memory.nptrs; i++)
                                {
                                    graph->nodes[n]->costs.bandwidth += Memory::computeMemorySize(&image->memory, i);
                                }
                            }
                            break;
                        }
                        default:
                            VX_PRINT(VX_ZONE_WARNING, "Node[%u].parameter[%u] Unknown bandwidth cost!\n", n, p);
                            break;
                    }
                }
            }
            VX_PRINT(VX_ZONE_GRAPH, "Node[%u] has bandwidth cost of " VX_FMT_SIZE " bytes\n", n, graph->nodes[n]->costs.bandwidth);
        }

exit:
        graph->reverify = vx_false_e;
        if (status == VX_SUCCESS)
        {
            graph->verified = vx_true_e;
            graph->state = VX_GRAPH_STATE_VERIFIED;
        }
        else
        {
            graph->verified = vx_false_e;
            graph->state = VX_GRAPH_STATE_UNVERIFIED;
        }

        /* unlock the graph */
        Osal::semPost(&graph->lock);
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }
    VX_PRINT(VX_ZONE_GRAPH,"Returning status %d\n", status);

    return status;
}

static vx_status vxExecuteGraph(vx_graph graph, vx_uint32 depth)
{
    vx_status status = VX_SUCCESS;
    vx_action action = VX_ACTION_CONTINUE;
    vx_uint32 n, p, numLast, numNext, numLeft = 0;
    vx_uint32 last_nodes[VX_INT_MAX_REF];
    vx_uint32 next_nodes[VX_INT_MAX_REF];
    vx_uint32 left_nodes[VX_INT_MAX_REF];
    vx_context context = vxGetContext((vx_reference)graph);
    (void)depth;

#if defined(OPENVX_USE_SMP)
    vx_value_set_t workitems[VX_INT_MAX_REF];
#endif
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph)) == vx_false_e)
    {
        return VX_ERROR_INVALID_REFERENCE;
    }

#ifdef OPENVX_USE_PIPELINING
    // Dequeue graph parameters if pipelining is enabled
    if (graph->scheduleMode == VX_GRAPH_SCHEDULE_MODE_QUEUE_AUTO ||
        graph->scheduleMode == VX_GRAPH_SCHEDULE_MODE_QUEUE_MANUAL)
    {
        for (vx_uint32 i = 0; i < graph->numEnqueableParams; i++)
        {
            auto& paramQueue = graph->parameters[i].queue;
            vx_reference ref;

            // Dequeue a reference from the "ready" queue
            if (paramQueue.peekReady(ref))
            {
                vx_node node = graph->parameters[i].node;
                vx_uint32 param_index = graph->parameters[i].index;
                // Save the old reference for this graph parameter
                vx_reference old_ref = node->parameters[param_index];

                // Update ALL node parameters that point to this old reference
                if (node->parameters[param_index] != ref)
                {
                    for (vx_uint32 n = 0; n < graph->numNodes; n++)
                    {
                        for (vx_uint32 p = 0; p < graph->nodes[n]->kernel->signature.num_parameters;
                             p++)
                        {
                            // Assign the dequeued reference to the corresponding node parameter
                            if (graph->nodes[n]->parameters[p] == old_ref)
                            {
                                std::cout << "Replacing node parameter ref "
                                          << node->parameters[param_index]
                                          << " with dequeued reference " << ref << std::endl;
                                graph->context->removeReference(graph->nodes[n]->parameters[p]);
                                ref->incrementReference(VX_INTERNAL);
                                graph->nodes[n]->parameters[p] = ref;
                            }
                        }
                    }
                }

                VX_PRINT(VX_ZONE_GRAPH,
                         "Dequeued reference for graph parameter %u and \
                    assigned to node parameter %u\n",
                         i, param_index);
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Failed to dequeue reference for graph parameter %u\n", i);
                std::cerr << "Failed to dequeue reference for graph parameter " << i << std::endl;
                return VX_ERROR_NO_RESOURCES;
            }
        }
    }
#endif

    if (graph->verified == vx_false_e)
    {
        status = vxVerifyGraph((vx_graph)graph);
        if (status != VX_SUCCESS)
        {
            return status;
        }
    }
    VX_PRINT(VX_ZONE_GRAPH,"************************\n");
    VX_PRINT(VX_ZONE_GRAPH,"*** PROCESSING GRAPH ***\n");
    VX_PRINT(VX_ZONE_GRAPH,"************************\n");

    graph->state = VX_GRAPH_STATE_RUNNING;
    graph->clearVisitation();
    graph->clearExecution();
    if (context->perf_enabled)
    {
        Osal::startCapture(&graph->perf);
    }

    /* initialize the next_nodes as the graph heads */
    memcpy(next_nodes, graph->heads, graph->numHeads * sizeof(vx_uint32));
    numNext = graph->numHeads;

    do {
        for (n = 0; n < numNext; n++)
        {
            Node::printNode(graph->nodes[next_nodes[n]]);
        }

        /* execute the next nodes */
        for (n = 0; n < numNext; n++)
        {
            if (graph->nodes[next_nodes[n]]->executed == vx_false_e)
            {
                vx_uint32 t = graph->nodes[next_nodes[n]]->affinity;
#if defined(OPENVX_USE_SMP)
                if (depth == 1 && graph->shouldSerialize == vx_false_e)
                {
                    vx_value_set_t *work = &workitems[n];
                    vx_target target = graph->context->targets[t];
                    vx_node node = graph->nodes[next_nodes[n]];
                    work->v1 = (vx_value_t)target;
                    work->v2 = (vx_value_t)node;
                    work->v3 = (vx_value_t)VX_ACTION_CONTINUE;
                    VX_PRINT(VX_ZONE_GRAPH, "Scheduling work on %s for %s\n", target->name, node->kernel->name);
                }
                else
#endif
                {
                    vx_target target = graph->context->targets[t];
                    vx_node node = graph->nodes[next_nodes[n]];

                    /* turn on access to virtual memory */
                    for (p = 0u; p < node->kernel->signature.num_parameters; p++)
                    {
                        if (node->parameters[p] == nullptr) continue;
                        if (node->parameters[p]->is_virtual == vx_true_e)
                        {
                            node->parameters[p]->is_accessible = vx_true_e;
                        }
                    }

                    VX_PRINT(VX_ZONE_GRAPH, "Calling Node[%u] %s:%s\n",
                             next_nodes[n],
                             target->name, node->kernel->name);

                    action = target->funcs.process(target, &node, 0, 1);

                    VX_PRINT(VX_ZONE_GRAPH, "Returned Node[%u] %s:%s Action %d\n",
                             next_nodes[n],
                             target->name, node->kernel->name,
                             action);

                    /* turn off access to virtual memory */
                    for (p = 0u; p < node->kernel->signature.num_parameters; p++)
                    {
                        if (node->parameters[p] == nullptr)
                        {
                            continue;
                        }
                        if (node->parameters[p]->is_virtual == vx_true_e)
                        {
                            node->parameters[p]->is_accessible = vx_false_e;
                        }
                    }

#ifdef OPENVX_USE_PIPELINING
                    /* Raise a node completed event. */
                    vx_event_info_t event_info;
                    event_info.node_completed.graph = graph;
                    event_info.node_completed.node = node;
                    if (graph->context->event_queue.isEnabled() &&
                        VX_SUCCESS != graph->context->event_queue.push(VX_EVENT_NODE_COMPLETED, 0,
                                                                       &event_info,
                                                                       (vx_reference)node))
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Failed to push node completed event for node %s\n",
                                 node->kernel->name);
                    }

                    /* Raise a graph parameter consumed event */
                    for (vx_uint32 gp = 0; gp < graph->numEnqueableParams; gp++)
                    {
                        vx_node param_node = graph->parameters[gp].node;
                        vx_uint32 param_index = graph->parameters[gp].index;

                        /* If this node just executed and consumed a graph parameter */
                        if (param_node == node)
                        {
                            vx_event_info_t event_info = {};
                            event_info.graph_parameter_consumed.graph = graph;
                            event_info.graph_parameter_consumed.graph_parameter_index = param_index;

                            (void)graph->parameters[gp].queue.moveReadyToDone();
                            std::cout << "Graph parameter " << gp << " consumed by node "
                                      << node->kernel->name << " at index " << param_index
                                      << std::endl;

                            if (graph->context->event_queue.isEnabled() &&
                                param_node->kernel->signature.directions[param_index] == VX_INPUT &&
                                VX_SUCCESS != graph->context->event_queue.push(
                                                  VX_EVENT_GRAPH_PARAMETER_CONSUMED, 0, &event_info,
                                                  (vx_reference)graph))
                            {
                                VX_PRINT(VX_ZONE_ERROR,
                                         "Failed to push graph parameter consumed event for "
                                         "graph %p, param %u\n",
                                         graph, gp);
                            }
                        }
                    }
#endif

                    if (action == VX_ACTION_ABANDON)
                    {
#ifdef OPENVX_USE_PIPELINING
                        /* Raise a node error event. */
                        vx_event_info_t event_info;
                        event_info.node_error.graph = graph;
                        event_info.node_error.node = node;
                        event_info.node_error.status = node->status;
                        if (graph->context->event_queue.isEnabled() &&
                            VX_SUCCESS != graph->context->event_queue.push(VX_EVENT_NODE_ERROR, 0,
                                                                           &event_info,
                                                                           (vx_reference)node))
                        {
                            VX_PRINT(VX_ZONE_ERROR, "Failed to push node error event for node %s\n",
                                     node->kernel->name);
                        }
#endif
                        break;
                    }
                }
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Multiple executions attempted!\n");
                break;
            }
        }

#if defined(OPENVX_USE_SMP)
        if (depth == 1 && graph->shouldSerialize == vx_false_e)
        {
            if (Osal::issueThreadpool(graph->context->workers, workitems, numNext) == vx_true_e)
            {
                /* do a blocking complete */
                VX_PRINT(VX_ZONE_GRAPH, "Issued %u work items!\n", numNext);
                if (Osal::completeThreadpool(graph->context->workers, vx_true_e) == vx_true_e)
                {
                    VX_PRINT(VX_ZONE_GRAPH, "Processed %u items in threadpool!\n", numNext);
                }
                action = VX_ACTION_CONTINUE;
                for (n = 0; n < numNext; n++)
                {
                    vx_action a = workitems[n].v3;
                    if (a != VX_ACTION_CONTINUE)
                    {
                        action = a;
                        VX_PRINT(VX_ZONE_WARNING, "Workitem[%u] returned action code %d\n", n, a);
                        break;
                    }
                }
            }
        }
#endif

        if (action == VX_ACTION_ABANDON)
        {
            break;
        }

        /* copy next_nodes to last_nodes */
        memcpy(last_nodes, next_nodes, numNext * sizeof(vx_uint32));
        numLast = numNext;

        /* determine the next nodes */
        graph->findNextNodes(last_nodes, numLast, next_nodes, &numNext, left_nodes, &numLeft);

    } while (numNext > 0);

    if (action == VX_ACTION_ABANDON)
    {
        status = VX_ERROR_GRAPH_ABANDONED;
    }
    if (context->perf_enabled)
    {
        Osal::stopCapture(&graph->perf);
    }
    graph->clearVisitation();

    for (n = 0; n < VX_INT_MAX_REF; n++)
    {
        if (graph->delays[n] && Reference::isValidReference(reinterpret_cast<vx_reference>(graph->delays[n]), VX_TYPE_DELAY) == vx_true_e)
        {
            vxAgeDelay(graph->delays[n]);
        }
    }

    VX_PRINT(VX_ZONE_GRAPH,"Process returned status %d\n", status);

#ifdef OPENVX_USE_PIPELINING
    /* Raise a graph completed event. */
    vx_event_info_t event_info;
    event_info.graph_completed.graph = graph;
    if (graph->context->event_queue.isEnabled() &&
        VX_SUCCESS != graph->context->event_queue.push(VX_EVENT_GRAPH_COMPLETED, 0, &event_info,
                                                       (vx_reference)graph))
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to push graph completed event for graph %p\n", graph);
    }
#endif

    // Report the performance of the graph execution.
    if (context->perf_enabled)
    {
        for (n = 0; n < graph->numNodes; n++)
        {
            VX_PRINT(VX_ZONE_PERF,"nodes[%u] %s[%d] last:" VX_FMT_TIME "ms avg:" VX_FMT_TIME "ms min:" VX_FMT_TIME "ms max:" VX_FMT_TIME "\n",
                     n,
                     graph->nodes[n]->kernel->name,
                     graph->nodes[n]->kernel->enumeration,
                     Osal::timeToMS(graph->nodes[n]->perf.tmp),
                     Osal::timeToMS(graph->nodes[n]->perf.avg),
                     Osal::timeToMS(graph->nodes[n]->perf.min),
                     Osal::timeToMS(graph->nodes[n]->perf.max)
            );
        }
    }

    if (status == VX_SUCCESS)
    {
        graph->state = VX_GRAPH_STATE_COMPLETED;
    }
    else
    {
        graph->state = VX_GRAPH_STATE_ABANDONED;
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxScheduleGraph(vx_graph graph)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph)) == vx_false_e)
    {
        return VX_ERROR_INVALID_REFERENCE;
    }

    if (graph->verified == vx_false_e)
    {
        status = vxVerifyGraph((vx_graph)graph);
        if (status != VX_SUCCESS)
        {
            return status;
        }
    }

#ifdef OPENVX_USE_PIPELINING
    vx_uint32 numParams = std::min(graph->numParams, graph->numEnqueableParams);
    vx_size batch_depth = 1u;
    if (graph->scheduleMode == VX_GRAPH_SCHEDULE_MODE_QUEUE_MANUAL)
    {
        batch_depth = UINT32_MAX;  // Use UINT32_MAX to indicate no limit on batch depth
        for (vx_uint32 i = 0; i < numParams; ++i)
        {
            batch_depth = std::min(batch_depth, graph->parameters[i].queue.readyQueueSize());
        }

        if (batch_depth == 0 || batch_depth == UINT32_MAX)
        {
            // Not enough data to schedule a batch
            return VX_ERROR_NOT_SUFFICIENT;
        }
    }

    for (vx_uint32 i = 0; i < batch_depth; i++)
#endif
    // if (Osal::semTryWait(&graph->lock) == vx_true_e)
    {
        Osal::semTryWait(&graph->lock);
        vx_sem_t* p_graph_queue_lock = graph->context->p_global_lock;
        vx_uint32 q = 0u;
        vx_value_set_t *pq = nullptr;

        Osal::semWait(p_graph_queue_lock);
        /* acquire a position in the graph queue */
        for (q = 0; q < dimof(graph_queue); q++)
        {
            if (graph_queue[q].v1 == 0)
            {
                pq = &graph_queue[q];
                numGraphsQueued++;
                break;
            }
        }
        Osal::semPost(p_graph_queue_lock);
        if (pq)
        {
            memset(pq, 0, sizeof(vx_value_set_t));
            pq->v1 = (vx_value_t)graph;

            /* Increment the schedule count */
            graph->scheduleCount++;

            /* now add the graph to the queue */
            VX_PRINT(VX_ZONE_GRAPH,"Writing graph=" VX_FMT_REF ", status=%d\n",graph, status);
            if (Osal::writeQueue(&graph->context->proc.input, pq) == vx_true_e)
            {
                status = VX_SUCCESS;
            }
            else
            {
                Osal::semPost(&graph->lock);
                VX_PRINT(VX_ZONE_ERROR, "Failed to write graph to queue" << std::endl;
                status = VX_ERROR_NO_RESOURCES;
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Graph queue is full\n");
            status = VX_ERROR_NO_RESOURCES;
        }
    }
    // else
    // {
    //     /* graph is already scheduled */
    //     VX_PRINT(VX_ZONE_WARNING, "Graph is already scheduled!\n");
    //     // status = VX_ERROR_GRAPH_SCHEDULED;
    // }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxWaitGraph(vx_graph graph)
{
    vx_status status = VX_SUCCESS;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph)) == vx_false_e)
    {
        return VX_ERROR_INVALID_REFERENCE;
    }

    if (Osal::semTryWait(&graph->lock) == vx_false_e ||
        graph->scheduleMode == VX_GRAPH_SCHEDULE_MODE_QUEUE_MANUAL) /* locked */
    {
        vx_sem_t* p_graph_queue_lock = graph->context->p_global_lock;
        vx_graph g2;
        vx_bool ret = vx_true_e;
        vx_value_set_t *data = nullptr;
        do
        {
            ret = Osal::readQueue(&graph->context->proc.output, &data);
            if (ret == vx_false_e)
            {
                /* graph was locked but the queue was empty... */
                VX_PRINT(VX_ZONE_ERROR, "Queue was empty but graph was locked.\n");
                status = VX_FAILURE;
            }
            else
            {
                g2 = (vx_graph)data->v1;
                status = (vx_status)data->v2;
                if (g2 == graph) /* great, it's the graph we want. */
                {
                    vx_uint32 q = 0u;
                    Osal::semWait(p_graph_queue_lock);
                    /* find graph in the graph queue */
                    for (q = 0; q < dimof(graph_queue); q++)
                    {
                        if (graph_queue[q].v1 == (vx_value_t)graph)
                        {
                            graph_queue[q].v1 = 0;
                            numGraphsQueued--;
                            break;
                        }
                    }
                    Osal::semPost(p_graph_queue_lock);

                    /* Decrement the schedule count */
                    graph->scheduleCount--;

                    /* Unlock the graph only if all scheduled executions are completed */
                    if (graph->scheduleCount == 0)
                    {
                        Osal::semPost(&graph->lock);
                        break;
                    }
                }
                else
                {
                    /* not the right graph, put it back. */
                    Osal::writeQueue(&graph->context->proc.output, data);
                }
            }
        } while (ret == vx_true_e);
        Osal::semPost(&graph->lock); /* unlock the graph. */
    }
    else
    {
        // status = VX_FAILURE;
        Osal::semPost(&graph->lock); /* was free, release */
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxProcessGraph(vx_graph graph)
{
    vx_status status = VX_SUCCESS;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph)) == vx_false_e)
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    if (VX_SUCCESS == status)
    {
        /* create a counter for re-entrancy checking */
        static vx_uint32 count = 0;
        vx_sem_t* p_sem = graph->context->p_global_lock;

        Osal::semWait(p_sem);
        count++;
        Osal::semPost(p_sem);
        status = vxExecuteGraph(graph, count);
        Osal::semWait(p_sem);
        count--;
        Osal::semPost(p_sem);
    }

    VX_PRINT(VX_ZONE_GRAPH, "%s returned %d\n", __func__, status );
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxAddParameterToGraph(vx_graph graph, vx_parameter param)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if ((Reference::isValidReference(reinterpret_cast<vx_reference>(graph), VX_TYPE_GRAPH) == vx_true_e) &&
        (Reference::isValidReference(reinterpret_cast<vx_reference>(param), VX_TYPE_PARAMETER) == vx_true_e))
    {
        graph->parameters[graph->numParams].node = param->node;
        graph->parameters[graph->numParams].index = param->index;
        graph->numParams++;
        status = VX_SUCCESS;
    }
    else if ((Reference::isValidReference(reinterpret_cast<vx_reference>(graph), VX_TYPE_GRAPH) == vx_true_e) &&
             (Reference::isValidReference(reinterpret_cast<vx_reference>(param), VX_TYPE_PARAMETER) == vx_false_e))
    {
        /* insert an empty parameter */
        graph->parameters[graph->numParams].node = nullptr;
        graph->parameters[graph->numParams].index = 0;
        graph->numParams++;
        status = VX_SUCCESS;
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Graph " VX_FMT_REF " was invalid!\n", graph);
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetGraphParameterByIndex(vx_graph graph, vx_uint32 index, vx_reference value)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph), VX_TYPE_GRAPH) == vx_true_e)
    {
        if (index < VX_INT_MAX_PARAMS)
        {
            status = vxSetParameterByIndex((vx_node)graph->parameters[index].node,
                                           graph->parameters[index].index,
                                           value);
        }
        else
        {
            status = VX_ERROR_INVALID_VALUE;
        }
    }

    return status;
}

VX_API_ENTRY vx_parameter VX_API_CALL vxGetGraphParameterByIndex(vx_graph graph, vx_uint32 index)
{
    vx_parameter parameter = nullptr;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph), VX_TYPE_GRAPH) == vx_true_e)
    {
        if ((index < VX_INT_MAX_PARAMS) && (index < graph->numParams))
        {
            vx_uint32 node_index = graph->parameters[index].index;
            parameter = vxGetParameterByIndex((vx_node)graph->parameters[index].node, node_index);
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid Graph!\n");
        vxAddLogEntry(reinterpret_cast<vx_reference>(graph), VX_ERROR_INVALID_REFERENCE, "Invalid Graph given to %s\n", __FUNCTION__);
    }

    return parameter;
}

VX_API_ENTRY vx_bool VX_API_CALL vxIsGraphVerified(vx_graph graph)
{
    vx_bool verified = vx_false_e;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(graph), VX_TYPE_GRAPH) == vx_true_e)
    {
        VX_PRINT(VX_ZONE_GRAPH, "Graph is %sverified\n", (graph->verified == vx_true_e?"":"NOT "));
        verified = graph->verified;
    }

    return verified;
}
