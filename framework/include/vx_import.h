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

#ifndef VX_IMPORT_H
#define VX_IMPORT_H

#include <VX/vx.h>
#include <VX/vx_import.h>
#include <VX/vx_khr_xml.h>

/*!
 * \file
 * \brief The Import Object Internal API.
 *
 * \defgroup group_int_import Internal Import Object API
 * \ingroup group_internal
 * \brief The Internal Import Object API
 */

/*! \brief The internal representation of a \ref vx_import
 * \ingroup group_int_import
 */
namespace corevx {

class Import : public Reference
{
public:
    /**
     * @brief Construct a new Import object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_import
     */
    Import(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Import object
     * @ingroup group_int_import
     */
    ~Import();

#if defined(OPENVX_USE_XML)
    /**
     * @brief Get the number of references in the import
     *
     * @return vx_uint32 The number of references in the import.
     * @ingroup group_int_import
     */
    vx_uint32 numRefs() const;

    /**
     * @brief Get the type of import
     *
     * @return vx_enum The type of import.
     * @ingroup group_int_import
     */
    vx_enum importType() const;

    /**
     * @brief Get the Reference By Index object
     *
     * @param index
     * @return vx_reference
     * @ingroup group_int_import
     */
    vx_reference getReferenceByIndex(vx_uint32 index);
#endif /* defined(OPENVX_USE_XML) */

#if defined(OPENVX_USE_IX) || defined(OPENVX_USE_XML)
    /*! \brief Create an import object.
     * \param [in] context The context.
     * \param [in] type The type of import.
     * \param [in] count The number of references to import.
     * \ingroup group_int_import
     */
    static vx_import createImportInt(vx_context context,
                                             vx_enum type,
                                             vx_uint32 count);

    /**
     * @brief Get a reference from the import object by name.\n
     *
     * @details All accessible references of the import object created using <tt>\ref
     * vxImportObjectsFromMemory</tt> are in the array *refs*, which is populated partly by the
     * application before import, and partly by the framework. However, it may be more convenient to
     * access the references in the import object without referring to this array, for example if
     * the import object is passed as a parameter to another function. In this case, references may
     * be retreived by name, assuming that <tt>\ref vxSetReferenceName</tt> was called to assign a
     * name to the reference. This function searches the given import for the given name and returns
     * the associated reference.\n The reference may have been named either before export or
     * after import.\n If more than one reference exists in the import with the given name,
     * this is an error.\n Only references in the array *refs* after calling <tt>\ref
     * vxImportObjectsFromMemory</tt> may be retrieved using this function.\n A reference to
     * a named object may be obtained from a valid import object using this API even if all other
     * references to the object have been released.
     *
     * @param name The name to find, points to a string of at least one and less than
     * VX_MAX_REFERENCE_NAME bytes followed by a zero byte; the function will fail if this is not
     * valid.
     * @return vx_reference A <tt>\ref vx_reference</tt>[*REQ*].\n
     * Calling <tt>\ref Error::getStatus</tt> with the reference as a parameter will return VX_SUCCESS if
     * the function was successful[*REQ*].\n
     * @ingroup group_int_import
     */
    vx_reference getReferenceByName(const vx_char* name);

    /*! \brief Destroys an Import and it's scoped-objects.
     *  \ingroup group_int_import
     */
    void destruct() override final;
#endif /* defined(OPENVX_USE_IX) || defined(OPENVX_USE_XML) */

    /*! \brief The internal representation of any import object.
     * \ingroup group_int_import
     */
    /*! \brief The type of import */
    vx_enum import_type;
    /*! \brief The number of references in the import. */
    vx_uint32 count;
    /*! \brief The set of references in the import. */
    vx_reference* refs;
};

} // namespace corevx

#endif /* VX_IMPORT_H */