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

/*!
 * \file
 * \brief The internal user data object implementation
 *
 * \defgroup group_int_user_data_object Internal User Data Object API
 * \ingroup group_internal
 * \brief The Internal User Data Object API.
 */

/*! \brief user data object descriptor as placed in shared memory
 * \ingroup group_int_user_data_object
 */
class UserDataObject : public Reference
{
public:
    /**
     * @brief Construct a new User Data Object object
     *
     * @param context    The context associated with this obj
     * @param scope      The parent ref of this obj
     * @ingroup group_int_user_data_object
     */
    UserDataObject(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the User Data Object object
     * @ingroup group_int_user_data_object
     */
    ~UserDataObject();

    /**
     * @brief Allocate user data object
     *
     * @return vx_bool      vx_true_e if allocated | otherwise vx_false_e
     * @ingroup group_int_user_data_object
     */
    vx_bool allocateUserDataObject();

    /*! \brief Memory layout */
    vx_memory_t memory;
    /*! \brief size of buffer in bytes */
    vx_uint32 size;
    /*! \brief The type name of the user data object. */
    vx_char type_name[VX_MAX_REFERENCE_NAME];
};

#endif /* VX_USER_DATA_OBJECT_H */