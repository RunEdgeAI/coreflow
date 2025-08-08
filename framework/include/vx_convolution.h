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

#ifndef VX_CONVOLUTION_H
#define VX_CONVOLUTION_H

#include "vx_internal.h"
#include "vx_matrix.h"

/*!
 * \file
 * \brief The internal convolution implementation
 *
 * \defgroup group_int_convolution Internal Convolution API
 * \ingroup group_internal
 * \brief The Internal Convolution API.
 */

/*! \brief A convolution is a special type of matrix (MxM)
 * \ingroup group_int_convolution
 */
namespace corevx {

class Convolution : public Matrix
{
public:
    /**
     * @brief Construct a new Convolution object
     *
     * @param context   The context associated with object
     * @param scope     The parent ref of this object
     * @ingroup group_int_convolution
     */
    Convolution(vx_context context, vx_reference scope);

    /**
     * @brief Get the scale factor of the convolution
     *
     * @return vx_uint32 The scale factor
     * @ingroup group_int_convolution
     */
    vx_uint32 scaleFactor() const;

    /**
     * @brief Get the size of the convolution in bytes
     *
     * @return vx_size The size in bytes
     * @ingroup group_int_convolution
     */
    vx_size size() const;

    /**
     * @brief Set the scale factor of the convolution
     *
     * @param value The scale factor to set
     * @ingroup group_int_convolution
     */
    void setScale(vx_uint32 value);

    /**
     * @brief Read the coefficients of the convolution
     *
     * @param array      The array to read coefficients into
     * @return vx_status The status of the operation
     * @ingroup group_int_convolution
     */
    vx_status readCoefficients(vx_int16 *array);

    /**
     * @brief Write the coefficients of the convolution
     *
     * @param array      The array containing coefficients to write
     * @return vx_status The status of the operation
     * @ingroup group_int_convolution
     */
    vx_status writeCoefficients(const vx_int16 *array);

    /**
     * @brief Copy the coefficients to a specified memory location
     *
     * @param ptr        Pointer to the memory location
     * @param usage      Memory usage type
     * @param mem_type   Memory type (e.g., host, device)
     * @return vx_status The status of the operation
     * @ingroup group_int_convolution
     */
    vx_status copyCoefficients(void *ptr, vx_enum usage, vx_enum mem_type);

    /**
     * @brief Destroy the Convolution object
     * @ingroup group_int_convolution
     */
    ~Convolution();

    /*! \brief Destroys a convolution.
     * \ingroup group_int_convolution
     */
    void destruct() override final;

    /*! \brief The Scale Factor.
     * \ingroup group_int_convolution
     */
    vx_uint32 scale;
};

} // namespace corevx

// Temporary global alias during namespace migration
using corevx::Convolution;

#endif /* VX_CONVOLUTION_H */
