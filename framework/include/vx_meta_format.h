/*
 * Copyright (c) 2014-2017 The Khronos Group Inc.
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


#ifndef VX_META_FORMAT_H
#define VX_META_FORMAT_H

#include <VX/vx.h>

/*!
 * \file
 * \brief The internal meta format implementation
 *
 * \defgroup group_int_meta_format Internal Meta-Format API
 * \ingroup group_internal
 * \brief The Internal Meta Format API
 */

/*!
 * \brief This structure is used to extract meta data from a validation
 * function. If the data object between nodes is virtual, this will allow the
 * framework to automatically create the data object, if needed.
 * \ingroup group_int_meta_format
 */
namespace coreflow {

class MetaFormat : public Reference
{
public:
    /**
     * @brief Construct a new Meta Format object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_meta_format
     */
    MetaFormat(vx_context context, vx_reference scope);

    /**
     * @brief Destroy the Meta Format object
     * @ingroup group_int_meta_format
     */
    ~MetaFormat() = default;

    /**
     * @brief Create a new meta format
     *
     * @param context   The context associated with this meta format
     * @return vx_meta_format The meta format object
     * @ingroup group_int_meta_format
     */
    static vx_meta_format createMetaFormat(vx_context context);

    /**
     * @brief Check if two meta formats are valid
     *
     * @param meta1     The first meta format
     * @param meta2     The second meta format
     * @return vx_bool  True if valid, false otherwise
     * @ingroup group_int_meta_format
     */
    static vx_bool isValidMetaFormat(vx_meta_format meta1, vx_meta_format meta2);

    /**
     * @brief Check if two meta formats are equal
     *
     * @param meta1     The first meta format
     * @param meta2     The second meta format
     * @param ref_type  The reference type
     * @return vx_bool  True if equal, false otherwise
     * @ingroup group_int_meta_format
     *
     */
    static vx_bool isMetaFormatEqual(
        vx_meta_format meta1, vx_meta_format meta2, vx_enum ref_type);

    /*! \brief Check if two image meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatImageEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two array meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatArrayEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two scalar meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatScalarEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two pyramid meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatPyramidEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two matrix meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatMatrixEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two distribution meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatDistributionEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two convolution meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatConvolutionEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two threshold meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatThresholdEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two remap meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatRemapEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two LUT meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatLutEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two object array meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatObjectArrayEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two tensor meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatTensorEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*! \brief Check if two user data object meta formats are equal
     * \param [in] meta1 The first meta format
     * \param [in] meta2 The second meta format
     * \ingroup group_int_meta_format
     */
    static vx_bool isMetaFormatUserDataObjectEqual(vx_meta_format meta1, vx_meta_format meta2);

    /*!< \brief The size of struct. */
    vx_size size;
    /*!< \brief The <tt>\ref vx_type_e</tt> or <tt>\ref vx_df_image_e</tt> code */
    vx_enum type;
    /*! \brief A struct of the configuration types needed by all object types. */
    struct dim {
        /*! \brief When a VX_TYPE_IMAGE */
        struct image {
            vx_uint32 width;    /*!< \brief The width of the image in pixels */
            vx_uint32 height;   /*!< \brief The height of the image in pixels */
            vx_df_image format;   /*!< \brief The format of the image. */
            vx_rectangle_t delta; /*!< \brief The delta rectangle applied to this image */
        } image;
        /*! \brief When a VX_TYPE_PYRAMID is specified */
        struct pyramid {
            vx_uint32 width;    /*!< \brief The width of the 0th image in pixels. */
            vx_uint32 height;   /*!< \brief The height of the 0th image in pixels. */
            vx_df_image format;   /*!< \brief The <tt>\ref vx_df_image_e</tt> format of the image. */
            vx_size levels;     /*!< \brief The number of scale levels */
            vx_float32 scale;   /*!< \brief The ratio between each level */
        } pyramid;
        /*! \brief When a VX_TYPE_SCALAR is specified */
        struct scalar {
            vx_enum type;       /*!< \brief The type of the scalar */
        } scalar;
        /*! \brief When a VX_TYPE_ARRAY is specified */
        struct array {
            vx_enum item_type;  /*!< \brief The type of the Array items */
            vx_size capacity;   /*!< \brief The capacity of the Array */
        } array;
        /*! \brief When a VX_TYPE_MATRIX is specified */
        struct matrix {
            vx_enum type;       /*!< \brief The value type of the matrix*/
            vx_size rows;       /*!< \brief The M dimension of the matrix*/
            vx_size cols;       /*!< \brief The N dimension of the matrix*/
        } matrix;
        /*! \brief When a VX_TYPE_DISTRIBUTION is specified */
        struct distribution {
            vx_size bins;       /*!< \brief Indicates the number of bins*/
            vx_int32 offset;    /*!< \brief Indicates the start of the values to use (inclusive)*/
            vx_uint32 range;    /*!< \brief Indicates the total number of the consecutive values of the distribution interval*/
        } distribution;
        /*! \brief When a VX_TYPE_REMAP is specified */
        struct remap {
            vx_uint32 src_width; /*!< \brief The source width*/
            vx_uint32 src_height;/*!< \brief The source height*/
            vx_uint32 dst_width; /*!< \brief The destination width*/
            vx_uint32 dst_height;/*!< \brief The destination width*/
        } remap;
        /*! \brief When a VX_TYPE_LUT is specified */
        struct lut {
            vx_enum type;        /*!< \brief Indicates the value type of the LUT*/
            vx_size count;       /*!< \brief Indicates the number of elements in the LUT*/
        } lut;
        /*! \brief When a VX_TYPE_THRESHOLD is specified */
        struct threshold {
            vx_enum type; /*!< \brief The value type of the threshold*/
        } threshold;
        /*! \brief When a VX_TYPE_OBJECT_ARRAY is specified */
        struct object_array {
            vx_enum item_type;  /*!< \brief The type of the ObjectArray items */
            vx_size num_items;  /*!< \brief The number of ObjectArray items */
        } object_array;
        struct tensor {
            vx_size number_of_dimensions;                 /*! \brief Number of dimensions */
            vx_size dimensions[VX_MAX_TENSOR_DIMENSIONS];   /*! \brief Size of all dimensions */
            vx_enum data_type;                              /*! \brief Type of data element */
            vx_int8 fixed_point_position;                  /*! \brief Fixed point position */
        } tensor;
        /*! \brief When a VX_TYPE_USER_DATA_OBJECT is specified */
        struct user_data_object {
            vx_size size;  /*!< \brief The size of the user data object */
            vx_char type_name[VX_MAX_REFERENCE_NAME];  /*!< \brief The name of the user data object */
        } user_data_object;
    } dim;

    vx_kernel_image_valid_rectangle_f set_valid_rectangle_callback;
};

} // namespace coreflow

/*! \brief Creates a metaformat object.
 * \param [in] context The overall context object.
 * \ingroup group_int_meta_format
 */
vx_meta_format vxCreateMetaFormat(vx_context context);

/*! \brief Releases a meta-format object.
 * \param [in,out] pmeta
 * \ingroup group_int_meta_format
 */
vx_status vxReleaseMetaFormat(vx_meta_format *pmeta);

#endif /* VX_META_FORMAT_H */