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

#endif /* VX_DELAY_H */