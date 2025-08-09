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

#ifndef VX_SCALAR_H
#define VX_SCALAR_H

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal scalars implementation.
 * \defgroup group_int_scalar Internal Scalar API
 * \ingroup group_internal
 * \brief The Internal Scalar API.
 */

/*! \brief The internal representation of a scalar value.
 * \ingroup group_int_scalar
 */
namespace corevx {

class Scalar : public Reference
{
public:
    /**
     * @brief Construct a new Scalar object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_scalar
     */
    Scalar(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Scalar object
     * @ingroup group_int_scalar
     */
    ~Scalar();

    /**
     * @brief Create a scalar object
     *
     * @param context   The context associated with this obj
     * @param data_type The data type of the scalar
     * @param ptr       The pointer to the scalar value
     * @return vx_scalar The scalar object
     * @ingroup group_int_scalar
     */
    static vx_scalar createScalar(vx_context context, vx_enum data_type, const void* ptr);

    /**
     * @brief   Allocate memory for scalar obj
     *
     * @param size      size of bytes to alloc
     * @return void*    pointer to allocated memory
     * @ingroup group_int_scalar
     */
    void* allocateScalarMemory(vx_size size);

    /**
     * @brief   Get host memory from scalar obj
     *
     * @param scalar    scalar obj
     * @param user_ptr  pointer to user memory
     * @return vx_status VX_SUCCESS on success
     * @ingroup group_int_scalar
     */
    static vx_status scalarToHostMem(vx_scalar scalar, void* user_ptr);

    /**
     * @brief Get scalar from host memory
     *
     * @param scalar   scalar obj
     * @param user_ptr pointer to user memory
     * @return vx_status VX_SUCCESS on success
     * @ingroup group_int_scalar
     */
    static vx_status hostMemToScalar(vx_scalar scalar, void* user_ptr);

    /**
     * @brief Copy scalar value to/from user memory
     *
     * @param user_ptr       pointer to user memory
     * @param usage          usage of the memory (read/write)
     * @param user_mem_type  type of memory (host, opencl, etc.)
     * @return vx_status VX_SUCCESS on success
     * @ingroup group_int_scalar
     */
    vx_status copy(void* user_ptr, vx_enum usage, vx_enum user_mem_type);

    /**
     * @brief Copy scalar value, given size, to/from user memory
     *
     * @param size          size of the scalar data
     * @param user_ptr      pointer to user memory
     * @param usage         usage of the memory (read/write)
     * @param user_mem_type type of memory (host, opencl, etc.)
     * @return vx_status VX_SUCCESS on success
     * @ingroup group_int_scalar
     */
    vx_status copy(vx_size size, void* user_ptr, vx_enum usage, vx_enum user_mem_type);

    /**
     * @brief Read the scalar value
     *
     * @param ptr Pointer to the memory location to read the value into
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_scalar
     */
    vx_status readValue(void* ptr);

    /**
     * @brief Write the scalar value
     *
     * @param ptr Pointer to the memory location containing the value to write
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_scalar
     */
    vx_status writeValue(const void* ptr);

    /**
     * @brief Get the data type of the scalar
     *
     * @return vx_enum The data type of the scalar
     * @ingroup group_int_scalar
     */
    vx_enum dataType() const;

    /**
     * @brief Print scalar object
     *
     * @param scalar scalar obj
     * @ingroup group_int_scalar
     */
    static void printScalarValue(vx_scalar scalar);

    /**
     * @brief Function to destroy scalar obj
     * @ingroup group_int_scalar
     */
    void destruct() override final;

    /*! \brief The atomic type of the scalar */
    vx_enum               data_type;
    /*! \brief The value contained in the reference for a scalar type */
    union {
        /*! \brief A character */
        vx_char   chr;
        /*! \brief Signed 8 bit */
        vx_int8   s08;
        /*! \brief Unsigned 8 bit */
        vx_uint8  u08;
        /*! \brief Signed 16 bit */
        vx_int16  s16;
        /*! \brief Unsigned 16 bit */
        vx_uint16 u16;
        /*! \brief Signed 32 bit */
        vx_int32  s32;
        /*! \brief Unsigned 32 bit */
        vx_uint32 u32;
        /*! \brief Signed 64 bit */
        vx_int64  s64;
        /*! \brief Unsigned 64 bit */
        vx_uint64  u64;
#if defined(EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT)
        /*! \brief 16 bit float */
        vx_float16 f16;
#endif
        /*! \brief 32 bit float */
        vx_float32 f32;
        /*! \brief 64 bit float */
        vx_float64 f64;
        /*! \brief 32 bit image format code */
        vx_df_image  fcc;
        /*! \brief Signed 32 bit*/
        vx_enum    enm;
        /*! \brief Architecture depth unsigned value */
        vx_size    size;
        /*! \brief Boolean Values */
        vx_bool    boolean;
    } data;
    /*! \brief Only used to scalar with size */
    void *data_addr;
    /*! \brief The length of pointer data_addr */
    vx_size data_len;
};

} // namespace corevx

#endif /* VX_SCALAR_H */