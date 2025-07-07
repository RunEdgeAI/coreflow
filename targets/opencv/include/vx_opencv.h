/**
 * @file vx_opencv.h
 * @brief OpenCV Extension Kernel Definitions
 * @ingroup group_opencv
 * @date 2025-07-03
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#ifndef VX_CV_H
#define VX_CV_H

#include <VX/vx.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 * \brief The extension library for OpenCV
 * \ingroup group_opencv
 */
#define VX_LIBRARY_OPENCV         1

/*!
 * \brief The list of available vision kernels in the OpenCV extension library.
 * \ingroup group_opencv
 */
enum vx_kernel_cv_ext_e
{
    /*!
     * \brief The OpenCV blur function kernel. Kernel name is "org.opencv.blur".
     * \defgroup group_opencv_blur Kernel: OpenCV Blur
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BLUR = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x100,

    /*!
     * \brief The OpenCV medianBlur function kernel. Kernel name is "org.opencv.medianblur".
     * \defgroup group_opencv_median_blur Kernel: OpenCV Median Blur
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_MEDIAN_BLUR = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x101,

    /*!
     * \brief The OpenCV GaussianBlur function kernel. Kernel name is "org.opencv.gaussianblur".
     * \defgroup group_opencv_gaussian Kernel: OpenCV Gaussian Blur
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_GAUSSIAN_BLUR = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x102,

    /*!
     * \brief The OpenCV boxFilter function kernel. Kernel name is "org.opencv.boxfilter".
     * \defgroup group_opencv_box_filter Kernel: OpenCV Box Filter
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BOXFILTER = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x103,

    /*!
     * \brief The OpenCV BilateralFilter function kernel. Kernel name is
     * "org.opencv.bilateralfilter".
     * \defgroup group_opencv_bilateral_filter Kernel: OpenCV Bilateral Filter
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BILATERAL_FILTER = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x104,

    /*!
     * \brief The OpenCV Flip function kernel. Kernel name is "org.opencv.flip".
     * \defgroup group_opencv_flip Kernel: OpenCV Flip
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_FLIP = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x37,

    /*!
     * \brief The OpenCV transpose function kernel. Kernel name is "org.opencv.transpose".
     * \defgroup group_opencv_transpose Kernel: OpenCV Transpose
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_TRANSPOSE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x39,

    /*!
     * \brief The OpenCV absdiff function kernel. Kernel name is "org.opencv.absdiff".
     * \defgroup group_opencv_absdiff Kernel: OpenCV AbsDiff
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_ABSDIFF = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x40,

    /*!
     * \brief The OpenCV add function kernel. Kernel name is "org.opencv.add".
     * \defgroup group_opencv_add Kernel: OpenCV Add
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_ADD = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x41,

    /*!
     * \brief The OpenCV bitwise_and function kernel. Kernel name is "org.opencv.bitwise_and".
     * \defgroup group_opencv_bitwise_and Kernel: OpenCV Bitwise And
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BITWISE_AND = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x42,

    /*!
     * \brief The OpenCV bitwise_not function kernel. Kernel name is "org.opencv.flip".
     * \defgroup group_opencv_bitwise_not Kernel: OpenCV Bitwise Not
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BITWISE_NOT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x43,

    /*!
     * \brief The OpenCV bitwise_or function kernel. Kernel name is "org.opencv.bitwise_or".
     * \defgroup group_opencv_bitwise_or Kernel: OpenCV Bitwise Or
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BITWISE_OR = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x44,

    /*!
     * \brief The OpenCV bitwise_xor function kernel. Kernel name is "org.opencv.bitwise_xor".
     * \defgroup group_opencv_bitwise_xor Kernel: OpenCV Bitwise Xor
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BITWISE_XOR = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x45,

    /*!
     * \brief The OpenCV subtract function kernel. Kernel name is "org.opencv.subtract".
     * \defgroup group_opencv_subtract Kernel: OpenCV Subtract
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SUBTRACT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x46,

    /*!
     * \brief The OpenCV compare function kernel. Kernel name is "org.opencv.compare".
     * \defgroup group_opencv_compare Kernel: OpenCV Compare
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_COMPARE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x47,

    /*!
     * \brief The OpenCV SOBEL function kernel. Kernel name is "org.opencv.sobel".
     * \defgroup group_opencv_sobel Kernel: OpenCV Sobel
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SOBEL = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x30,

    /*!
     * \brief The OpenCV CONVERTSCALEABS function kernel. Kernel name is
     * "org.opencv.convertscaleabs".
     * \defgroup group_opencv_convertscaleabs Kernel: OpenCV ConvertScaleAbs
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_CONVERTSCALEABS = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x31,

    /*!
     * \brief The OpenCV ADDWEIGHTED function kernel. Kernel name is "org.opencv.addweighted".
     * \defgroup group_opencv_addweighted Kernel: OpenCV AddWeighted
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_ADDWEIGHTED = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x32,

    /*!
     * \brief The OpenCV CANNY function kernel. Kernel name is "org.opencv.canny".
     * \defgroup group_opencv_canny Kernel: OpenCV Canny
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_CANNY = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x33,

    /*!
     * \brief The OpenCV LAPLACIAN function kernel. Kernel name is "org.opencv.laplacian".
     * \defgroup group_opencv_laplacian Kernel: OpenCV Laplacian
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_LAPLACIAN = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x34,

    /*!
     * \brief The OpenCV MORPHOLOGYEX function kernel. Kernel name is "org.opencv.morphologyex".
     * \defgroup group_opencv_morphologyex Kernel: OpenCV MorphologyEx
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_MORPHOLOGYEX = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x35,

    /*!
     * \brief The OpenCV SCHARR function kernel. Kernel name is "org.opencv.scharr".
     * \defgroup group_opencv_scharr Kernel: OpenCV Scharr
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SCHARR = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x36,

    /*!
     * \brief The OpenCV FAST feature detector function kernel. Kernel name is "org.opencv.fast".
     * \defgroup group_opencv_fast Kernel: OpenCV Fast
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_FAST = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x12,

    /*!
     * \brief The OpenCV GoodFeaturesToTrack (GFTT) detector function kernel. Kernel name is
     * "org.opencv.good_features_to_track".
     * \defgroup group_opencv_good_features_to_track Kernel: OpenCV Good Features To Track
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_GOOD_FEATURE_TO_TRACK = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x13,

    /*!
     * \brief The OpenCV SIFT detector function kernel.Kernel name is "org.opencv.sift_detect".
     * \defgroup group_opencv_sift_detect Kernel: OpenCV SIFT Detect
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SIFT_DETECT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x10,

    /*!
     * \brief The OpenCV SURF detector function kernel. Kernel name is "org.opencv.surf_detect".
     * \defgroup group_opencv_surf_detect Kernel: OpenCV SURF Detect
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SURF_DETECT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x11,

    /*!
     * \brief The OpenCV BRISK detector function kernel. Kernel name is "org.opencv.brisk_detect".
     * \defgroup group_opencv_brisk_detect Kernel: OpenCV BRISK Detect
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BRISK_DETECT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x14,

    /*!
     * \brief The OpenCV MSER feature detector function kernel. Kernel name is
     * "org.opencv.mser_detect".
     * \defgroup group_opencv_mser_detect Kernel: OpenCV MSER Detect
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_MSER_DETECT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x16,

    /*!
     * \brief The OpenCV ORB detector function kernel. Kernel name is "org.opencv.orb_detect".
     * \defgroup group_opencv_orb_detect Kernel: OpenCV ORB Detect
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_ORB_DETECT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x17,

    /*!
     * \brief The OpenCV Simple Blob detector function kernel. Kernel name is
     * "org.opencv.simple_blob_detect".
     * \defgroup group_opencv_simple_blob_detect Kernel: OpenCV Simple Blob Detect
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SIMPLE_BLOB_DETECT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x18,

    /*!
     * \brief The OpenCV simple_blob_detector_initialize function kernel. Kernel name is
     * "org.opencv.simple_blob_detector_initialize".
     * \defgroup group_opencv_simple_blob_detector_initialize Kernel: OpenCV Simple Blob Detector
     * Initialize
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SIMPLE_BLOB_DETECT_INITIALIZE =
        VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x19,

    /*!
     * \brief The OpenCV STAR feature detector function kernel. Kernel name is
     * "org.opencv.star_detect".
     * \defgroup group_opencv_star_detect Kernel: OpenCV STAR Detect
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_STAR_FEATURE_DETECT = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x20,

    /*!
     * \brief The OpenCV SIFT descriptor function kernel. Kernel name is "org.opencv.sift_compute".
     * \defgroup group_opencv_sift_compute Kernel: OpenCV SIFT Compute
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SIFT_COMPUTE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x21,

    /*!
     * \brief The OpenCV SURF descriptor function kernel. Kernel name is "org.opencv.surf_compute".
     * \defgroup group_opencv_surf_compute Kernel: OpenCV SURF Compute
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SURF_COMPUTE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x22,

    /*!
     * \brief The OpenCV BRISK descriptor function kernel. Kernel name is
     * "org.opencv.brisk_compute".
     * \defgroup group_opencv_brisk_compute Kernel: OpenCV BRISK Compute
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BRISK_COMPUTE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x23,

    /*!
     * \brief The OpenCV ORB descriptor function kernel. Kernel name is "org.opencv.orb_compute".
     * \defgroup group_opencv_orb_compute Kernel: OpenCV ORB Compute
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_ORB_COMPUTE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x24,

    /*!
     * \brief The OpenCV MULTIPLY function kernel. Kernel name is "org.opencv.multiply".
     * \defgroup group_opencv_multiply Kernel: OpenCV Multiply
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_MULTIPLY = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x51,

    /*!
     * \brief The OpenCV Divide function kernel. Kernel name is "org.opencv.divide".
     * \defgroup group_opencv_divide Kernel: OpenCV Divide
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_DIVIDE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x52,

    /*!
     * \brief The OpenCV ADAPTIVETHRESHOLD function kernel. Kernel name is
     * "org.opencv.adaptivethreshold".
     * \defgroup group_opencv_adaptivethreshold Kernel: OpenCV AdaptiveThreshold
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_ADAPTIVETHRESHOLD = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x53,

    /*!
     * \brief The OpenCV DISTANCETRANSFORM function kernel. Kernel name is
     * "org.opencv.distancetransform".
     * \defgroup group_opencv_distancetransform Kernel: OpenCV DistanceTransform
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_DISTANCETRANSFORM = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x54,

    /*!
     * \brief The OpenCV cvtcolor function kernel. Kernel name is "org.opencv.cvtcolor".
     * \defgroup group_opencv_cvtcolor Kernel: OpenCV CvtColor
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_CVTCOLOR = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x55,

    /*!
     * \brief The OpenCV Threshold function kernel. Kernel name is "org.opencv.threshold".
     * \defgroup group_opencv_threshold Kernel: OpenCV Threshold
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_THRESHOLD = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x56,

    /*!
     * \brief The OpenCV fastNlMeansDenoising function kernel. Kernel name is
     * "org.opencv.fastnlmeansdenoising".
     * \defgroup group_opencv_fastnlmeansdenoising Kernel: OpenCV FastNlMeansDenoising
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_FAST_NL_MEANS_DENOISING = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x57,

    /*!
     * \brief The OpenCV fastNlMeansDenoising Colored function kernel. Kernel name is
     * "org.opencv.fastnlmeansdenoisingcolored".
     * \defgroup group_opencv_fastnlmeansdenoisingcolored Kernel: OpenCV FastNlMeansDenoisingColored
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_FAST_NL_MEANS_DENOISING_COLORED =
        VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x58,

    /*!
     * \brief The OpenCV pyrup function kernel. Kernel name is "org.opencv.pyrup".
     * \defgroup group_opencv_pyrup Kernel: OpenCV PyrUp
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_PYRUP = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x59,

    /*!
     * \brief The OpenCV pyrdown function kernel. Kernel name is "org.opencv.pyrdown".
     * \defgroup group_opencv_pyrdown Kernel: OpenCV PyrDown
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_PYRDOWN = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x60,

    /*!
     * \brief The OpenCV filter2D function kernel. Kernel name is "org.opencv.filter2D".
     * \defgroup group_opencv_filter2d Kernel: OpenCV Filter2D
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_FILTER_2D = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x61,

    /*!
     * \brief The OpenCV sepFilter2D function kernel. Kernel name is "org.opencv.sepFilter2D".
     * \defgroup group_opencv_sepfilter2d Kernel: OpenCV SepFilter2D
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_SEPFILTER_2D = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x62,

    /*!
     * \brief The OpenCV dilate function kernel. Kernel name is "org.opencv.dilate".
     * \defgroup group_opencv_dilate Kernel: OpenCV Dilate
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_DILATE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x63,

    /*!
     * \brief The OpenCV erode function kernel. Kernel name is "org.opencv.erode".
     * \defgroup group_opencv_erode Kernel: OpenCV Erode
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_ERODE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x64,

    /*!
     * \brief The OpenCV warpAffine function kernel. Kernel name is "org.opencv.warpAffine".
     * \defgroup group_opencv_warpaffine Kernel: OpenCV WarpAffine
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_WARP_AFFINE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x65,

    /*!
     * \brief The OpenCV warpPerspective function kernel. Kernel name is
     * "org.opencv.warpPerspective".
     * \defgroup group_opencv_warpperspective Kernel: OpenCV WarpPerspective
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_WARP_PERSPECTIVE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x66,

    /*!
     * \brief The OpenCV resize function kernel. Kernel name is "org.opencv.resize".
     * \defgroup group_opencv_resize Kernel: OpenCV Resize
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_RESIZE = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x67,

    /*!
     * \brief The OpenCV buildPyramid function kernel. Kernel name is "org.opencv.buildPyramid".
     * \defgroup group_opencv_buildpyramid Kernel: OpenCV BuildPyramid
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BUILD_PYRAMID = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x68,

    /*!
     * \brief The OpenCV Flip function kernel. Kernel name is "org.opencv.Flip".
     * \defgroup group_opencv_build_optical_flow_pyramid Kernel: OpenCV Build Optical Flow Pyramid
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_BUILD_OPTICAL_FLOW_PYRAMID =
        VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x69,

    /*!
     * \brief The OpenCV integral function kernel. Kernel name is "org.opencv.integral".
     * \defgroup group_opencv_integral Kernel: OpenCV Integral
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_INTEGRAL = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x70,

    /*!
     * \brief The OpenCV countNonZero function kernel. Kernel name is "org.opencv.countnonzero".
     * \defgroup group_opencv_countnonzero Kernel: OpenCV CountNonZero
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_COUNT_NON_ZERO = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x71,

    /*!
     * \brief The OpenCV norm function kernel. Kernel name is "org.opencv.norm".
     * \defgroup group_opencv_norm Kernel: OpenCV Norm
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_NORM = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x72,

    /*!
     * \brief The OpenCV CORNERHARRIS function kernel. Kernel name is "org.opencv.cornerharris".
     * \defgroup group_opencv_cornerharris Kernel: OpenCV CornerHarris
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_CORNERHARRIS = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x201,

    /*!
     * \brief The OpenCV cornerMinEigenVal function kernel. Kernel name is
     * "org.opencv.cornermineigenVal".
     * \defgroup group_opencv_cornermineigenval Kernel: OpenCV CornerMinEigenVal
     * \ingroup group_all_kernels
     */
    VX_KERNEL_OPENCV_CORNER_MIN_EIGEN_VAL = VX_KERNEL_BASE(VX_ID_AMD, VX_LIBRARY_OPENCV) + 0x202,

};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VX_CV_H */
