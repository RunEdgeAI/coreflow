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

#ifndef VX_CONVOLUTION_H
#define VX_CONVOLUTION_H

#include "vx_internal.h"
#include "vx_matrix.h"

/*!
 * \file
 * \brief The internal convolution implementation
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_convolution Internal Convolution API
 * \ingroup group_internal
 * \brief The Internal Convolution API.
 */

/*! \brief Destroys a convolution.
 * \ingroup group_int_convolution
 */
void ownDestructConvolution(vx_reference ref);

/*! \brief A convolution is a special type of matrix (MxM)
 * \ingroup group_int_convolution
 */
class Convolution : public Matrix
{
public:
    /**
     * @brief Construct a new Convolution object
     *
     * @param context
     * @param scope
     */
    Convolution(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Convolution object
     *
     */
    ~Convolution();

    /*!< \brief The Scale Factor. */
    vx_uint32 scale;
};

#endif /* VX_CONVOLUTION_H */
