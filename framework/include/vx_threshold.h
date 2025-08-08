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
namespace corevx {

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
     * @brief Set the binary threshold value
     *
     * @param value The binary threshold value
     * @ingroup group_int_threshold
     */
    void setBinaryValue(vx_int32 value);

    /**
     * @brief Set the binary threshold value as a pixel value union
     *
     * @param value The binary threshold value as a pixel value union
     * @ingroup group_int_threshold
     */
    void setBinaryValueUnion(vx_pixel_value_t value);

    /**
     * @brief Set the lower bound for range threshold
     *
     * @param lower The lower bound value
     * @ingroup group_int_threshold
     */
    void setLowerBound(vx_int32 lower);

    /**
     * @brief Set the lower bound for range threshold as a pixel value union
     *
     * @param lower The lower bound value as a pixel value union
     * @ingroup group_int_threshold
     */
    void setLowerBoundUnion(vx_pixel_value_t lower);

    /**
     * @brief Set the upper bound for range threshold
     *
     * @param upper The upper bound value
     * @ingroup group_int_threshold
     */
    void setUpperBound(vx_int32 upper);

    /**
     * @brief Set the upper bound for range threshold as a pixel value union
     *
     * @param upper The upper bound value as a pixel value union
     * @ingroup group_int_threshold
     */
    void setUpperBoundUnion(vx_pixel_value_t upper);

    /**
     * @brief Set the true value for output
     *
     * @param true_value The true value
     * @ingroup group_int_threshold
     */
    void setTrueValue(vx_int32 true_value);

    /**
     * @brief Set the true value for output as a pixel value union
     *
     * @param true_value The true value as a pixel value union
     * @ingroup group_int_threshold
     */
    void setTrueValueUnion(vx_pixel_value_t true_value);

    /**
     * @brief Set the false value for output
     *
     * @param false_value The false value
     * @ingroup group_int_threshold
     */
    void setFalseValue(vx_int32 false_value);

    /**
     * @brief Set the false value for output as a pixel value union
     *
     * @param false_value The false value as a pixel value union
     * @ingroup group_int_threshold
     */
    void setFalseValueUnion(vx_pixel_value_t false_value);

    /**
     * @brief Get the binary threshold value
     *
     * @return vx_int32 The binary threshold value
     * @ingroup group_int_threshold
     */
    vx_int32 binaryValue() const;

    /**
     * @brief Get the binary threshold value as a pixel value union
     *
     * @return vx_pixel_value_t
     * @ingroup group_int_threshold
     */
    vx_pixel_value_t binaryValueUnion() const;

    /**
     * @brief Get the lower bound for range threshold
     *
     * @return vx_int32 The lower bound value
     * @ingroup group_int_threshold
     */
    vx_int32 lowerBound() const;

    /**
     * @brief Get the lower bound for range threshold as a pixel value union
     *
     * @return vx_pixel_value_t
     * @ingroup group_int_threshold
     */
    vx_pixel_value_t lowerBoundUnion() const;

    /**
     * @brief Get the upper bound for range threshold
     *
     * @return vx_int32 The upper bound value
     * @ingroup group_int_threshold
     */
    vx_int32 upperBound() const;

    /**
     * @brief Get the upper bound for range threshold as a pixel value union
     *
     * @return vx_pixel_value_t
     * @ingroup group_int_threshold
     */
    vx_pixel_value_t upperBoundUnion() const;

    /**
     * @brief Get the true value for output
     *
     * @return vx_int32 The true value
     * @ingroup group_int_threshold
     */
    vx_int32 trueValue() const;

    /**
     * @brief Get the true value for output as a pixel value union
     *
     * @return vx_pixel_value_t
     * @ingroup group_int_threshold
     */
    vx_pixel_value_t trueValueUnion() const;

    /**
     * @brief Get the false value for output
     *
     * @return vx_int32 The false value
     * @ingroup group_int_threshold
     */
    vx_int32 falseValue() const;

    /**
     * @brief Get the false value for output as a pixel value union
     *
     * @return vx_pixel_value_t
     * @ingroup group_int_threshold
     */
    vx_pixel_value_t falseValueUnion() const;

    /**
     * @brief Get the data type of the threshold
     *
     * @return vx_enum The data type
     * @ingroup group_int_threshold
     */
    vx_enum dataType() const;

    /**
     * @brief Get the threshold type
     *
     * @return vx_enum The threshold type
     * @ingroup group_int_threshold
     */
    vx_enum threshType() const;

    /**
     * @brief Get the input image format
     *
     * @return vx_df_image The input image format
     * @ingroup group_int_threshold
     */
    vx_df_image inputFormat() const;

    /**
     * @brief Get the output image format
     *
     * @return vx_df_image The output image format
     * @ingroup group_int_threshold
     */
    vx_df_image outputFormat() const;

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

    /**
     * @brief Validate threshold format
     *
     * @param format   Threshold format to validate
     * @return vx_bool true if valid, false otherwise
     * @ingroup group_int_threshold
     */
    static vx_bool isValidThresholdFormat(vx_df_image format);

    /**
     * @brief Copy the threshold output to/from user memory
     *
     * @param true_value_ptr   Pointer to the true value
     * @param false_value_ptr  Pointer to the false value
     * @param usage            Memory usage type (read/write)
     * @param user_mem_type    The type of memory (host, opencl, etc.)
     * @return vx_status       VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_threshold
     */
    vx_status copyOutput(vx_pixel_value_t *true_value_ptr, vx_pixel_value_t *false_value_ptr,
                         vx_enum usage, vx_enum user_mem_type);

    /**
     * @brief Copy the threshold range to/from user memory
     * @param lower_value_ptr  Pointer to the lower value
     * @param upper_value_ptr  Pointer to the upper value
     * @param usage            Memory usage type (read/write)
     * @param user_mem_type    The type of memory (host, opencl, etc.)
     * @return vx_status       VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_threshold
     */
    vx_status copyRange(vx_pixel_value_t *lower_value_ptr, vx_pixel_value_t *upper_value_ptr,
                        vx_enum usage, vx_enum user_mem_type);

    /**
     * @brief Copy the threshold value to/from user memory
     * @param value_ptr       Pointer to the value
     * @param usage           Memory usage type (read/write)
     * @param user_mem_type   The type of memory (host, opencl, etc.)
     * @return vx_status      VX_SUCCESS on success, error code otherwise
     * @ingroup group_int_threshold
     */
    vx_status copyValue(vx_pixel_value_t *value_ptr, vx_enum usage, vx_enum user_mem_type);

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

} // namespace corevx


#endif /* VX_THRESHOLD_H */
