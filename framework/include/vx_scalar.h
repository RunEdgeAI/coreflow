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

#include <VX/vx.h>
#include "vx_internal.h"

/*!
 * \file
 * \brief The internal scalars implementation.
 * \author Erik Rainey <erik.rainey@gmail.com>
 * \defgroup group_int_scalar Internal Scalar API
 * \ingroup group_internal
 * \brief The Internal Scalar API.
 */

/*! \brief The internal representation of a scalar value.
 * \ingroup group_int_scalar
 */
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
     * @brief   Allocate memory for scalar obj
     *
     * @param size      size of bytes to alloc
     * @return void*    pointer to allocated memory
     * @ingroup group_int_scalar
     */
    void* allocateScalarMemory(vx_size size);

    /**
     * @brief Print scalar object
     *
     * @param scalar
     * @ingroup group_int_scalar
     */
    static void printScalarValue(vx_scalar scalar);

    /**
     * @brief Function to destroy scalar obj
     * @ingroup group_int_scalar
     */
    void destructScalar();

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
        vx_int64  u64;
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

#endif /* VX_SCALAR_H */
