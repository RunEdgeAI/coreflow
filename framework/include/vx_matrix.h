/*
 * Copyright (c) 2012-2017 The Khronos Group Inc.
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

#ifndef VX_MATRIX_H
#define VX_MATRIX_H

#include <VX/vx.h>

/*!
 * \file
 * \brief The internal matrix implementation
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_matrix Internal Matrix API
 * \ingroup group_internal
 * \brief The Internal Matrix API.
 */

/*! \brief The internal matrix structure.
 * \ingroup group_int_matrix
 */
class Matrix : public Reference
{
public:
    /**
     * @brief Construct a new Matrix object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_matrix
     */
    Matrix(vx_context context, vx_reference scope);

    /**
     * @brief Construct a new Matrix object
     *
     * @param context   The context associated with this obj
     * @param type      The data type of matrix
     * @param scope     The parent ref of this obj
     * @ingroup group_int_matrix
     */
    Matrix(vx_context context, vx_type_e type, vx_reference scope);

    /**
     * @brief Destroy the Matrix object
     * @ingroup group_int_matrix
     */
    ~Matrix();

    /*! \brief Destroys a matrix objects
     * \ingroup group_int_matrix
     */
    void destruct() override;

    /*! \brief Memory Layout */
    vx_memory_t memory;
    /*! \brief From \ref vx_type_e */
    vx_enum data_type;
    /*! \brief Number of columns */
    vx_size columns;
    /*! \brief Number of rows */
    vx_size rows;
    /*! \brief Origin */
    vx_coordinates2d_t origin;
    /*! \brief Pattern */
    vx_enum pattern;
};

#endif /* VX_MATRIX_H */
