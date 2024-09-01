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

/*! \brief The internal representation of any delay object.
 * \ingroup group_int_delay
 */
class Delay : public Reference
{
public:
    Delay(vx_context context, vx_reference scope);
    ~Delay() = default;

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