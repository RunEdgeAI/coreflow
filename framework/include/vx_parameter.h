/*
 * Copyright (c) 2012-2017 The Khronos Group Inc. *
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

#ifndef VX_PARAMETER_H
#define VX_PARAMETER_H

#include <VX/vx.h>

/*!
 * \file
 * \brief The internal parameter implementation
 *
 * \defgroup group_int_parameter Internal Parameter API
 * \ingroup group_internal
 * \brief The Internal Parameter API
 */

/*! \brief The internal representation of a parameter.
 * \ingroup group_int_parameter
 */
namespace corevx {

class Parameter : public Reference
{
public:
    /**
     * @brief Construct a new Parameter object
     *
     * @param context
     * @param ref
     * @ingroup group_int_parameter
     */
    Parameter(vx_context context, vx_reference ref);

    /**
     * @brief Destroy the Parameter object
     * @ingroup group_int_parameter
     */
    ~Parameter();

    /**
     * @brief Function to destruct the Parameter object
     * @ingroup group_int_parameter
     */
    void destruct() override final;

    /*! \brief This returns true if the direction is a valid enum
     * \param [in] dir The \ref vx_direction_e enum.
     * \ingroup group_int_parameter
     */
    static vx_bool isValidDirection(vx_enum dir);

    /*! \brief This returns true if the supplied type matches the expected type with
     * some fuzzy rules.
     * \ingroup group_int_parameter
     */
    static vx_bool isValidTypeMatch(vx_enum expected, vx_enum supplied);

    /*! \brief This returns true if the supplied state is a valid enum.
     * \ingroup group_int_parameter
     */
    static vx_bool isValidState(vx_enum state);

    /**
     * @brief Returns the direction of the parameter
     *
     * @return vx_enum The direction of the parameter: VX_INPUT, VX_OUTPUT, or VX_BIDIRECTIONAL
     * @ingroup group_int_parameter
     */
    vx_enum direction() const;

    /**
     * @brief Returns the index of the parameter
     *
     * @return vx_uint32 The index of the parameter
     * @ingroup group_int_parameter
     */
    vx_uint32 idx() const;

    /**
     * @brief Returns the data type of the parameter
     *
     * @return vx_enum The type of the parameter
     * @ingroup group_int_parameter
     */
    vx_enum dataType() const;

    /**
     * @brief Returns the state of the parameter
     *
     * @return vx_enum The state of the parameter: VX_PARAMETER_STATE_REQUIRED or
     * VX_PARAMETER_STATE_OPTIONAL
     * @ingroup group_int_parameter
     */
    vx_enum state() const;

    /**
     * @brief Returns the reference referred to by the parameter
     *
     * @return vx_reference The reference contained in the parameter
     * @ingroup group_int_parameter
     */
    vx_reference ref() const;

    /**
     * @brief Returns the meta format contained in the parameter
     *
     * @return vx_meta_format The meta format contained in the parameter
     * @ingroup group_int_parameter
     */
    vx_meta_format metaFormat() const;

    /**
     * @brief Get a parameter by index from a kernel
     *
     * @param kernel The kernel to get the parameter from
     * @param index  The index of the parameter
     * @return vx_parameter The parameter object, or nullptr if not found
     * @ingroup group_int_parameter
     */
    static vx_parameter getKernelParameterByIndex(vx_kernel kernel, vx_uint32 index);

    /**
     * @brief Get a parameter by index from a node
     *
     * @param node  The node to get the parameter from
     * @param index The index of the parameter
     * @return vx_parameter The parameter object, or nullptr if not found
     * @ingroup group_int_parameter
     */
    static vx_parameter getParameterByIndex(vx_node node, vx_uint32 index);

    /**
     * @brief Set a parameter by index on a node
     *
     * @param node   The node to set the parameter on
     * @param index  The index of the parameter
     * @param value  The reference to set as the parameter value
     * @return vx_status VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_parameter
     */
    static vx_status setParameterByIndex(vx_node node, vx_uint32 index, vx_reference value);

    /**
     * @brief Get a parameter by index from a node
     *
     * @param node  The node to get the parameter from
     * @param index The index of the parameter
     * @return vx_parameter The parameter object, or nullptr if not found
     * @ingroup group_int_parameter
     */
    vx_status setParameterByReference(vx_reference value);

    /*! \brief Index at which this parameter is tracked in both the node references and kernel signatures */
    vx_uint32      index;
    /*! \brief Pointer to the node which this parameter is associated with */
    vx_node        node;
    /*! \brief Pointer to the kernel which this parameter is associated with, if retreived from
     * \ref vxGetKernelParameterByIndex.
     */
    vx_kernel      kernel;
};

} // namespace corevx

// Temporary global alias during namespace migration
using corevx::Parameter;

#endif /* VX_PARAMETER_H */
