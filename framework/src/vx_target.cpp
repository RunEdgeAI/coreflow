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

#include "vx_internal.h"

Target::Target(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_TARGET, scope)
{
}

void Target::printTarget(vx_target target, vx_uint32 index)
{
    if (target)
    {
        Reference::printReference(target);
        VX_PRINT(VX_ZONE_TARGET, "Target[%u]=>%s\n", index, target->name);
    }
}

vx_status Target::initializeTarget(vx_kernel_description_t* kernel_desc[],
                             vx_uint32 numkernels)
{
    vx_uint32 k = 0u;
    vx_status status = VX_FAILURE;
    for (k = 0u; k < numkernels; k++)
    {
        kernels[k] = reinterpret_cast<vx_kernel>(Reference::createReference(context, VX_TYPE_KERNEL, VX_INTERNAL, context));
        VX_PRINT(VX_ZONE_TARGET, "kernel is valid ? %d\n", Reference::isValidReference((vx_reference)kernels[k], VX_TYPE_KERNEL));

        status = kernels[k]->initializeKernel(kernel_desc[k]->enumeration,
                                   kernel_desc[k]->function,
                                   kernel_desc[k]->name,
                                   kernel_desc[k]->parameters,
                                   kernel_desc[k]->numParams,
                                   kernel_desc[k]->validate,
                                   kernel_desc[k]->initialize,
                                   kernel_desc[k]->deinitialize);
        VX_PRINT(VX_ZONE_KERNEL, "Initialized Kernel %s, %d\n", kernels[k]->name, status);

        if (status != VX_SUCCESS) {
            break;
        }

        num_kernels++;
        status = vxFinalizeKernel(kernels[k]);

        if (status != VX_SUCCESS) {
            break;
        }
    }
    return status;
}

vx_status Target::deinitializeTarget()
{
    vx_uint32 k = 0u;
    vx_status status = VX_SUCCESS;
    vx_kernel kernel = nullptr;

    for (k = 0u; k < num_kernels; k++)
    {
        kernel = kernels[k];

        if ((kernel->enabled != vx_false_e) ||
            (kernel->enumeration != VX_KERNEL_INVALID))
        {
            kernel->enabled = vx_false_e;

            if (Kernel::isKernelUnique(kernels[k]) == vx_true_e)
            {
                context->num_unique_kernels--;
            }

            if (kernel->deinitializeKernel() != VX_SUCCESS )
            {
                status = VX_FAILURE;
            }
        }
    }

    context->num_kernels -= num_kernels;
    num_kernels = 0;

    return status;
}

vx_bool Target::matchTargetNameWithString(const char* target_name, const char* target_string)
{
    /* 1. find latest occurrence of target_string in target_name;
       2. match only the cases: target_name == "[smth.]<target_string>[.smth]"
     */
    const char dot = '.';
    vx_bool match = vx_false_e;
    const char* ptr = Target::reverse_strstr(target_name, target_string);
    if (ptr != nullptr)
    {
        vx_size name_len = strlen(target_name);
        vx_size string_len = strlen(target_string);
        vx_size begin = (vx_size)(ptr - target_name);
        vx_size end = begin + string_len;
        if ((!(begin > 0) || ((begin > 0) && (target_name[begin-1] == dot))) ||
            (!(end < name_len) || ((end < name_len) && (target_name[end] == dot))))
        {
            match = vx_true_e;
        }
    }
    return match;
}

const char* Target::reverse_strstr(const char* string, const char* substr)
{
    const char* last = nullptr;
    const char* cur = string;
    do {
        cur = (const char*) strstr(cur, substr);
        if (cur != nullptr)
        {
            last = cur;
            cur = cur+1;
        }
    } while (cur != nullptr);
    return last;
}

vx_uint32 vxFindTargetIndex(vx_target target)
{
    vx_uint32 t = 0u;
    for (t = 0u; t < target->context->num_kernels; t++)
    {
        if (target == target->context->targets[t])
        {
            break;
        }
    }
    return t;
}

/******************************************************************************/
/* PUBLIC API */
/******************************************************************************/


