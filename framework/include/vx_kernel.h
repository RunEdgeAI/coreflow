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

#ifndef VX_KERNEL_H
#define VX_KERNEL_H

#include "vx_internal.h"
#include "vx_reference.h"

/*!
 * \file
 * \brief The internal kernel implementation.
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_kernel Internal Kernel API
 * \ingroup group_internal
 * \brief The internal Kernel API.
 */

/*! \brief The internal representation of an abstract kernel.
 * \ingroup group_int_kernel
 */
class Kernel : public Reference
{
public:
    /**
     * @brief Construct a new Kernel object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_kernel
     */
    Kernel(vx_context context, vx_reference scope);

    /**
     * @brief Construct a new Kernel object
     *
     * @param context
     * @param kenum
     * @param function
     * @param name
     * @param parameters
     * @param numParams
     * @param scope
     * @ingroup group_int_kernel
     */
    Kernel(vx_context context,
            vx_enum kenum,
            vx_kernel_f function,
            vx_char name[VX_MAX_KERNEL_NAME],
            vx_param_description_t *parameters,
            vx_uint32 numParams,
            vx_reference scope);

    /**
     * @brief Destroy the Kernel object
     * @ingroup group_int_kernel
     */
    ~Kernel() = default;

    /*! \brief Determines if a kernel is unique in the system.
     * \param kernel The handle to the kernel.
     * \ingroup group_int_kernel
     */
    static vx_bool isKernelUnique(vx_kernel kernel);

    /*! \brief Used to initialize a kernel object in a target kernel list.
     * \param [in] kenum The kernel enumeration value.
     * \param [in] function The pointer to the function of the kernel.
     * \param [in] name The name of the kernel in dotted notation.
     * \param [in] parameters The list of parameters for each kernel.
     * \param [in] numParams The number of parameters in the list.
     * \param [in] validator The function pointer to the params validator.
     * \param [in] input_validator The function pointer to the input validator.
     * \param [in] output_validator The function pointer to the output validator.
     * \param [in] initialize The function to call to initialize the kernel.
     * \param [in] deinitialize The function to call to deinitialize the kernel.
     * \ingroup group_int_kernel
     */
    vx_status initializeKernel(vx_enum kenum,
                             vx_kernel_f function,
                             vx_char name[VX_MAX_KERNEL_NAME],
                             vx_param_description_t *parameters,
                             vx_uint32 numParams,
                             vx_kernel_validate_f validator,
                             vx_kernel_input_validate_f input_validator,
                             vx_kernel_output_validate_f output_validator,
                             vx_kernel_initialize_f initialize,
                             vx_kernel_deinitialize_f deinitialize);

    /**
     * @brief Add Kernel
     *
     * @param context           The global context
     * @param name              The kernel name value.
     * @param enumeration       The kernel enumeration value.
     * @param func_ptr          The pointer to the function of the kernel.
     * @param numParams         The number of parameters in the kernel.
     * @param validate          The function pointer to the params validator.
     * @param input             The function pointer to the input validator.
     * @param output            The function pointer to the output validator.
     * @param initialize        The function to call to initialize the kernel.
     * @param deinitialize      The function to call to deinitialize the kernel.
     * @param valid_rect_reset  The bool to reset the valid rect or not.
     * @return vx_kernel
     * @ingroup group_int_kernel
     */
    static vx_kernel addkernel(vx_context context,
                           const vx_char name[VX_MAX_KERNEL_NAME],
                           vx_enum enumeration,
                           vx_kernel_f func_ptr,
                           vx_uint32 numParams,
                           vx_kernel_validate_f validate,
                           vx_kernel_input_validate_f input,
                           vx_kernel_output_validate_f output,
                           vx_kernel_initialize_f initialize,
                           vx_kernel_deinitialize_f deinitialize,
                           vx_bool valid_rect_reset);

    /*! \brief Used to deinitialize a kernel object in a target kernel list.
     * \ingroup group_int_kernel
     */
    vx_status deinitializeKernel();

    /**
     * @brief Print kernel object
     *
     * @param kernel
     * @ingroup group_int_kernel
     */
    static void printKernel(vx_kernel kernel);

    /*! \brief */
    vx_char        name[VX_MAX_KERNEL_NAME];
    /*! \brief */
    vx_enum        enumeration;
    /*! \brief */
    vx_kernel_f    function;
    /*! \brief */
    vx_signature_t signature;
    /*! Indicates that the kernel is not yet enabled. */
    vx_bool        enabled;
    /*! Indicates that this kernel is added by user. */
    vx_bool        user_kernel;
    /*! \brief */
    vx_kernel_validate_f validate;
    /*! \brief */
    vx_kernel_input_validate_f validate_input;
    /*! \brief */
    vx_kernel_output_validate_f validate_output;
    /*! \brief */
    vx_kernel_initialize_f initialize;
    /*! \brief */
    vx_kernel_deinitialize_f deinitialize;
    /*! \brief The collection of attributes of a kernel */
    vx_kernel_attr_t attributes;
    /*! \brief Target Index, back reference for the later nodes to inherit affinity */
    vx_uint32 affinity;
#ifdef OPENVX_KHR_TILING
    /*! \brief The tiling function pointer interface */
    vx_tiling_kernel_f tilingfast_function;
    vx_tiling_kernel_f tilingflexible_function;
#endif
    /*! \brief The pointer to the kernel object deinitializer. */
    vx_kernel_object_deinitialize_f kernel_object_deinitialize;
    /*! \brief The kernel's input depth required to start */
    vx_kernel_attribute_streaming_e input_depth;
    /*! \brief The kernel's output depth required to start */
    vx_kernel_attribute_streaming_e output_depth;
};

#endif /* VX_KERNEL_H */
