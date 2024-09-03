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

#include <VX/vx.h>

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
    Kernel(vx_context context, vx_reference scope);

    Kernel(vx_context context,
            vx_enum kenum,
            vx_kernel_f function,
            vx_char name[VX_MAX_KERNEL_NAME],
            vx_param_description_t *parameters,
            vx_uint32 numParams,
            vx_reference scope);

    ~Kernel() = default;

    static vx_bool isKernelUnique(vx_kernel kernel);

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

    vx_status deinitializeKernel();

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
    /*! \brief The pointer to the kernel object deinitializer. */
    vx_kernel_object_deinitialize_f kernel_object_deinitialize;
};

#endif /* VX_KERNEL_H */
