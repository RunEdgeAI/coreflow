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

#ifndef VX_DISTRIBUTION_H
#define VX_DISTRIBUTION_H

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal distribution implementation
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_distribution Internal Distribution API
 * \ingroup group_internal
 * \brief The Internal Distribution API.
 */

/*! \brief Destroys a distribution.
 * \param [in] ref The generic handle to the object.
 * \ingroup group_int_distribution
 */
void ownDestructDistribution(vx_reference ref);


 /*! \brief A Distribution.
  * \ingroup group_int_distribution
  */
class Distribution : public Reference
{
public:
    /**
     * @brief Construct a new Distribution object
     *
     * @param context
     * @param scope
     */
    Distribution(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Distribution object
     *
     */
    ~Distribution();

    /*! \brief Memory layout */
    vx_memory_t memory;
    /*! \brief The total number of the values in the active X dimension of the distribution. */
    vx_uint32 range_x;
    /*! \brief The total number of the values in the active Y dimension of the distribution. */
    vx_uint32 range_y;
    /*! \brief The number of inactive elements from zero in the X dimension */
    vx_int32 offset_x;
    /*! \brief The number of inactive elements from zero in the Y dimension */
    vx_int32 offset_y;
};

#endif /* VX_DISTRIBUTION_H */
