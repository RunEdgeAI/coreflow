/**
 * @file vx_corevx_ext.h
 * @brief Extensions enabled for corevx
 * @version 0.1
 * @date 2024-12-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef VX_COREVX_EXT_H
#define VX_COREVX_EXT_H

#include <VX/vx_kernels.h>
#include <VX/vx_types.h>
#include <VX/vx_vendors.h>

/*!
 * \defgroup group_extensions All OpenVX Extensions
 * \brief A comprehensive collection of all OpenVX extensions implemented.
 *
 * \defgroup group_all_kernels All OpenVX Kernel Functions
 * \brief A comprehensive collection of all OpenVX kernel functions available.
 *
 * \defgroup group_external OpenVX User Facing Implementation
 * \brief The OpenVX User Facing Implementation.
 * \details A proprietary, Closed Source implementation of the OpenVX implementation
 *
 * \defgroup group_adv_array User Facing Custom Struct API
 * \ingroup group_external
 * \brief The Public User Struct API
 *
 * \defgroup group_adv_node User Facing Custom Node API
 * \ingroup group_external
 * \brief The Public Generic Node API
 *
 * \defgroup group_array User Facing Array API
 * \ingroup group_external
 * \brief The Public Array API
 *
 * \defgroup group_basic_features User Facing Basic Features API
 * \ingroup group_external
 * \brief The Public Basic Features API
 *
 * \defgroup group_borders User Facing Borders API
 * \ingroup group_external
 * \brief The Public Borders API
 *
 * \defgroup group_context User Facing Context API
 * \ingroup group_external
 * \brief The Public Context API
 *
 * \defgroup group_convolution User Facing Convolution API
 * \ingroup group_external
 * \brief The Public Convolution API
 *
 * \defgroup group_delay User Facing Delay API
 * \ingroup group_external
 * \brief The Public Delay API
 *
 * \defgroup group_directive User Facing Directive API
 * \ingroup group_external
 * \brief The Public Directive API
 *
 * \defgroup group_distribution User Facing Distribution API
 * \ingroup group_external
 * \brief The Public API
 *
 * \defgroup group_graph User Facing Graph API
 * \ingroup group_external
 * \brief The Public Graph API
 *
 * \defgroup group_graph_parameters User Facing Graph Parameters API
 * \ingroup group_external
 * \brief The Public Graph Parameters API
 *
 * \defgroup group_hint User Facing Hint API
 * \ingroup group_external
 * \brief The Public Hint API
 *
 * \defgroup group_image User Facing Image API
 * \ingroup group_external
 * \brief The Public Image API
 *
 * \defgroup group_import User Facing Import API
 * \ingroup group_external
 * \brief The Public Import API
 *
 * \defgroup group_import_kernel User Facing Import Kernel API
 * \ingroup group_external
 * \brief The Public Import Kernel API
 *
 * \defgroup group_kernel User Facing Kernel API
 * \ingroup group_external
 * \brief The Public Kernel API
 *
 * \defgroup group_log User Facing Log API
 * \ingroup group_external
 * \brief The Public Log API
 *
 * \defgroup group_lut User Facing LUT API
 * \ingroup group_external
 * \brief The Public LUT API
 *
 * \defgroup group_matrix User Facing Matrix API
 * \ingroup group_external
 * \brief The Public Matrix API
 *
 * \defgroup group_meta_format User Facing Meta Format API
 * \ingroup group_external
 * \brief The Public Meta Format API
 *
 * \defgroup group_node User Facing Node API
 * \ingroup group_external
 * \brief The Public Node API
 *
 * \defgroup group_node_callback User Facing Node Callback API
 * \ingroup group_external
 * \brief The Public Node Callback API
 *
 * \defgroup group_object_array User Facing Object Array API
 * \ingroup group_external
 * \brief The Public Object Array API
 *
 * \defgroup group_osal User Facing OS Abstratction Layer API
 * \ingroup group_external
 * \brief The Public OSAL API
 *
 * \defgroup group_parameter User Facing Parameter API
 * \ingroup group_external
 * \brief The Public API
 *
 * \defgroup group_performance User Facing Performance API
 * \ingroup group_external
 * \brief The Public Performance API
 *
 * \defgroup group_pyramid User Facing Pyramid API
 * \ingroup group_external
 * \brief The Public Pyramid API
 *
 * \defgroup group_reference User Facing Reference API
 * \ingroup group_external
 * \brief The Public Reference API
 *
 * \defgroup group_remap User Facing Remap API
 * \ingroup group_external
 * \brief The Public Remap API
 *
 * \defgroup group_scalar User Facing Scalar API
 * \ingroup group_external
 * \brief The Public Scalar API
 *
 * \defgroup group_target User Facing Target API
 * \ingroup group_external
 * \brief The Public Target API
 *
 * \defgroup group_tensor User Facing Tensor API
 * \ingroup group_external
 * \brief The Public Tensor API
 *
 * \defgroup group_threshold User Facing Threshold API
 * \ingroup group_external
 * \brief The Public Threshold API
 *
 * \defgroup group_user_data_object User Facing User Data Object API
 * \ingroup group_external
 * \brief The Public User Data Object API
 *
 * \defgroup group_user_kernels User Facing User Kernels API
 * \ingroup group_external
 * \brief The Public User Kernels API
 */

/*!
 * \brief The OpenVX EdgeAI Vendor Extension.
 * \defgroup group_corevx_ext Extension: AI/ML
 * \ingroup group_extensions
 *
 * \defgroup group_ort_function_cpu_inference Kernel: ORT Inference
 * \ingroup group_all_kernels
 * \brief An ONNX Runtime Inference Implementation
 *
 * \defgroup group_ai_function_chatbot Kernel: AI Chatbot
 * \ingroup group_all_kernels
 *
 * \defgroup group_litert_function_cpu_inference Kernel: LiteRT Inference
 * \ingroup group_all_kernels
 * \brief A LiteRT (Formerly TFLite) Inference Implementation
 *
 * \defgroup group_torch_function_cpu_inference Kernel: Executorch Inference
 * \ingroup group_all_kernels
 * \brief An Executorch Inference Implementation
 */

#ifdef __cplusplus
#include <string>

/*! \brief A character array (string) type.
 * \note This is a C++ string type. It is not a C string.
 * \ingroup group_basic_features
 */
using vx_string = std::string;
#else
typedef vx_char* vx_string;
#endif /* __cplusplus */

/*! \brief The type enumeration lists additional types to extend the known types in OpenVX.
 * \ingroup group_basic_features
 */
enum vx_type_ext_e
{
    VX_TYPE_STRING = 0x818, /*!< \brief A <tt>\ref vx_string</tt>. */
};

/*! \brief Define Edge AI Vendor ID
 * \ingroup group_basic_features
 */
enum vx_vendor_id_ext_e
{
    VX_ID_EDGE_AI = 0x80D
};

/*! \brief Define Edge AI Kernels
 * \ingroup group_corevx_ext
 */
enum vx_kernel_ext_e
{
    /*!
     * \brief The ONNX Runtime CPU Inference kernel.
     * \param [in] vx_array The input char array.
     * \param [in] vx_object_array The input tensor object array.
     * \param [out] vx_object_array The output tensor object array.
     * \see group_ort_function_cpu_inference
     */
    VX_KERNEL_ORT_CPU_INF = VX_KERNEL_BASE(VX_ID_EDGE_AI, VX_LIBRARY_KHR_BASE) + 0x1,
    /*!
     * \brief The AI Model Server Chatbot kernel.
     * \param [in] vx_array The input char array.
     * \param [out] vx_array The output char array.
     * \see group_ai_function_chatbot
     */
    VX_KERNEL_AIS_CHATBOT = VX_KERNEL_BASE(VX_ID_EDGE_AI, VX_LIBRARY_KHR_BASE) + 0x2,
    /*!
     * \brief The LiteRT CPU Inference kernel.
     * \param [in] vx_array The input char array.
     * \param [in] vx_object_array The input tensor object array.
     * \param [out] vx_object_array The output tensor object array.
     * \see group_litert_function_cpu_inference
     */
    VX_KERNEL_LITERT_CPU_INF = VX_KERNEL_BASE(VX_ID_EDGE_AI, VX_LIBRARY_KHR_BASE) + 0x3,
    /*!
     * \brief The Torch CPU Inference kernel.
     * \param [in] vx_array The input char array.
     * \param [in] vx_object_array The input tensor object array.
     * \param [out] vx_object_array The output tensor object array.
     * \see group_torch_function_cpu_inference
     */
    VX_KERNEL_TORCH_CPU_INF = VX_KERNEL_BASE(VX_ID_EDGE_AI, VX_LIBRARY_KHR_BASE) + 0x4,
};

/*! \brief addtitional tensor attributes.
 * \ingroup group_int_tensor
 */
enum vx_tensor_attribute_ext_e
{
    /*! \brief tensor strides in each dimension. */
    VX_TENSOR_STRIDE = VX_ATTRIBUTE_BASE(VX_ID_KHRONOS, VX_TYPE_TENSOR) + 0x4,
    /*! \brief tensor total size in bytes. */
    VX_TENSOR_TOTAL_SIZE = VX_ATTRIBUTE_BASE(VX_ID_KHRONOS, VX_TYPE_TENSOR) + 0x5,
};

/*!
 * \brief Creates a reference to an ObjectArray of a specific object type.
 *
 * It uses the enum value to determine the object attributes, No data objects are created by this function.
 *
 * \param [in] context      The reference to the overall Context.
 * \param [in] type         The enum of the object type to be contained in the ObjectArray.
 *
 * \returns An ObjectArray reference <tt>\ref vx_object_array</tt>. Any possible errors preventing a
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>. Data objects are not initialized by this function.
 *
 * \ingroup group_object_array
 */
VX_API_ENTRY vx_object_array VX_API_CALL vxCreateObjectArrayWithType(vx_context context, vx_enum type);

/*!
 * \brief                 Adds the reference to the OpenVX Object in location index of the ObjectArray.
 *
 * This is a vx_reference, which can be later fetched using vxFetObjectArrayItem anc can be used elsewhere in OpenVX.
 *
 * \param [in] arr       The ObjectArray.
 * \param [in] index     The index of the object in the ObjectArray.
 * \param [in] ref       The reference to the Object to be added to the ObjectArray.
 * \return vx_status     VX_SUCCESS if the reference was successfully added; otherise a non-zero error code.
 * \ingroup group_object_array
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetObjectArrayItem(vx_object_array arr, vx_uint32 index, vx_reference ref);

/**
 * @brief Import graph from DOT file.
 *
 * @param graph      The graph to import into.
 * @param dotfile    The DOT file to import from.
 * @param acceptData Accept data nodes in the graph.
 * @return vx_status VX_SUCCESS on success.
 */
VX_API_ENTRY vx_status VX_API_CALL vxImportGraphFromDot(vx_graph graph, vx_char dotfile[], vx_bool acceptData);

/* COREVX Internal Macros */
#define VX_INT_MAX_PARAM_QUEUE_DEPTH 10

/* ENABLED FEATURES IN COREVX ONLY */
#define OPENVX_USE_USER_DATA_OBJECT 1
#define OPENVX_USE_IX 1
#define OPENVX_USE_XML 1
#define OPENVX_USE_S16 1
#define OPENVX_USE_OPENCL_INTEROP 1
#define OPENVX_USE_NN 1
#define OPENVX_USE_NN_16 1
#define OPENVX_USE_PIPELINING 1
#define OPENVX_USE_STREAMING 1

#if defined(__arm__) || defined(__arm64__)
#define OPENVX_USE_TILING 1
#define OPENVX_KHR_TILING 1
#define EXPERIMENTAL_USE_VENUM
#endif /* defined(__arm__) || defined(__arm64__) */

#define OPENVX_CONFORMANCE_NNEF_IMPORT 1
#define OPENVX_CONFORMANCE_NEURAL_NETWORKS 1
#define EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
#define EXPERIMENTAL_USE_DOT
#define EXPERIMENTAL_USE_OPENCL
/* ******************************* */

#endif /* VX_COREVX_EXT_H */