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
#ifndef VX_THRESHOLD_H
#define VX_THRESHOLD_H

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal thresholds implementation.
 * \author Erik Rainey <erik.rainey@gmail.com>
 * \defgroup group_int_threshold Internal Threshold API
 * \ingroup group_internal
 * \brief The Internal Threshold API.
 */

/*! \brief Releases an internally referenced threshold.
 * \param [in] threshold The pointer to a threshold structure.
 * \ingroup group_int_threshold
 */
void ownReleaseThresholdInt(vx_threshold* threshold);

/*! \brief The internal threshold structure.
 * \ingroup group_int_threshold
 */
class Threshold : public Reference
{
public:
    /**
     * @brief Construct a new Threshold object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_threshold
     */
    Threshold(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Threshold object
     * @ingroup group_int_threshold
     */
    ~Threshold();

    /**
     * @brief Validate threshold type
     *
     * @param thresh_type
     * @return vx_bool
     * @ingroup group_int_threshold
     */
    static vx_bool isValidThresholdType(vx_enum thresh_type);

    /**
     * @brief Validate theshold data type
     *
     * @param data_type
     * @return vx_bool
     * @ingroup group_int_threshold
     */
    static vx_bool isValidThresholdDataType(vx_enum data_type);

    /*! \brief From \ref vx_threshold_type_e */
    vx_enum thresh_type;
    /*! \brief From \ref vx_type_e */
    vx_enum data_type;
    /*! \brief The binary threshold value */
    vx_pixel_value_t value;
    /*! \brief Lower bound for range threshold */
    vx_pixel_value_t lower;
    /*! \brief Upper bound for range threshold */
    vx_pixel_value_t upper;
    /*! \brief True value for output */
    vx_pixel_value_t true_value;
    /*! \brief False value for output */
    vx_pixel_value_t false_value;
    /*! \brief The input image format */
    vx_df_image input_format;
    /*! \brief The output image format  */
    vx_df_image output_format;
};

#endif /* VX_THRESHOLD_H */
