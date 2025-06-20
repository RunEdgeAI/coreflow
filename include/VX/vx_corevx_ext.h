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

#ifdef __cplusplus
#include <string>

/*! \brief A character array (string) type.
 * \note This is a C++ string type. It is not a C string.
 * \ingroup group_basic_features
 */
using vx_string = std::string;
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
 * \ingroup group_kernel
 */
enum vx_kernel_ext_e
{
    /*!
     * \brief The ONNX Runtime CPU Inference kernel.
     */
    VX_KERNEL_ORT_CPU_INF = VX_KERNEL_BASE(VX_ID_EDGE_AI, VX_LIBRARY_KHR_BASE) + 0x1,
    /*!
     * \brief The AI Model Server Chatbot kernel.
     */
    VX_KERNEL_AIS_CHATBOT = VX_KERNEL_BASE(VX_ID_EDGE_AI, VX_LIBRARY_KHR_BASE) + 0x2,
    /*!
     * \brief The LiteRT CPU Inference kernel.
     */
    VX_KERNEL_LITERT_CPU_INF = VX_KERNEL_BASE(VX_ID_EDGE_AI, VX_LIBRARY_KHR_BASE) + 0x3,
    /*!
     * \brief The Torch CPU Inference kernel.
     */
    VX_KERNEL_TORCH_CPU_INF = VX_KERNEL_BASE(VX_ID_EDGE_AI, VX_LIBRARY_KHR_BASE) + 0x4,
};

/*! \brief addtitional tensor attributes.
 * \ingroup group_object_tensor
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