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
 * \author Jesse Villarreal <jesse.villarreal@ti.com>
 *
 * \defgroup group_int_import Internal Import Object API
 * \ingroup group_internal
 * \brief The Internal Import Object API
 */

/*! \brief The internal representation of a \ref vx_import
 * \ingroup group_int_import
 */
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

    /*! \brief Destroys an Import and it's scoped-objects.
     *  \ingroup group_int_import
     */
    void destructImport();

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

#endif /* VX_IMPORT_H */
