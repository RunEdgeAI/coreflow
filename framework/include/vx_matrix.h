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
 *
 * \defgroup group_int_matrix Internal Matrix API
 * \ingroup group_internal
 * \brief The Internal Matrix API.
 */

/*! \brief The internal matrix structure.
 * \ingroup group_int_matrix
 */
namespace coreflow {

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
     * @brief Create a matrix object
     *
     * @param context   The context associated with this obj
     * @param data_type The data type of the matrix
     * @param columns   The number of columns in the matrix
     * @param rows      The number of rows in the matrix
     * @return vx_matrix The matrix object
     * @ingroup group_int_matrix
     */
    static vx_matrix createMatrix(vx_context context, vx_enum data_type, vx_size columns, vx_size rows);

    /**
     * @brief Get the data type of the matrix
     *
     * @return vx_enum The data type of the matrix
     * @ingroup group_int_matrix
     */
    vx_enum dataType() const;

    /**
     * @brief Get the number of rows in the matrix.
     *
     * @return vx_size The number of rows in the matrix.
     * @ingroup group_int_matrix
     */
    vx_size numRows() const;

    /**
     * @brief Get the number of columns in the matrix.
     *
     * @return vx_size The number of columns in the matrix.
     * @ingroup group_int_matrix
     */
    vx_size numCols() const;

    /**
     * @brief Get the origin coordinates of the matrix.
     *
     * @return vx_coordinates2d_t The origin coordinates of the matrix.
     * @ingroup group_int_matrix
     */
    vx_coordinates2d_t originCoord() const;

    /**
     * @brief Get the pattern type of the matrix.
     *
     * @return vx_enum The pattern type of the matrix.
     * @ingroup group_int_matrix
     */
    vx_enum patternType() const;

    /**
     * @brief Read the matrix data into an array.
     *
     * @param array The array to read the matrix data into.
     * @return vx_status The status of the operation.
     * @ingroup group_int_matrix
     */
    vx_status read(void *array);

    /**
     * @brief Write the matrix data from an array.
     *
     * @param array The array containing the matrix data to write.
     * @return vx_status The status of the operation.
     * @ingroup group_int_matrix
     */
    vx_status write(const void *array);

    /**
     * @brief Copy the matrix data to/from a pointer.
     *
     * @param ptr The pointer to copy the matrix data to/from.
     * @param usage The usage flags for the memory operation.
     * @param mem_type The type of memory used (e.g., OpenCL buffer).
     * @return vx_status The status of the operation.
     * @ingroup group_int_matrix
     */
    vx_status copy(void *ptr, vx_enum usage, vx_enum mem_type);

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
    /*! \brief Origin Coordinates */
    vx_coordinates2d_t origin;
    /*! \brief Pattern */
    vx_enum pattern;
};

} // namespace coreflow

#endif /* VX_MATRIX_H */