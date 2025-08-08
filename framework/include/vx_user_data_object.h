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
namespace corevx {

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
     * @brief Get the type name of the user data object
     *
     * @return vx_char* The type name
     * @ingroup group_int_user_data_object
     */
    const vx_char *typeName() const;

    /**
     * @brief Get the size of the user data object
     *
     * @return vx_size The size in bytes
     * @ingroup group_int_user_data_object
     */
    vx_size objSize() const;

    /**
     * @brief Allocate user data object
     *
     * @return vx_bool      vx_true_e if allocated | otherwise vx_false_e
     * @ingroup group_int_user_data_object
     */
    vx_bool allocateUserDataObject();

    /**
     * @brief Copy data to/from user memory
     *
     * @param offset       The offset in bytes
     * @param size         The size in bytes
     * @param user_ptr     The pointer to the user memory
     * @param usage        The usage of the memory (read/write)
     * @param user_mem_type The type of memory (host, opencl, etc.)
     * @return vx_status   VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_user_data_object
     */
    vx_status copy(vx_size offset, vx_size size, void *user_ptr, vx_enum usage,
                   vx_enum user_mem_type);

    /**
     * @brief Map the user data object for access
     *
     * @param offset       The offset in bytes
     * @param size         The size in bytes
     * @param map_id       The map ID to be used
     * @param ptr          Pointer to the mapped memory
     * @param usage        Memory usage type (read/write)
     * @param mem_type     Memory type (host, opencl, etc.)
     * @param flags        Additional flags for mapping
     * @return vx_status   VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_user_data_object
     */
    vx_status map(vx_size offset, vx_size size, vx_map_id *map_id, void **ptr, vx_enum usage,
                  vx_enum mem_type, vx_uint32 flags);

    /**
     * @brief Unmap the user data object from access
     *
     * @param map_id       The map ID to be used
     * @return vx_status   VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_user_data_object
     */
    vx_status unmap(vx_map_id map_id);

    /*! \brief Memory layout */
    vx_memory_t memory;
    /*! \brief size of buffer in bytes */
    vx_uint32 size;
    /*! \brief The type name of the user data object. */
    vx_char type_name[VX_MAX_REFERENCE_NAME];
};

} // namespace corevx


#endif /* VX_USER_DATA_OBJECT_H */