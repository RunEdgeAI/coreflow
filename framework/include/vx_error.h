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

#ifndef VX_ERROR_H
#define VX_ERROR_H

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal error implementation
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_error Internal Error API
 * \ingroup group_internal
 * \brief The Internal Error API.
 */


/*! \brief The internal representation of the error object.
 * \ingroup group_int_error
 */
class Error : public Reference
{
public:
    /**
     * @brief Construct a new Error object
     *
     * @ingroup group_int_error
     */
    Error(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Error object
     *
     * @ingroup group_int_error
     */
    ~Error();

    /*! \brief Creates all the status codes as error objects.
     * \ingroup group_int_error
     */
    static vx_bool createConstErrors(vx_context context);

    /*! \brief Creates an Error Object.
     * \ingroup group_int_error
     */
    static vx_error allocateError(vx_context context, vx_status status);

    /*! \brief Releases an error object.
     * \ingroup group_int_error
     */
    static void releaseError(vx_error* error);

    /*! \brief The specific error code contained in this object. */
    vx_status status;
};

/*! \brief Matches the status code against all known error objects in the
 * context.
 * \param [in] context The pointer to the overall context.
 * \param [in] status The status code to find.
 * \return Returns a matching error object.
 */
vx_error vxGetErrorObject(vx_context context, vx_status status);

#endif /* VX_ERROR_H */
