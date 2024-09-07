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

#ifndef VX_TARGET_H
#define VX_TARGET_H

#include <VX/vx.h>

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal target implementation.
 * \author Erik Rainey <erik.rainey@gmail.com>
 * \defgroup group_int_target Internal Target API
 * \ingroup group_internal
 * \brief The Internal Target API.
 */

/*! \brief The internal representation of a target.
 * \ingroup group_int_target
 */
class Target : public Reference
{
public:
    /**
     * @brief Construct a new Target object
     *
     * @param context
     * @param scope
     */
    Target(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Target object
     *
     */
    ~Target() = default;

    static void printTarget(vx_target target, vx_uint32 index);

    static vx_bool matchTargetNameWithString(const char* target_name, const char* target_string);

    static const char* reverse_strstr(const char* string, const char* substr);

    vx_status initializeTarget(vx_kernel_description_t *kernels[],
                             vx_uint32 numkernels);

    vx_status deinitializeTarget();

    vx_uint32 findTargetIndex();

    vx_kernel findKernelByEnum(vx_enum enumeration);

    /*! \brief A quick checking method to see if the target is usable. */
    vx_bool             enabled;
    /*! \brief The name of the target */
    vx_char             name[VX_MAX_TARGET_NAME];
    /*! \brief The handle to the module which contains the target interface */
    vx_module_t         module;
    /*! \brief The table of function pointers to target */
    vx_target_funcs_t   funcs;
    /*! \brief Used to determine precidence when more than once core supports a kernel */
    vx_uint32           priority;
    /*! \brief The number of supported kernels on this target */
    vx_uint32           num_kernels;
    /*! \brief The supported kernels on this target */
    vx_kernel         kernels[VX_INT_MAX_KERNELS];
    /*! \brief Target Specific Private Data */
    void               *reserved;
};

#endif /* VX_TARGET_H */
