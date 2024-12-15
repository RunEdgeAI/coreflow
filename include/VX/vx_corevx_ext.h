/**
 * @file vx_corevx_ext.h
 * @brief Extensions enabled for corevs
 * @version 0.1
 * @date 2024-12-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef VX_COREVX_EXT_H
#define VX_COREVX_EXT_H

/* ENABLED FEATURES IN COREVX ONLY */
#define OPENVX_USE_USER_DATA_OBJECT
#define OPENVX_USE_IX
#define OPENVX_USE_XML
#define OPENVX_USE_S16
#define OPENVX_USE_OPENCL_INTEROP
#define OPENVX_USE_NN
#if defined(__arm__) || defined(__arm64__)
#define OPENVX_USE_TILING
#define OPENVX_KHR_TILING
#define EXPERIMENTAL_USE_VENUM
#endif /* defined(__arm__) || defined(__arm64__) */
#define OPENVX_CONFORMANCE_NNEF_IMPORT
#define OPENVX_CONFORMANCE_NEURAL_NETWORKS
#define EXPERIMENTAL_PLATFORM_SUPPORTS_16_FLOAT
#define EXPERIMENTAL_USE_DOT
#define EXPERIMENTAL_USE_OPENCL
/* ******************************* */

#endif /* VX_COREVX_EXT_H */