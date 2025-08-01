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
#include "vx_parameter.h"

/******************************************************************************/
/* INTERNAL INTERFACE                                                         */
/******************************************************************************/
Parameter::Parameter(vx_context context, vx_reference ref) : Reference(context, VX_TYPE_PARAMETER, ref),
index(0),
node(nullptr),
kernel(nullptr)
{

}

Parameter::~Parameter()
{
}

vx_bool Parameter::isValidDirection(vx_enum dir)
{
    if ((dir == VX_INPUT) || (dir == VX_OUTPUT) || (dir == VX_BIDIRECTIONAL))
    {
        return vx_true_e;
    }
    else
    {
        return vx_false_e;
    }
}

vx_bool Parameter::isValidTypeMatch(vx_enum expected, vx_enum supplied)
{
    vx_bool match = vx_false_e;
    if (expected == supplied)
    {
        match = vx_true_e;
    }
    if (match == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Expected %08x and got %08x!\n", expected, supplied);
    }
    return match;
}

vx_bool Parameter::isValidState(vx_enum state)
{
    if ((state == VX_PARAMETER_STATE_REQUIRED) ||
        (state == VX_PARAMETER_STATE_OPTIONAL))
    {
        return vx_true_e;
    }
    else
    {
        return vx_false_e;
    }
}

vx_enum Parameter::direction() const
{
    if (kernel)
    {
        return kernel->signature.directions[index];
    }
    return VX_FAILURE;
}

vx_uint32 Parameter::idx() const
{
    return index;
}

vx_enum Parameter::dataType() const
{
    if (kernel && index < VX_INT_MAX_PARAMS)
    {
        return kernel->signature.types[index];
    }
    return VX_FAILURE;
}

vx_enum Parameter::state() const
{
    if (kernel && index < VX_INT_MAX_PARAMS)
    {
        return kernel->signature.states[index];
    }
    return VX_FAILURE;
}

vx_reference Parameter::ref() const
{
    if (node && index < VX_INT_MAX_PARAMS)
    {
        vx_reference ref = node->parameters[index];
        /* does this object have USER access? */
        if (ref)
        {
            /*! \internal this could potentially allow the user to break
             * a currently chosen optimization! We need to alert the
             * system that if a write occurs to this data, put the graph
             * into an unverified state.
             */
            if (ref->external_count == 0) ref->extracted = vx_true_e;
            ref->incrementReference(VX_EXTERNAL);
        }
        return ref;
    }

    return nullptr;
}

vx_meta_format Parameter::metaFormat() const
{
    if (kernel && index < VX_INT_MAX_PARAMS && kernel->signature.meta_formats[index])
    {
        return kernel->signature.meta_formats[index];
    }

    return nullptr;
}

vx_parameter Parameter::getKernelParameterByIndex(vx_kernel kernel, vx_uint32 index)
{
    vx_parameter parameter = nullptr;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(kernel), VX_TYPE_KERNEL) ==
        vx_true_e)
    {
        if (index < VX_INT_MAX_PARAMS && index < kernel->signature.num_parameters)
        {
            parameter = (vx_parameter)Reference::createReference(kernel->context, VX_TYPE_PARAMETER,
                                                                 VX_EXTERNAL, kernel->context);
            if (vxGetStatus((vx_reference)parameter) == VX_SUCCESS &&
                parameter->type == VX_TYPE_PARAMETER)
            {
                parameter->index = index;
                parameter->node = nullptr;
                parameter->kernel = kernel;
                parameter->kernel->incrementReference(VX_INTERNAL);
            }
            else
            {
                VX_PRINT(VX_ZONE_ERROR, "Failed to create valid parameter object!\n");
                delete parameter;
                parameter = nullptr;
            }
        }
        else
        {
            vxAddLogEntry(reinterpret_cast<vx_reference>(kernel), VX_ERROR_INVALID_PARAMETERS,
                          "Index %u out of range for node %s (numparams = %u)!\n", index,
                          kernel->name, kernel->signature.num_parameters);
            parameter =
                (vx_parameter)vxGetErrorObject(kernel->context, VX_ERROR_INVALID_PARAMETERS);
        }
    }

    return parameter;
}

vx_parameter Parameter::getParameterByIndex(vx_node node, vx_uint32 index)
{
    vx_parameter param = nullptr;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(node), VX_TYPE_NODE) ==
        vx_false_e)
    {
        return param;
    }

    if (node->kernel == nullptr)
    {
        /* this can probably never happen */
        vxAddLogEntry(reinterpret_cast<vx_reference>(node), VX_ERROR_INVALID_NODE,
                      "Node was created without a kernel! Fatal Error!\n");
        param = (vx_parameter)vxGetErrorObject(node->context, VX_ERROR_INVALID_NODE);
    }
    else
    {
        if (/*0 <= index &&*/ index < VX_INT_MAX_PARAMS &&
            index < node->kernel->signature.num_parameters)
        {
            param = (vx_parameter)Reference::createReference(node->context, VX_TYPE_PARAMETER,
                                                             VX_EXTERNAL, node);
            if (vxGetStatus((vx_reference)param) == VX_SUCCESS && param->type == VX_TYPE_PARAMETER)
            {
                param->index = index;
                param->node = node;
                node->incrementReference(VX_INTERNAL);
                param->kernel = node->kernel;
                param->kernel->incrementReference(VX_INTERNAL);
                // if (node->parameters[index])
                // node->parameters[index]->incrementReference(VX_INTERNAL);
            }
        }
        else
        {
            vxAddLogEntry(reinterpret_cast<vx_reference>(node), VX_ERROR_INVALID_PARAMETERS,
                          "Index %u out of range for node %s (numparams = %u)!\n", index,
                          node->kernel->name, node->kernel->signature.num_parameters);
            param = (vx_parameter)vxGetErrorObject(node->context, VX_ERROR_INVALID_PARAMETERS);
        }
    }
    VX_PRINT(VX_ZONE_API, "%s: returning %p\n", __FUNCTION__, param);
    return param;
}

vx_status Parameter::setParameterByIndex(vx_node node, vx_uint32 index, vx_reference value)
{
    vx_status status = VX_SUCCESS;
    vx_enum type = 0;
    vx_enum data_type = 0;

    if (Reference::isValidReference(reinterpret_cast<vx_reference>(node), VX_TYPE_NODE) ==
        vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Supplied node was not actually a node\n");
        status = VX_ERROR_INVALID_REFERENCE;
        goto exit;
    }

    VX_PRINT(VX_ZONE_PARAMETER,
             "Attempting to set parameter[%u] on %s (enum:%d) to " VX_FMT_REF "\n", index,
             node->kernel->name, node->kernel->enumeration, value);

    /* is the index out of bounds? */
    if ((index >= node->kernel->signature.num_parameters) || (index >= VX_INT_MAX_PARAMS))
    {
        VX_PRINT(VX_ZONE_ERROR, "Invalid index %u\n", index);
        status = VX_ERROR_INVALID_VALUE;
        goto exit;
    }

    /* if it's an optional parameter, it's ok to be nullptr */
    if ((value == 0) && (node->kernel->signature.states[index] == VX_PARAMETER_STATE_OPTIONAL))
    {
        status = VX_SUCCESS;
        goto exit;
    }

    /* if it's required, it's got to exist */
    if (value == nullptr)
    {
        value = Reference::createReference(node->context, node->kernel->signature.types[index],
                                           VX_INTERNAL, node->context);
    }

    /* validate reference */
    if (Reference::isValidReference((vx_reference)value) == vx_false_e)
    {
        VX_PRINT(VX_ZONE_ERROR, "Supplied value was not actually a reference\n");
        status = VX_ERROR_INVALID_REFERENCE;
        goto exit;
    }

    /* if it was a valid reference then get the type from it */
    vxQueryReference(value, VX_REFERENCE_TYPE, &type, sizeof(type));
    VX_PRINT(VX_ZONE_PARAMETER, "Query returned type %08x for ref " VX_FMT_REF "\n", type, value);

    /* Check that signature type matches reference type */
    if (node->kernel->signature.types[index] != type)
    {
        /* Check special case where signature is a specific scalar type.
           This can happen if the vxAddParameterToKernel() passes one of the scalar
           vx_type_e types instead of the more generic VX_TYPE_SCALAR since the spec
           doesn't specify that only VX_TYPE_SCALAR should be used for scalar types in
           this function. */
        if (node->kernel->signature.types[index] == VX_TYPE_REFERENCE)
        {
            if (!VX_TYPE_IS_DATA_OBJECT(type))
            {
                VX_PRINT(VX_ZONE_ERROR, "Invalid data object type 0x%08x!\n", type);
                status = VX_ERROR_INVALID_TYPE;
                goto exit;
            }
        }
        else if ((type == VX_TYPE_SCALAR) &&
                 (vxQueryScalar((vx_scalar)value, VX_SCALAR_TYPE, &data_type, sizeof(data_type)) ==
                  VX_SUCCESS))
        {
            if (data_type != node->kernel->signature.types[index])
            {
                VX_PRINT(VX_ZONE_ERROR, "Invalid scalar type 0x%08x!\n", data_type);
                status = VX_ERROR_INVALID_TYPE;
                goto exit;
            }
        }
        else
        {
            VX_PRINT(VX_ZONE_ERROR, "Invalid type %s (0x%08x), expected %s (0x%08x)\n",
                     vxGetObjectTypeName(type), type,
                     vxGetObjectTypeName(node->kernel->signature.types[index]),
                     node->kernel->signature.types[index]);
            status = VX_ERROR_INVALID_TYPE;
            *((volatile vx_uint8 *)nullptr) = 0;
            goto exit;
        }
    }

    if (node->parameters[index] && node->parameters[index]->delay != nullptr)
    {
        // we already have a delay element here */
        vx_bool res = Delay::removeAssociationToDelay(node->parameters[index], node, index);
        if (res == vx_false_e)
        {
            VX_PRINT(VX_ZONE_ERROR, "Internal error removing delay association\n");
            status = VX_ERROR_INVALID_REFERENCE;
            goto exit;
        }
    }

    if (value->delay != nullptr)
    {
        /* the new parameter is a delay element */
        vx_bool res = Delay::addAssociationToDelay(value, node, index);
        if (res == vx_false_e)
        {
            VX_PRINT(VX_ZONE_ERROR, "Internal error adding delay association\n");
            status = VX_ERROR_INVALID_REFERENCE;
            goto exit;
        }
    }

    /* actual change of the node parameter */
    node->setParameter(index, value);

    /* Note that we don't need to do anything special for parameters to child graphs. */

exit:
    if (status == VX_SUCCESS)
    {
        VX_PRINT(VX_ZONE_PARAMETER, "Assigned Node[%u] %p type:%08x ref=" VX_FMT_REF "\n", index,
                 node, type, value);
    }
    else if (vx_true_e ==
             Reference::isValidReference(reinterpret_cast<vx_reference>(node), VX_TYPE_NODE))
    {
        VX_PRINT(VX_ZONE_ERROR, "Specified: parameter[%u] type:%08x => " VX_FMT_REF "\n", index,
                 type, value);
        VX_PRINT(VX_ZONE_ERROR, "Required: parameter[%u] dir:%d type:%08x\n", index,
                 node->kernel->signature.directions[index], node->kernel->signature.types[index]);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "%s: Invalid node reference provided\n", __func__);
    }

    return status;
}

vx_status Parameter::setParameterByReference(vx_reference value)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (node)
    {
        status = Parameter::setParameterByIndex(node, index, value);
    }

    return status;
}

void Parameter::destruct()
{
    if (node)
    {
        if (Reference::isValidReference(reinterpret_cast<vx_reference>(node), VX_TYPE_NODE) ==
            vx_true_e)
        {
            Reference::releaseReference((vx_reference *)&node, VX_TYPE_NODE, VX_INTERNAL, nullptr);
        }
    }
    if (kernel)
    {
        if (Reference::isValidReference(reinterpret_cast<vx_reference>(kernel), VX_TYPE_KERNEL) ==
            vx_true_e)
        {
            Reference::releaseReference((vx_reference *)&kernel, VX_TYPE_KERNEL, VX_INTERNAL,
                                        nullptr);
        }
    }
}

/******************************************************************************/
/* PUBLIC API                                                                 */
/******************************************************************************/

VX_API_ENTRY vx_parameter VX_API_CALL vxGetKernelParameterByIndex(vx_kernel kernel, vx_uint32 index)
{
    return Parameter::getKernelParameterByIndex(kernel, index);
}

VX_API_ENTRY vx_parameter VX_API_CALL vxGetParameterByIndex(vx_node node, vx_uint32 index)
{
    return Parameter::getParameterByIndex(node, index);
}

VX_API_ENTRY vx_status VX_API_CALL vxSetParameterByIndex(vx_node node, vx_uint32 index,
                                                         vx_reference value)
{
    return Parameter::setParameterByIndex(node, index, value);
}

VX_API_ENTRY vx_status VX_API_CALL vxSetParameterByReference(vx_parameter parameter, vx_reference value)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(parameter), VX_TYPE_PARAMETER) == vx_true_e)
    {
        status = parameter->setParameterByReference(value);
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxQueryParameter(vx_parameter parameter, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;
    if (Reference::isValidReference(reinterpret_cast<vx_reference>(parameter), VX_TYPE_PARAMETER) == vx_true_e)
    {
        switch (attribute)
        {
            case VX_PARAMETER_DIRECTION:
                if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
                    *(vx_enum *)ptr = parameter->direction();
                else
                    status = VX_ERROR_INVALID_PARAMETERS;
                break;
            case VX_PARAMETER_INDEX:
                if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
                    *(vx_uint32 *)ptr = parameter->idx();
                else
                    status = VX_ERROR_INVALID_PARAMETERS;
                break;
            case VX_PARAMETER_TYPE:
                if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
                    *(vx_enum *)ptr = parameter->dataType();
                else
                    status = VX_ERROR_INVALID_PARAMETERS;
                break;
            case VX_PARAMETER_STATE:
                if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
                    *(vx_enum *)ptr = parameter->state();
                else
                    status = VX_ERROR_INVALID_PARAMETERS;
                break;
            case VX_PARAMETER_REF:
                if (VX_CHECK_PARAM(ptr, size, vx_reference, 0x3))
                {
                    if (parameter->node)
                    {
                        *(vx_reference *)ptr = parameter->ref();
                    }
                    else
                        status = VX_ERROR_NOT_SUPPORTED;
                }
                else
                    status = VX_ERROR_INVALID_PARAMETERS;
                break;
            case VX_PARAMETER_META_FORMAT:
                if (VX_CHECK_PARAM(ptr, size, vx_meta_format, 0x3))
                {
                    if (nullptr != parameter->kernel->signature.meta_formats[parameter->index])
                    {
                        *(vx_meta_format *)ptr = parameter->metaFormat();
                    }
                    else
                    {
                        status = VX_ERROR_NOT_SUPPORTED;
                    }
                }
                else
                    status = VX_ERROR_INVALID_PARAMETERS;
                break;
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

VX_API_ENTRY vx_status VX_API_CALL vxReleaseParameter(vx_parameter *param)
{
    vx_status status = VX_FAILURE;

    if (param != nullptr)
    {
        vx_parameter this_param = *param;
        if (Reference::isValidReference((vx_reference)this_param, VX_TYPE_PARAMETER) == vx_true_e)
        {
            status = Reference::releaseReference((vx_reference *)param, VX_TYPE_PARAMETER,
                                                 VX_EXTERNAL, nullptr);
        }
    }

    return status;
}