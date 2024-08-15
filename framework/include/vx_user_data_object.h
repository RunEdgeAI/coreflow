/**
 * @file vx_user_data_object.h
 * @author Andrew Mikhail
 * @brief
 * @version 0.1
 * @date 2024-08-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef VX_USER_DATA_OBJECT_H
#define VX_USER_DATA_OBJECT_H

#include "vx_internal.h"

/*! \brief user data object descriptor as placed in shared memory
 * \ingroup group_int_user_data_object
 */
class UserDataObject : public Reference
{
public:
    UserDataObject(/* args */) = default;
    ~UserDataObject() = default;

    /*! \brief Memory layout */
    vx_memory_t memory;
    /*! \brief size of buffer in bytes */
    vx_uint32 size;
    /*! \brief The type name of the user data object. */
    vx_char type_name[VX_MAX_REFERENCE_NAME];
};

#endif /* VX_USER_DATA_OBJECT_H */