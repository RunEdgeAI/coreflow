/**
 * @file vx_delay.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef VX_DELAY_H
#define VX_DELAY_H

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal Delay implementation
 *
 * \defgroup group_int_delay Internal Delay API
 * \ingroup group_internal
 * \brief The Internal Delay API.
 */

/*! \brief The internal representation of any delay object.
 * \ingroup group_int_delay
 */
namespace corevx {

class Delay : public Reference
{
public:
    /**
     * @brief Construct a new Delay object
     *
     * @param context
     * @param scope
     * @ingroup group_int_delay
     */
    Delay(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Delay object
     * @ingroup group_int_delay
     */
    ~Delay();

    /**
     * @brief Get data type of references associated with this delay object
     *
     * @return vx_enum The data type of references.
     * @ingroup group_int_delay
     */
    vx_enum dataType() const;

    /**
     * @brief Get the number of objects in this delay object
     *
     * @return vx_size The number of objects in this delay object
     * @ingroup group_int_delay
     */
    vx_size numObjects() const;

    /**
     * @brief Retrieves a reference to a delay slot object.
     *
     * @param [in] index    The index of the delay slot from which to extract the object reference.
     * @return vx_reference The vx_reference at the index specified if found, otherwise an error object
     * @ingroup group_int_delay
     */
    vx_reference getReference(vx_int32 index);

    /*! \brief Removes an association to a node from a delay slot object reference.
     * \param [in] value The delay slot object reference.
     * \param [in] n The node reference.
     * \param [in] i The index of the parameter.
     * \ingroup group_int_delay
     */
    static vx_bool removeAssociationToDelay(vx_reference value, vx_node n, vx_uint32 i);

    /*! \brief Adds an association to a node to a delay slot object reference.
     * \param [in] value The delay slot object reference.
     * \param [in] n The node reference.
     * \param [in] i The index of the parameter.
     * \ingroup group_int_delay
     */
    static vx_bool addAssociationToDelay(vx_reference value, vx_node n, vx_uint32 i);

    /**
     * @brief Shifts the internal delay ring by one.
     *
     * This function performs a shift of the internal delay ring by one. This means that,
     * the data originally at index 0 move to index -1 and so forth until index
     * \f$ -count+1 \f$. The data originally at index \f$ -count+1 \f$ move to index 0.
     *  Here \f$ count \f$ is the number of slots in delay ring.
     * When a delay is aged, any graph making use of this delay (delay object itself or data
     * objects in delay slots) gets its data automatically updated accordingly.
     *
     * @return vx_status VX_SUCCESS if successful, any other value indicates failure.
     * @ingroup group_int_delay
     */
    vx_status age();

    /**
     * @brief Register a delay for auto-aging.
     *
     * This function registers a delay object to be auto-aged by the graph.
     * This delay object will be automatically aged after each successful completion of
     * this graph. Aging of a delay object cannot be called during graph execution.
     * A graph abandoned due to a node callback will trigger an auto-aging.
     *
     * If a delay is registered for auto-aging multiple times in a same graph,
     * the delay will be only aged a single time at each graph completion.
     * If a delay is registered for auto-aging in multiple graphs, this delay will
     * aged automatically after each successful completion of any of these graphs.
     *
     * @param [in] graph      The graph to which the delay is registered for auto-aging.
     * @return vx_status VX_SUCCESS if successful, any other value indicates failure.
     */
    vx_status registerAutoAging(vx_graph graph);

    /**
     * @brief Destruct function for delay objects
     * @ingroup group_int_delay
     */
    void destruct() override final;

    /*! \brief The number of objects in the delay. */
    vx_size count;
    /*! \brief The current index which is '0' */
    vx_uint32 index;
    /*! \brief Object Type in the Delay. */
    vx_enum type;
    /*! \brief The set of Nodes each object is associated with in the delay. */
    vx_delay_param_t* set;
    /*! \brief The set of objects in the delay. */
    vx_reference* refs;
    /*! \brief The set of delays for pyramid levels. */
    vx_delay* pyr;
};

} // namespace corevx

// Temporary global alias during namespace migration
using corevx::Delay;

#endif /* VX_DELAY_H */