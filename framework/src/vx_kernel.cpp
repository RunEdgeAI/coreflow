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

#include "vx_internal.h"
#include "vx_kernel.h"

Kernel::Kernel(vx_context context, vx_reference scope) : Reference(context, VX_TYPE_KERNEL, scope)
{
}

Kernel::Kernel(vx_context context,
            vx_enum kenum,
            vx_kernel_f function,
            vx_char name[VX_MAX_KERNEL_NAME],
            vx_param_description_t *parameters,
            vx_uint32 numParams,
            vx_reference scope) : Reference(context, VX_TYPE_KERNEL, context)
{
    vx_kernel kernel = (vx_kernel)Reference::createReference(context, VX_TYPE_KERNEL, VX_INTERNAL, context);
    {
        /* setup the kernel meta-data */
        strncpy(this->name, name, VX_MAX_KERNEL_NAME - 1);
        this->function = function;
        enumeration = kenum;
        signature.num_parameters = numParams;
        attributes.borders.mode = VX_BORDER_UNDEFINED;
        if (signature.num_parameters <= VX_INT_MAX_PARAMS)
        {
            vx_uint32 p = 0;
            if (parameters != nullptr)
            {
                for (p = 0; p < numParams; p++)
                {
                    signature.directions[p] = parameters[p].direction;
                    signature.types[p] = parameters[p].data_type;
                    /* Initialize to nullptr, kernel import function can create meta format for each param if it is called */
                    signature.meta_formats[p] = nullptr;
                }
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid num parameters on kernel signature! Max supported [%d]", VX_INT_MAX_PARAMS);
        }
    }
}

void vxPrintKernel(vx_kernel kernel)
{
    VX_PRINT(VX_ZONE_KERNEL, "kernel[%u] enabled?=%s %s \n",
            kernel->enumeration,
            (kernel->enabled?"TRUE":"FALSE"),
            kernel->name);
}

vx_bool Kernel::isKernelUnique(vx_kernel kernel)
{
    vx_uint32 t = 0u, k = 0u;
    vx_context context = kernel->context;
    vx_bool unique = vx_true_e;


    for (t = 0u; t < context->num_targets; t++)
    {
        for (k = 0u; k < VX_INT_MAX_KERNELS; k++)
        {
            if (context->targets[t]->kernels[k] &&
                context->targets[t]->kernels[k]->enabled &&
                context->targets[t]->kernels[k]->enumeration == kernel->enumeration)
                {
                unique = vx_false_e;
                break;
            }
        }
    }

    if (unique == vx_true_e)
    {
        VX_PRINT(VX_ZONE_KERNEL, "Kernel %s (%x) is unique!\n", kernel->name, kernel->enumeration);
    }

    return unique;
}

vx_status Kernel::initializeKernel(vx_enum kenum,
                             vx_kernel_f function,
                             vx_char name[VX_MAX_KERNEL_NAME],
                             vx_param_description_t* parameters,
                             vx_uint32 numParams,
                             vx_kernel_validate_f validator,
                             vx_kernel_input_validate_f input_validator,
                             vx_kernel_output_validate_f output_validator,
                             vx_kernel_initialize_f initialize,
                             vx_kernel_deinitialize_f deinitialize)
{
    {
        incrementReference(VX_INTERNAL);

        // setup the kernel meta-data
        strncpy(this->name, name, VX_MAX_KERNEL_NAME - 1);
        enumeration = kenum;
        this->function = function;
        signature.num_parameters = numParams;
        validate = validator;
        validate_input = input_validator;
        validate_output = output_validator;
        this->initialize = initialize;
        this->deinitialize = deinitialize;
        attributes.borders.mode = VX_BORDER_UNDEFINED;
        attributes.borders.constant_value.U32 = 0;
        attributes.valid_rect_reset = vx_false_e; /* default value for std nodes */
        attributes.localDataSize = 0;
#ifdef OPENVX_USE_OPENCL_INTEROP
        attributes.opencl_access = vx_false_e;
#endif
        if (signature.num_parameters <= VX_INT_MAX_PARAMS)
        {
            vx_uint32 p = 0;
            if (parameters != nullptr)
            {
                for (p = 0; p < numParams; p++)
                {
                    signature.directions[p] = parameters[p].direction;
                    signature.types[p] = parameters[p].data_type;
                    signature.states[p] = parameters[p].state;
                    /* Initialize to nullptr, kernel import function can create meta format for each param if it is called */
                    signature.meta_formats[p] = nullptr;
                }
                return VX_SUCCESS;
            }
            else
            {
                return VX_FAILURE;
            }
        }
        else
        {
            return VX_FAILURE;
        }
    }
}

vx_status Kernel::deinitializeKernel()
{
    vx_status status = VX_SUCCESS;

    VX_PRINT(VX_ZONE_KERNEL, "Releasing kernel " VX_FMT_REF "\n", (void *)this);
    status = releaseReference(VX_TYPE_KERNEL, VX_INTERNAL, nullptr);

    return status;
}

static vx_size strnindex(const vx_char *str, vx_char c, vx_size limit)
{
    vx_size index = 0;
    while (index < limit && *str != c)
    {
        if(!*str)
        {
            index = limit;
            break;
        }
        str++;
        index++;
    }
    return index;
}

static vx_size strncount(const vx_char string[], vx_size size, vx_char c)
{
    vx_size i = 0ul, count = 0ul;
    while (string[i] != '\0' && i < size)
        if (string[i++] == c)
            count++;
    return count;
}

/******************************************************************************/
/* PUBLIC FUNCTIONS */
/******************************************************************************/

VX_API_ENTRY vx_status VX_API_CALL vxLoadKernels(vx_context context, const vx_char *name)
{
    vx_status status = VX_FAILURE;
    vx_char module[VX_INT_MAX_PATH];
    vx_uint32 m = 0;
    vx_publish_kernels_f publish = nullptr;

    snprintf(module, VX_INT_MAX_PATH, VX_MODULE_NAME("%s"), (name?name:"openvx-ext"));
    VX_PRINT(VX_ZONE_INFO, "Attempting to load module: %s\n", module);

    if (Context::isValidContext(context) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Context is invalid!\n");
        return VX_ERROR_INVALID_REFERENCE;
    }

    for (m = 0; m < VX_INT_MAX_MODULES; m++)
    {
        ownSemWait(&context->modules[m].lock);
        if (context->modules[m].handle != nullptr && strncmp(name, context->modules[m].name, VX_INT_MAX_PATH) == 0)
        {
            context->modules[m].ref_count++;
            ownSemPost(&context->modules[m].lock);
            return VX_SUCCESS;
        }
        ownSemPost(&context->modules[m].lock);
    }

    for (m = 0; m < VX_INT_MAX_MODULES; m++)
    {
        ownSemWait(&context->modules[m].lock);
        if (context->modules[m].handle == nullptr)
        {
            context->modules[m].handle = ownLoadModule(module);
            if (context->modules[m].handle)
            {
                vx_symbol_t sym = ownGetSymbol(context->modules[m].handle, "vxPublishKernels");
                publish = (vx_publish_kernels_f)sym;
                if (publish == nullptr)
                {
                    VX_PRINT(VX_ZONE_ERROR, "Failed to load symbol vxPublishKernels\n");
                    status = VX_ERROR_INVALID_MODULE;
                    ownUnloadModule(context->modules[m].handle);
                    context->modules[m].handle = nullptr;
                }
                else
                {
                    VX_PRINT(VX_ZONE_INFO, "Calling %s publish function\n", module);
                    status = publish((vx_context)context);
                    if (status != VX_SUCCESS)
                    {
                        VX_PRINT(VX_ZONE_ERROR, "Failed to publish kernels in module\n");
                        ownUnloadModule(context->modules[m].handle);
                        context->modules[m].handle = nullptr;
                    }
                    else
                    {
                        strncpy(context->modules[m].name, name, VX_INT_MAX_PATH - 1);
                        context->modules[m].ref_count = 1;
                        context->num_modules++;
                    }
                }
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Failed to find module %s in libraries path\n", module);
            }
            ownSemPost(&context->modules[m].lock);
            break;
        }
        else
        {
            VX_PRINT(VX_ZONE_CONTEXT, "module[%u] is used\n", m);
        }
        ownSemPost(&context->modules[m].lock);
    }
    if (status != VX_SUCCESS)
    {
        VX_PRINT(VX_ZONE_ERROR, "Failed to load module %s; error %d\n", module, status);
    }
    else
    {
        for (m = 0; m < context->num_modules; m++)
        {
            VX_PRINT(VX_ZONE_INFO, "Module: %s\n", context->modules[m].name);
        }
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxUnloadKernels(vx_context context, const vx_char *name)
{
    vx_status status = VX_FAILURE;
    vx_char module[VX_INT_MAX_PATH];
    vx_uint32 m = 0;
    vx_unpublish_kernels_f unpublish = nullptr;

    snprintf(module, VX_INT_MAX_PATH, VX_MODULE_NAME("%s"), (name?name:"openvx-ext"));

    if (Context::isValidContext(context) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Context is invalid!\n");
        return VX_ERROR_INVALID_REFERENCE;
    }

    for (m = 0; m < VX_INT_MAX_MODULES; m++)
    {
        ownSemWait(&context->modules[m].lock);
        if (context->modules[m].handle != nullptr && strncmp(name, context->modules[m].name, VX_INT_MAX_PATH) == 0)
        {
            context->modules[m].ref_count--;
            if (context->modules[m].ref_count != 0)
            {
                ownSemPost(&context->modules[m].lock);
                return VX_SUCCESS;
            }

            vx_symbol_t sym = ownGetSymbol(context->modules[m].handle, "vxUnpublishKernels");
            unpublish = (vx_unpublish_kernels_f)sym;
            if (unpublish == nullptr)
            {
                VX_PRINT(VX_ZONE_ERROR, "Failed to load symbol vxUnpublishKernels\n");
                status = VX_ERROR_INVALID_MODULE;
            }
            else
            {
                VX_PRINT(VX_ZONE_INFO, "Calling %s unpublish function\n", module);
                status = unpublish((vx_context)context);
                if (status != VX_SUCCESS)
                {
                    VX_PRINT(VX_ZONE_ERROR, "Failed to unpublish kernels in module\n");
                }
                else
                {
                    ownUnloadModule(context->modules[m].handle);
                    context->modules[m].handle = nullptr;
                    context->num_modules--;
                    ownSemPost(&context->modules[m].lock);
                    return VX_SUCCESS;
                }
            }
        }
        ownSemPost(&context->modules[m].lock);
    }

    VX_PRINT(VX_ZONE_ERROR, "Failed to find module %s in libraries path\n", module);

    return status;
}

VX_API_ENTRY vx_kernel VX_API_CALL vxGetKernelByName(vx_context context, const vx_char string[VX_MAX_KERNEL_NAME])
{
    vx_kernel kern = nullptr;
    if (Context::isValidContext(context) == vx_true_e)
    {
        vx_uint32 k = 0u, t = 0u;
        vx_size colons = strncount(string, VX_MAX_KERNEL_NAME, ':');
        vx_char targetName[VX_MAX_TARGET_NAME] = "default";
        vx_char kernelName[VX_MAX_KERNEL_NAME];
#if defined(_WIN32)
        vx_char *nameBuffer = _strdup(string);
#else
        vx_char *nameBuffer = strdup(string);
#endif

        if (colons == 0)
        {
            strncpy(kernelName, string, VX_MAX_KERNEL_NAME - 1);
        }
        else
        {
            /* There should be no colon */
            /* Doing nothing will leave kern = nullptr, causing error condition below */
            VX_PRINT(VX_ZONE_ERROR, "Kernel name should not contain any ':' in this implementation\n");
        }

        free(nameBuffer);

        for (t = 0; t < context->num_targets && kern == nullptr; t++)
        {
            vx_target target = context->targets[context->priority_targets[t]];
            if (target == nullptr || target->enabled == vx_false_e)
            {
                continue;
            }
            if (target->funcs.supports(target, targetName, kernelName, &k) == VX_SUCCESS)
            {
                vx_kernel kernel = target->kernels[k];
                vxPrintKernel(kernel);
                if (kernel->enabled == vx_true_e)
                {
                    kernel->affinity = context->priority_targets[t];
                    kern = kernel;
                    kern->incrementReference(VX_EXTERNAL);
                    break;
                }
            }
        }

        if (kern == nullptr)
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to find kernel %s\n", string);
            vxAddLogEntry(reinterpret_cast<vx_reference>(context), VX_ERROR_INVALID_PARAMETERS, "Failed to find kernel %s\n", string);
            // kern = (vx_kernel_t *)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
        }
        else
        {
            VX_PRINT(VX_ZONE_KERNEL,"Found Kernel enum %d, name %s on target %s\n",
                kern->enumeration,
                kern->name,
                context->targets[kern->affinity]->name);
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid context %p\n", context);
    }
    return (vx_kernel)kern;
}

VX_API_ENTRY vx_kernel VX_API_CALL vxGetKernelByEnum(vx_context context, vx_enum kernelenum)
{
    vx_kernel kernel = nullptr;
    if (Context::isValidContext(context) == vx_true_e)
    {
        vx_uint32 k = 0u, t = 0u;
        VX_PRINT(VX_ZONE_KERNEL,"Scanning for kernel enum %d out of %d kernels\n", kernelenum, context->num_kernels);
        for (t = 0; t < context->num_targets; t++)
        {
            vx_target target = context->targets[context->priority_targets[t]];
            if (target == nullptr || target->enabled == vx_false_e)
            {
                VX_PRINT(VX_ZONE_KERNEL, "Target[%u] is not valid!\n", t);
                continue;
            }
            VX_PRINT(VX_ZONE_KERNEL, "Checking Target[%u]=%s for %u kernels\n", context->priority_targets[t], target->name, target->num_kernels);
            for (k = 0; k < VX_INT_MAX_KERNELS; k++)
            {
                if (target->kernels[k]->enumeration == kernelenum)
                {
                    kernel = target->kernels[k];
                    kernel->affinity = context->priority_targets[t];
                    kernel->incrementReference(VX_EXTERNAL);
                    VX_PRINT(VX_ZONE_KERNEL,"Found Kernel[%u] enum:%d name:%s in target[%u]=%s\n", k, kernelenum, kernel->name, context->priority_targets[t], target->name);
                    break;
                }
            }
            /* Acquire the highest priority target */
            if (kernel != nullptr)
                break;
        }

        if (kernel == nullptr)
        {
            VX_PRINT(VX_ZONE_KERNEL, "Kernel enum %x not found.\n", kernelenum);
            vxAddLogEntry(reinterpret_cast<vx_reference>(context), VX_ERROR_INVALID_PARAMETERS, "Kernel enum %x not found.\n", kernelenum);
            // kernel = (vx_kernel_t *)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
        }

    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid context %p\n", context);
    }
    return kernel;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseKernel(vx_kernel *kernel)
{
    vx_status status = VX_SUCCESS;
    if (kernel && Reference::isValidReference(reinterpret_cast<vx_reference>(*kernel), VX_TYPE_KERNEL) == vx_true_e)
    {
        VX_PRINT(VX_ZONE_KERNEL, "Releasing kernel " VX_FMT_REF "\n", (void *)*kernel);

        // deinitialize kernel object
        if ((*kernel)->kernel_object_deinitialize != nullptr)
        {
            (*kernel)->kernel_object_deinitialize(*kernel);
        }

        (*(kernel))->releaseReference(VX_TYPE_KERNEL, VX_EXTERNAL, nullptr);
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
        VX_PRINT(VX_ZONE_ERROR, "Invalid Reference!\n");
    }
    return status;
}

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
                           vx_bool valid_rect_reset)
{
    vx_kernel kernel = 0;
    vx_uint32 t = 0;
    vx_size index = 0;
    vx_target target = nullptr;
    vx_char targetName[VX_MAX_TARGET_NAME];

    VX_PRINT(VX_ZONE_INFO, "Entered %s\n", __func__);

    if (Context::isValidContext(context) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid Context\n");
        return (vx_kernel)nullptr;
    }

    if (func_ptr == nullptr ||
        ((validate == nullptr) &&
         (input == nullptr ||
          output == nullptr)) ||
        numParams > VX_INT_MAX_PARAMS || numParams == 0 ||
        name == nullptr ||
        strncmp(name, "",  VX_MAX_KERNEL_NAME) == 0)
        /* initialize and de-initialize can be nullptr */
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid Parameters!\n");
        vxAddLogEntry((vx_reference)context, VX_ERROR_INVALID_PARAMETERS, "Invalid Parameters supplied to vxAddKernel or vxAddUserKernel\n");
        // kernel = (vx_kernel_t *)ownGetErrorObject(context, VX_ERROR_INVALID_PARAMETERS);
        return kernel;
    }

    /* find target to assign this to */
    index = strnindex(name, ':', VX_MAX_TARGET_NAME);
    if (index == VX_MAX_TARGET_NAME)
    {
        strcpy(targetName,"khronos.any");
    }
    else
    {
        strncpy(targetName, name, index);
    }
    VX_PRINT(VX_ZONE_KERNEL, "Deduced Name as %s\n", targetName);
    for (t = 0u; t < context->num_targets; t++)
    {
        target = context->targets[t];
        if (strncmp(targetName,target->name, VX_MAX_TARGET_NAME) == 0)
        {
            break;
        }
        target = nullptr;
    }
    if (target && target->funcs.addkernel)
    {
        kernel = target->funcs.addkernel(target, name, enumeration,
                                         func_ptr, numParams,
                                         validate, input, output,
                                         initialize, deinitialize);
        if (kernel)
        {
            kernel->user_kernel = vx_true_e;
            kernel->attributes.valid_rect_reset = valid_rect_reset;
            VX_PRINT(VX_ZONE_KERNEL,"Added Kernel %s to Target %s (" VX_FMT_REF ")\n", name, target->name, kernel);
            /* A reference is returned to the user */
            kernel->incrementReference(VX_EXTERNAL);
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to add kernel %s to target %s\n", name, target->name);
            kernel = nullptr;
        }
    }
    else
    {
        vxAddLogEntry((vx_reference)context, VX_ERROR_NO_RESOURCES, "No target named %s exists!\n", targetName);
        // kernel = (vx_kernel_t *)ownGetErrorObject(context, VX_ERROR_NO_RESOURCES);
    }

    return kernel;
}

/*
 *  add std-extra kernels
 */
VX_API_ENTRY vx_kernel VX_API_CALL vxAddKernel(vx_context c,
                             const vx_char name[VX_MAX_KERNEL_NAME],
                             vx_enum enumeration,
                             vx_kernel_f func_ptr,
                             vx_uint32 numParams,
                             vx_kernel_input_validate_f input,
                             vx_kernel_output_validate_f output,
                             vx_kernel_initialize_f initialize,
                             vx_kernel_deinitialize_f deinitialize)
{
    return addkernel(c, name, enumeration, func_ptr, numParams,
                     nullptr, input, output, initialize, deinitialize,
                     vx_false_e);
}

/*
 * add user kernels
 */
VX_API_ENTRY vx_kernel VX_API_CALL vxAddUserKernel(vx_context c,
                             const vx_char name[VX_MAX_KERNEL_NAME],
                             vx_enum enumeration,
                             vx_kernel_f func_ptr,
                             vx_uint32 numParams,
                             vx_kernel_validate_f validate,
                             vx_kernel_initialize_f initialize,
                             vx_kernel_deinitialize_f deinitialize)
{
    return addkernel(c, name, enumeration, func_ptr, numParams,
                     validate, nullptr, nullptr, initialize, deinitialize,
                     vx_true_e);
}

VX_API_ENTRY vx_status VX_API_CALL vxFinalizeKernel(vx_kernel kernel)
{
    vx_status status = VX_SUCCESS;
    if (kernel && Reference::isValidReference(reinterpret_cast<vx_reference>(kernel), VX_TYPE_KERNEL) == vx_true_e)
    {
        vx_uint32 p = 0;
        for (p = 0; p < VX_INT_MAX_PARAMS; p++)
        {
            if (p >= kernel->signature.num_parameters)
            {
                break;
            }
            if ((kernel->signature.directions[p] < VX_INPUT) ||
                (kernel->signature.directions[p] > VX_BIDIRECTIONAL))
            {
                status = VX_ERROR_INVALID_PARAMETERS;
                break;
            }
            if (Context::isValidType(kernel->signature.types[p]) == vx_false_e)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
                break;
            }
        }
        if (p == kernel->signature.num_parameters)
        {
            kernel->context->num_kernels++;
            if (Kernel::isKernelUnique(kernel) == vx_true_e)
            {
                VX_PRINT(VX_ZONE_KERNEL, "Kernel %s (%x) is unique!\n", kernel->name, kernel->enumeration);
                kernel->context->num_unique_kernels++;
            }
            kernel->enabled = vx_true_e;
        }
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryKernel(vx_kernel kernel, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (kernel && Reference::isValidReference(reinterpret_cast<vx_reference>(kernel), VX_TYPE_KERNEL) == vx_true_e)
    {
        switch (attribute)
        {
            case VX_KERNEL_PARAMETERS:
                if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
                {
                    *(vx_uint32 *)ptr = kernel->signature.num_parameters;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_KERNEL_NAME:
                if (ptr != nullptr && size <= VX_MAX_KERNEL_NAME)
                {
                    vx_char kname[VX_MAX_KERNEL_NAME];
                    vx_char *k, *v;
                    strncpy(kname, kernel->name, VX_MAX_KERNEL_NAME);
                    k = strtok(kname, ":");
                    v = strtok(nullptr, ":");
                    (void)v; // need this variable in the future for variant searches
                    strncpy(reinterpret_cast<char*>(ptr), k, VX_MAX_KERNEL_NAME);
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_KERNEL_ENUM:
                if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
                {
                    *(vx_enum *)ptr = kernel->enumeration;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
            case VX_KERNEL_LOCAL_DATA_SIZE:
                if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
                {
                    *(vx_size *)ptr = kernel->attributes.localDataSize;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
#ifdef OPENVX_USE_OPENCL_INTEROP
            case VX_KERNEL_USE_OPENCL:
                if (VX_CHECK_PARAM(ptr, size, vx_bool, 0x3))
                {
                    *(vx_bool *)ptr = kernel->attributes.opencl_access;
                }
                else
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                break;
#endif
            default:
                status = VX_ERROR_NOT_SUPPORTED;
                break;
        }
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxAddParameterToKernel(vx_kernel kernel,
                                        vx_uint32 index,
                                        vx_enum dir,
                                        vx_enum data_type,
                                        vx_enum state)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    VX_PRINT(VX_ZONE_KERNEL,"INFO: Adding index %u, data_type 0x%x, dir:%d state:%d\n", index, data_type, dir, state);

    if (kernel && Reference::isValidReference(reinterpret_cast<vx_reference>(kernel), VX_TYPE_KERNEL) == vx_true_e)
    {
        if (index < kernel->signature.num_parameters)
        {
            {
                if (((Context::isValidType(data_type) == vx_false_e) ||
                     (Parameter::isValidDirection(dir) == vx_false_e) ||
                     (Parameter::isValidState(state) == vx_false_e)) ||
                     (data_type == VX_TYPE_DELAY && dir != VX_INPUT))
                {
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
                else
                {
                    kernel->signature.directions[index] = dir;
                    kernel->signature.types[index] = data_type;
                    kernel->signature.states[index] = state;
                    status = VX_SUCCESS;
                }
            }
        }
        else
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Not a valid reference!\n");
        status = VX_ERROR_INVALID_REFERENCE;
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxRemoveKernel(vx_kernel kernel)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (kernel &&
        Reference::isValidReference(reinterpret_cast<vx_reference>(kernel), VX_TYPE_KERNEL) == vx_true_e &&
        kernel->user_kernel)
    {
        vx_target target = nullptr;
        vx_char targetName[VX_MAX_TARGET_NAME];
        vx_uint32 kernelIdx = 0u;
        vx_context context = kernel->context;

        /* Remove the reference from the context */
        // ownRemoveReference(context, &kernel->base);

        /* find back references to kernel's target and kernel in target->kernels array */
        vx_uint32 index = (vx_uint32)(strnindex(kernel->name, ':', VX_MAX_TARGET_NAME));
        if (index == VX_MAX_TARGET_NAME)
        {
            strcpy(targetName, "khronos.any");
        }
        else
        {
            strncpy(targetName, kernel->name, index);
        }

        for (vx_uint32 t = 0u; t < context->num_targets; t++)
        {
            target = context->targets[t];
            if (strncmp(targetName,target->name, VX_MAX_TARGET_NAME) == 0)
            {
                break;
            }
            target = nullptr;
        }

        if (target)
        {
            for (vx_uint32 k = 0u; k < VX_INT_MAX_KERNELS; k++)
            {
                if (kernel == target->kernels[k])
                {
                    kernelIdx = k;
                    break;
                }
            }
        }

        if (target && kernelIdx < VX_INT_MAX_KERNELS)
        {
            if (kernel->enabled)
            {
                kernel->enabled = vx_false_e;
                context->num_kernels--;
                if (Kernel::isKernelUnique(kernel) == vx_true_e)
                {
                    context->num_unique_kernels--;
                }
            }
            target->num_kernels--;

            status = kernel->deinitializeKernel();

            if (status == VX_SUCCESS)
            {
                target->kernels[kernelIdx]->enumeration = VX_KERNEL_INVALID;
                target->kernels[kernelIdx]->user_kernel = vx_false_e;
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Can't deinitialize kernel properly\n");
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Can't locate kernel in its context\n");
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetKernelAttribute(vx_kernel kernel, vx_enum attribute, const void * ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;

    if (!kernel || Reference::isValidReference(reinterpret_cast<vx_reference>(kernel), VX_TYPE_KERNEL) == vx_false_e)
    {
        return VX_ERROR_INVALID_REFERENCE;
    }
    if (kernel->enabled == vx_true_e)
    {
        return VX_ERROR_NOT_SUPPORTED;
    }
    switch (attribute)
    {
        case VX_KERNEL_LOCAL_DATA_SIZE:
            if (VX_CHECK_PARAM(ptr, size, vx_size, 0x3))
            {
                kernel->attributes.localDataSize = *(vx_size *)ptr;
                VX_PRINT(VX_ZONE_KERNEL, "Set Local Data Size to " VX_FMT_SIZE " bytes\n", kernel->attributes.localDataSize);
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
#ifdef OPENVX_USE_OPENCL_INTEROP
        case VX_KERNEL_USE_OPENCL:
            if (VX_CHECK_PARAM(ptr, size, vx_bool, 0x3))
            {
                kernel->attributes.opencl_access = *(vx_bool *)ptr;
            }
            else
            {
                status = VX_ERROR_INVALID_VALUE;
            }
            break;
#endif
        default:
            status = VX_ERROR_NOT_SUPPORTED;
            break;
    }
    return status;
}
