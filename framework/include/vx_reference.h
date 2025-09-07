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

#ifndef VX_REFERENCE_H
#define VX_REFERENCE_H

#include "vx_internal.h"

#define VX_BAD_MAGIC (42)

/*!
 * \file
 * \brief The internal Reference implementation
 *
 * \defgroup group_int_reference Internal Reference API
 * \ingroup group_internal
 * \brief The Internal Reference API.
 */

/*! \brief The most basic type in the OpenVX system. Any type that inherits
 *  from vx_reference must have a vx_reference as its first memeber
 *  to allow casting to this type.
 * \ingroup group_int_reference
 */
namespace coreflow {

class Reference {
protected:
    /**
     * @brief Construct a new Reference object
     *
     * @param context   The context associated with this obj
     * @param type      The type of this ref
     * @param scope     The parent ref of this obj
     * @ingroup group_int_reference
     */
    Reference(vx_context context, vx_enum type, vx_reference scope);

    /**
     * @brief Destroy the vx reference object
     * @ingroup group_int_reference
     */
    virtual ~Reference();

public:
    /**
     * @brief Returns the reference count of the object
     * @return vx_uint32 The reference count
     * @ingroup group_int_reference
     */
    vx_uint32 refCount() const;

    /**
     * @brief Returns the type of the reference
     *
     * @return vx_enum The type of the reference
     * @ingroup group_int_reference
     */
    vx_enum dataType() const;

    /**
     * @brief Returns the name of the reference
     *
     * @return const vx_char* The name of the reference
     * @ingroup group_int_reference
     */
    const vx_char* refName() const;

    /**
     * @brief Sets the name of the reference
     *
     * @param name The name to set
     * @ingroup group_int_reference
     */
    void setName(const vx_char* name);

    /*! \brief Used to create a reference.
     * \note This does not add the reference to the system context yet.
     * \param [in] context The system context.
     * \param [in] type The \ref vx_type_e type desired.
     * \param [in] refType The reference type.
     * \param [in] scope The parent reference.
     * \ingroup group_int_reference
     */
    static vx_reference createReference(vx_context context, vx_enum type, vx_reftype_e refType, vx_reference scope);

    /*! \brief Prints the values of a reference.
     * \param [in] ref The reference to print.
     * \ingroup group_int_reference
     */
    static void printReference(vx_reference ref);

    /*! \brief Used to validate everything but vx_context, vx_image and vx_buffer.
     * \param [in] ref The reference to validate.
     * \ingroup group_implementation
     */
    static vx_bool isValidReference(vx_reference ref);

    /*! \brief Used to validate everything but vx_context, vx_image and vx_buffer.
     * \param [in] ref The reference to validate.
     * \param [in] type The \ref vx_type_e to check for.
     * \ingroup group_implementation
     */
    static vx_bool isValidReference(vx_reference ref, vx_enum type);

    /*! \brief Returns the number of bytes in the internal structure for a given type.
     * \ingroup group_int_reference
     */
    static vx_size sizeOfType(vx_enum type);

    /*! \brief Increments the ref count.
     * \param [in] refType The reference type to increment.
     * \ingroup group_int_reference
     */
    vx_uint32 incrementReference(vx_reftype_e refType);

    /*! \brief Decrements the ref count.
     * \param [in] refType The reference type to decrement.
     * \ingroup group_int_reference
     */
    vx_uint32 decrementReference(vx_reftype_e refType);

    /*! \brief Returns the total reference count of the object.
     * \ingroup group_int_reference
     */
    vx_uint32 totalReferenceCount();

    /*! \brief Used to destroy a reference.
     * \param [in] ref The reference to release.
     * \param [in] type The \ref vx_type_e to check against.
     * \param [in] reftype The reference type.
     * \param [in] special_destructor The a special function to call after the total count has reached zero, if NULL, a default destructor is used.
     * \ingroup group_int_reference
     */
    static vx_status releaseReference(vx_reference* ref,
                                      vx_enum type,
                                      vx_reftype_e reftype,
                                      vx_destructor_f special_destructor);

    /*! \brief Used to initialize any vx_reference as a delay element
    * \param [in] delay The delay to which the object belongs
    * \param [in] index The index in the delay
    * \ingroup group_int_reference
    */
    void initReferenceForDelay(vx_delay delay, vx_int32 index);

    /**
     * @brief Destruct reference
     * @ingroup group_int_reference
     */
    virtual void destruct();

#if !DISABLE_ICD_COMPATIBILITY
    /*! \brief Platform for ICD compatibility. */
    struct _vx_platform * platform;
#endif
    /*! \brief Used to validate references, must be set to VX_MAGIC. */
    vx_uint32 magic;
    /*! \brief Set to an enum value in \ref vx_type_e. */
    vx_enum type;
    /*! \brief Pointer to the top level context.
     * If this reference is the context, this will be NULL.
     */
    vx_context context;
    /*! \brief The pointer to the object's scope parent. When virtual objects
     * are scoped within a graph, this will point to that parent graph. This is
     * left generic to allow future scoping variations. By default scope should
     * be the same as context.
     */
    vx_reference scope;
    /*! \brief The count of the number of users with this reference. When
     * greater than 0, this can not be freed. When zero, the value can be
     * considered inaccessible.
     */
    vx_uint32 external_count;
    /*! \brief The count of the number of framework references. When
     * greater than 0, this can not be freed.
     */
    vx_uint32 internal_count;
    /*! \brief The number of times the object has been read (in some portion) */
    vx_uint32 read_count;
    /*! \brief The number of times the object has been written to (in some portion) */
    vx_uint32 write_count;
    /*! \brief The reference lock which is used to protect access to "in-fly" data. */
    vx_sem_t lock;
    /*! \brief A reserved field which can be used to store anonymous data */
    void *reserved;
    /*! \brief A field which can be used to store a temporary, per-graph index. */
    vx_uint32 index;
    /*! \brief This indicates if the object was extracted from another object */
    vx_bool extracted;
    /*! \brief This indicates if the object is virtual or not */
    vx_bool is_virtual;
    /* \brief This indicates if the object belongs to a delay */
    vx_delay delay;
    /* \brief This indicates the original delay slot index when the object belongs to a delay */
    vx_int32 delay_slot_index;
    /*! \brief This indicates that if the object is virtual whether it is accessible at the moment or not */
    vx_bool is_accessible;
#if defined(EXPERIMENTAL_USE_OPENCL)
    /*! \brief An OpenCL event that the framework can block upon for this object */
    cl_event event;
#endif
    /*! \brief The reference name */
    vx_char name[VX_MAX_REFERENCE_NAME];
};

} // namespace coreflow

#endif /* VX_REFERENCE_H */