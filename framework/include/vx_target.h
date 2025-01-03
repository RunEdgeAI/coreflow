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
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_target
     */
    Target(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Target object
     * @ingroup group_int_target
     */
    ~Target() = default;

    /*! \brief Prints Target Information for Debugging.
     * \ingroup group_int_target
     */
    static void printTarget(vx_target target, vx_uint32 index);

    /*! \brief Match target name with specified target string.
     * \param [in] target_name The target name string.
     * \param [in] target_string The target string.
     * \ingroup group_int_target
     * \retval vx_true_e If string matches, vx_false_e if not.
     */
    static vx_bool matchTargetNameWithString(const char* target_name, const char* target_string);

    /**
     * @brief Find the last occurrence of a substring in a string.
     *
     * @param string The string to search in.
     * @param substr The substring to search for.
     * @return const char* Pointer to the last occurrence of the substring, or nullptr if not found.
     * @ingroup group_int_target
     */
    static const char* reverse_strstr(const char* string, const char* substr);

    /*! \brief Initializes a target's kernels list.
     * \param [in] kernels The array of kernels that the target supports.
     * \param [in] numkernels The length of the kernels list.
     * \ingroup group_int_target
     */
    vx_status initializeTarget(vx_kernel_description_t *kernels[],
                             vx_uint32 numkernels);

    /*! \brief Deinitializes a target's kernels list.
     * \ingroup group_int_target
     */
    vx_status deinitializeTarget();

    /**
     * @brief Find target's index within context
     *
     * @return vx_uint32    index of target
     * @ingroup group_int_target
     */
    vx_uint32 findTargetIndex();

    /**
     * @brief Find kernel belonging to this target module by enum
     *
     * @param enumeration   enum of kernel to search for
     * @return vx_kernel    kernel found or nullptr
     * @ingroup group_int_target
     */
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
