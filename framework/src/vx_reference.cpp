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

vx_enum static_objects[] = {
        VX_TYPE_TARGET,
        VX_TYPE_KERNEL,
};

/*****************************************************************************/
/* INTERNAL REFERENCE APIS                                                   */
/*****************************************************************************/

Reference::Reference(vx_context context, vx_enum type, vx_reference scope)
{
#if !DISABLE_ICD_COMPATIBILITY
    if(context)
        platform = context->platform;
    else
        platform = nullptr;
#endif
    this->context = context;
    this->scope = scope;
    this->type = type;
    magic = VX_MAGIC;
    internal_count = 0;
    external_count = 0;
    write_count = 0;
    read_count = 0;
    extracted = vx_false_e;
    delay = nullptr;
    delay_slot_index = 0;
    is_virtual = vx_false_e;
    is_accessible = vx_false_e;
    name[0] = 0;
    ownCreateSem(&lock, 1);
}

Reference::~Reference()
{
    ownDestroySem(&lock);
    /* make sure no existing copies of refs can use ref again */
    magic = VX_BAD_MAGIC;
    // VX_PRINT(VX_ZONE_REFERENCE, ">>>> Reference count was zero, destructed object " VX_FMT_REF "\n", this);
}

vx_bool Reference::isValidReference(vx_reference ref)
{
    vx_bool ret = vx_false_e;

    if (nullptr != ref)
    {

        if ( (ref->magic == VX_MAGIC) &&
             (Context::isValidType(ref->type) == vx_true_e) &&
             (( (ref->type != VX_TYPE_CONTEXT) && (Context::isValidContext(ref->context) == vx_true_e) ) ||
              ( (ref->type == VX_TYPE_CONTEXT) && (ref->context == nullptr) )) )
        {
            ret = vx_true_e;
        }
        else if (ref->magic == VX_BAD_MAGIC)
        {
            VX_PRINT(VX_ZONE_ERROR, "%p has already been released and garbage collected!\n", ref);
        }
        else if (ref->type != VX_TYPE_CONTEXT)
        {
            printReference(ref);
            VX_PRINT(VX_ZONE_ERROR, "%p is not a valid reference!\n", ref);
            DEBUG_BREAK();
            VX_BACKTRACE(VX_ZONE_ERROR);
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Reference was NULL\n");
        VX_BACKTRACE(VX_ZONE_WARNING);
    }

    return ret;
}

vx_bool Reference::isValidReference(vx_reference ref, vx_enum type)
{
    vx_bool ret = vx_false_e;

    if (nullptr != ref)
    {

        if ((ref->magic == VX_MAGIC) &&
            (ref->type == type) &&
            (Context::isValidContext(ref->context) == vx_true_e))
        {
            ret = vx_true_e;
        }
        else if (ref->type != VX_TYPE_CONTEXT)
        {
            Reference::printReference(ref); // For debugging
            VX_PRINT(VX_ZONE_ERROR, "%p is not a valid reference!\n", ref);
            DEBUG_BREAK(); // this will catch any "invalid" objects, but is not useful otherwise.
            VX_BACKTRACE(VX_ZONE_WARNING);
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_WARNING, "Reference was NULL\n");
        VX_BACKTRACE(VX_ZONE_WARNING);
    }

    return ret;
}

vx_uint32 Reference::incrementReference(vx_reftype_e refType)
{
    vx_uint32 count = 0u;

    ownSemWait(&lock);
    if (refType == VX_EXTERNAL || refType == VX_BOTH)
        external_count++;
    if (refType == VX_INTERNAL || refType == VX_BOTH)
        internal_count++;
    count = internal_count + external_count;
    VX_PRINT(VX_ZONE_REFERENCE, "Incremented Total Reference Count to %u on " VX_FMT_REF
        " [ext:%d, int:%d]\n", count, this, external_count, internal_count);
    ownSemPost(&lock);

    return count;
}

vx_uint32 Reference::decrementReference(vx_reftype_e refType)
{
    vx_uint32 result = UINT32_MAX;

    ownSemWait(&lock);
    if (refType == VX_INTERNAL || refType == VX_BOTH) {
        if (internal_count == 0) {
            VX_PRINT(VX_ZONE_WARNING, "#### INTERNAL REF COUNT IS ALREADY ZERO!!! " VX_FMT_REF " type:%08x #####\n", this, type);
            DEBUG_BREAK();
        } else {
            internal_count--;
        }
    }
    if (refType == VX_EXTERNAL || refType == VX_BOTH) {
        if (external_count == 0)
        {
            VX_PRINT(VX_ZONE_WARNING, "#### EXTERNAL REF COUNT IS ALREADY ZERO!!! " VX_FMT_REF " type:%08x #####\n", this, type);
            DEBUG_BREAK();
        }
        else
        {
            external_count--;
            if ((external_count == 0) && (extracted == vx_true_e))
            {
                extracted = vx_false_e;
            }
        }
    }

    result = internal_count + external_count;
    VX_PRINT(VX_ZONE_REFERENCE, "Decremented Total Reference Count to %u on " VX_FMT_REF
        " type:%08x [ext:%d, int:%d]\n", result, this, type, external_count, internal_count);
    ownSemPost(&lock);

    return result;
}

vx_size Reference::sizeOfType(vx_enum type)
{
    vx_uint32 i = 0;
    vx_size size = 0ul;

    for (i = 0; i < dimof(type_sizes); i++) {
        if (type == type_sizes[i].type) {
            size = type_sizes[i].size;
            break;
        }
    }

    return size;
}

vx_uint32 Reference::totalReferenceCount()
{
    vx_uint32 count = 0;
    {
        ownSemWait(&lock);
        count = external_count + internal_count;
        ownSemPost(&lock);
    }
    return count;
}

vx_reference Reference::createReference(vx_context context, vx_enum type, vx_reftype_e refType, vx_reference scope)
{
    vx_reference ref = nullptr;

    try
    {
        switch(type)
        {
            case VX_TYPE_GRAPH:
            {
                ref = new Graph(context, scope);
                break;
            }
            case VX_TYPE_NODE:
            {
                ref = new Node(context, scope);
                break;
            }
            case VX_TYPE_KERNEL:
            {
                ref = new Kernel(context, scope);
                break;
            }
            case VX_TYPE_PARAMETER:
            {
                ref = new Parameter(context, scope);
                break;
            }
            case VX_TYPE_IMAGE:
            {
                ref = new Image(context, scope);
                break;
            }
            case VX_TYPE_SCALAR:
            {
                ref = new Scalar(context, scope);
                break;
            }
            case VX_TYPE_TENSOR:
            {
                ref = new Tensor(context, scope);
                break;
            }
            case VX_TYPE_LUT:
            case VX_TYPE_ARRAY:
            {
                ref = new Array(context, type, scope);
                break;
            }
            case VX_TYPE_OBJECT_ARRAY:
            {
                ref = new ObjectArray(context, scope);
                break;
            }
            case VX_TYPE_MATRIX:
            {
                ref = new Matrix(context, scope);
                break;
            }
            case VX_TYPE_CONVOLUTION:
            {
                ref = new Convolution(context, scope);
                break;
            }
#if defined(OPENVX_USE_USER_DATA_OBJECT)
            case VX_TYPE_USER_DATA_OBJECT:
            {
                ref = new UserDataObject(context, scope);
                break;
            }
#endif /* defined(OPENVX_USE_USER_DATA_OBJECT) */
            case VX_TYPE_DELAY:
            {
                ref = new Delay(context, scope);
                break;
            }
            case VX_TYPE_DISTRIBUTION:
            {
                ref = new Distribution(context, scope);
                break;
            }
            case VX_TYPE_REMAP:
            {
                ref = new Remap(context, scope);
                break;
            }
            case VX_TYPE_PYRAMID:
            {
                ref = new Pyramid(context, scope);
                break;
            }
            case VX_TYPE_THRESHOLD:
            {
                ref = new Threshold(context, scope);
                break;
            }
            case VX_TYPE_META_FORMAT:
            {
                ref = new MetaFormat(context, scope);
                break;
            }
            case VX_TYPE_TARGET:
            {
                ref = new Target(context, scope);
                break;
            }
            case VX_TYPE_IMPORT:
            {
                ref = new Import(context, scope);
                break;
            }
            case VX_TYPE_ERROR:
            {
                ref = new Error(context, scope);
                break;
            }
            default:
            {
                VX_PRINT(VX_ZONE_ERROR, "Unsupported type passed 0x%x\n", type);
            }
        }

        if (ref)
        {
            (void)context->addReference(ref);
            ref->incrementReference(refType);
        }
    }
    catch (const std::exception& e)
    {
        // Handle error (e.g., log it) and return a null or invalid reference
        ref = nullptr;
        VX_PRINT(VX_ZONE_ERROR, "Error creating reference: %s\n", e.what());
        DEBUG_BREAK();
    }

    return ref;
}

void Reference::printReference(vx_reference ref)
{
    if (ref)
    {
        VX_PRINT(VX_ZONE_REFERENCE, "vx_reference:%p magic:%08x type:%08x count:[%u,%u] context:%p\n", ref, ref->magic, ref->type, ref->external_count, ref->internal_count, ref->context);
    }
}

vx_status Reference::releaseReference(vx_reference* r,
                                      vx_enum type,
                                      vx_reftype_e reftype,
                                      vx_destructor_f special_destructor)
{
    vx_status status = VX_SUCCESS;
    vx_reference ref = *r;
    if (Reference::isValidReference(ref, type) == vx_true_e &&
        type == ref->type)
    {
        if (ref->decrementReference(reftype) == 0)
        {
            vx_destructor_f destructor = special_destructor;

            /* if there is a destructor, call it. */
            if (destructor)
            {
                destructor(ref);
            }

            ref->destruct();

            if (ref->context->removeReference(ref) == vx_false_e)
            {
                status = VX_FAILURE;
                return status;
            }
        }
        *r = nullptr;
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    return status;
}

void Reference::initReferenceForDelay(vx_delay d, vx_int32 index)
{
    delay = d;
    delay_slot_index = index;
}

void Reference::destruct()
{
}

/*****************************************************************************/
/* PUBLIC APIS                                                               */
/*****************************************************************************/

VX_API_ENTRY vx_status VX_API_CALL vxQueryReference(vx_reference ref, vx_enum attribute, void *ptr, vx_size size)
{
    vx_status status = VX_SUCCESS;

    /* if it is not a reference and not a context */
    if ((Reference::isValidReference(ref) == vx_false_e) &&
        (Context::isValidContext((vx_context)ref) == vx_false_e))
    {
        return VX_ERROR_INVALID_REFERENCE;
    }

    switch (attribute)
    {
        case VX_REFERENCE_COUNT:
            if (VX_CHECK_PARAM(ptr, size, vx_uint32, 0x3))
            {
                *(vx_uint32 *)ptr = ref->external_count;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REFERENCE_TYPE:
            if (VX_CHECK_PARAM(ptr, size, vx_enum, 0x3))
            {
                *(vx_enum *)ptr = ref->type;
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        case VX_REFERENCE_NAME:
            if (VX_CHECK_PARAM(ptr, size, vx_char*, 0x3))
            {
                *(vx_char**)ptr = &ref->name[0];
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            break;
        default:
            status = VX_ERROR_NOT_SUPPORTED;
            break;
    }
    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetReferenceName(vx_reference ref, const vx_char* name)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    if (Reference::isValidReference(ref))
    {
        strncpy(ref->name, name, strnlen(name, VX_MAX_REFERENCE_NAME));
        status = VX_SUCCESS;
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseReference(vx_reference* ref_ptr)
{
    vx_status status = VX_ERROR_INVALID_REFERENCE;

    vx_reference ref = (ref_ptr ? *ref_ptr : nullptr);
    if (Reference::isValidReference(ref) == vx_true_e)
    {
        switch (ref->type)
        {
        case VX_TYPE_CONTEXT:      status = vxReleaseContext((vx_context*)ref_ptr); break;
        case VX_TYPE_GRAPH:        status = vxReleaseGraph((vx_graph*)ref_ptr); break;
        case VX_TYPE_NODE:         status = vxReleaseNode((vx_node*)ref_ptr); break;
        case VX_TYPE_ARRAY:        status = vxReleaseArray((vx_array*)ref_ptr); break;
        case VX_TYPE_OBJECT_ARRAY: status = vxReleaseObjectArray((vx_object_array*)ref_ptr); break;
        case VX_TYPE_CONVOLUTION:  status = vxReleaseConvolution((vx_convolution*)ref_ptr); break;
        case VX_TYPE_DISTRIBUTION: status = vxReleaseDistribution((vx_distribution*)ref_ptr); break;
        case VX_TYPE_IMAGE:        status = vxReleaseImage((vx_image*)ref_ptr); break;
        case VX_TYPE_LUT:          status = vxReleaseLUT((vx_lut*)ref_ptr); break;
        case VX_TYPE_MATRIX:       status = vxReleaseMatrix((vx_matrix*)ref_ptr); break;
        case VX_TYPE_PYRAMID:      status = vxReleasePyramid((vx_pyramid*)ref_ptr); break;
        case VX_TYPE_REMAP:        status = vxReleaseRemap((vx_remap*)ref_ptr); break;
        case VX_TYPE_SCALAR:       status = vxReleaseScalar((vx_scalar*)ref_ptr); break;
        case VX_TYPE_THRESHOLD:    status = vxReleaseThreshold((vx_threshold*)ref_ptr); break;
        case VX_TYPE_DELAY:        status = vxReleaseDelay((vx_delay*)ref_ptr); break;
        case VX_TYPE_KERNEL:       status = vxReleaseKernel((vx_kernel*)ref_ptr); break;
        case VX_TYPE_PARAMETER:    status = vxReleaseParameter((vx_parameter*)ref_ptr); break;
        case VX_TYPE_TENSOR:       status = vxReleaseTensor((vx_tensor*)ref_ptr); break;
#if defined(OPENVX_USE_USER_DATA_OBJECT)
        case VX_TYPE_USER_DATA_OBJECT: status = vxReleaseUserDataObject((vx_user_data_object*)ref_ptr); break;
#endif /* defined(OPENVX_USE_USER_DATA_OBJECT) */
#if defined(OPENVX_USE_IX) || defined(OPENVX_USE_XML)
        case VX_TYPE_IMPORT:       status = vxReleaseImport((vx_import*)ref_ptr); break;
#endif
        default:
            break;
        }
    }

    return status;
}

VX_API_ENTRY vx_status VX_API_CALL vxRetainReference(vx_reference ref)
{
    vx_status status = VX_SUCCESS;

    if (Reference::isValidReference(ref) == vx_true_e)
    {
        ref->incrementReference(VX_EXTERNAL);
    }
    else
    {
        status = VX_ERROR_INVALID_REFERENCE;
    }

    return status;
}
