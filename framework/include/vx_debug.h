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

#ifndef VX_DEBUG_H
#define VX_DEBUG_H

#ifdef __cplusplus
#include <csignal>
#include <string>
#else
#include <signal.h>
#endif /* __cplusplus */

#if defined(__linux__) && !defined(__ANDROID__)
#include <execinfo.h>
#endif

/*!
 * \file
 * \brief The internal debugging implementation.
 *
 * \defgroup group_int_debug Internal Debugging API
 * \ingroup group_internal
 * \brief The Internal Debugging API
 */

/*! \brief These are the bit flags for debugging.
 * \ingroup group_int_debug
 */
enum vx_debug_zone_e {
    VX_ZONE_ERROR       = 0,    /*!< Used for most errors */
    VX_ZONE_WARNING     = 1,    /*!< Used to warning developers of possible issues */
    VX_ZONE_API         = 2,    /*!< Used to trace API calls and return values */
    VX_ZONE_INFO        = 3,    /*!< Used to show run-time processing debug */
    VX_ZONE_DEBUG       = 4,    /*!< Used to show detailed debug information */

    VX_ZONE_PERF        = 5,    /*!< Used to show performance information */
    VX_ZONE_CONTEXT     = 6,    /*!< Used to show only context logs */
    VX_ZONE_OSAL        = 7,    /*!< Used to show only OSAL logs */
    VX_ZONE_REFERENCE   = 8,    /*!< Used to show only reference logs */

    VX_ZONE_ARRAY       = 9,    /*!< Used to show only array logs */
    VX_ZONE_IMAGE       = 10,    /*!< Used to show only image logs */
    VX_ZONE_SCALAR      = 11,   /*!< Used to show only scalar logs */
    VX_ZONE_KERNEL      = 12,   /*!< Used to show only kernel logs */

    VX_ZONE_GRAPH       = 13,   /*!< Used to show only graph logs */
    VX_ZONE_NODE        = 14,   /*!< Used to show only node logs */
    VX_ZONE_PARAMETER   = 15,   /*!< Used to show only parameter logs */
    VX_ZONE_DELAY       = 16,   /*!< Used to show only delay logs */

    VX_ZONE_TARGET      = 17,   /*!< Used to show only target logs */
    VX_ZONE_TENSOR      = 18,   /*!< Used to show only tensor logs */
    VX_ZONE_LOG         = 19,   /*!< Used to show only logs */

    VX_ZONE_MAX         = 32    /*!< The maximum number of zones */
};

#if defined(_WIN32) && !defined(__GNUC__)
#define VX_PRINT(zone, message, ...) \
    do { \
        vx_print(zone, "[%s:%u] " message, __FUNCTION__, __LINE__, __VA_ARGS__); \
    } while (0)
#else
#define VX_PRINT(zone, message, ...) \
    do { \
        vx_print(zone, "[%s:%u] " message, __FUNCTION__, __LINE__, ## __VA_ARGS__); \
    } while (0)
#endif

/*! \def VX_PRINT
 * \brief The OpenVX Debugging Facility.
 * \ingroup group_int_debug
 */

/*! \brief A debugging macro for entering kernels.
 * \ingroup group_int_debug
 */
#define VX_KERNEL_ENTRY(params, num) { \
    vx_uint32 p; \
    VX_PRINT(VX_ZONE_API, "Entered Kernel! Parameters:\n"); \
    for (p = 0; p < num; p++) { \
        VX_PRINT(VX_ZONE_API, "\tparameter[%u]="VX_FMT_REF"\n", p, params[p]); \
    }\
}

/*! \brief A debugging macro for leaving kernels
 * \ingroup group_int_debug
 */
#define VX_KERNEL_RETURN(status) VX_PRINT(VX_ZONE_API, "returning %d\n", status);

#ifndef DEBUG_BREAK
#if defined(_WIN32) && !defined(__CYGWIN__)
#define DEBUG_BREAK()  do{ *((int *) NULL) = 0;exit(3); }while(0)
#else
#define DEBUG_BREAK()  raise(SIGINT) // SIGABRT
#endif
#endif

#if (defined(__linux__) || defined(__QNX__)) || defined(__APPLE__) && !defined(__ANDROID__)

#define VX_BACKTRACE(zone) { \
    void *stack[50]; \
    int i, cnt = backtrace(stack, dimof(stack)); \
    char **symbols = backtrace_symbols(stack, cnt); \
    vx_print(zone, "Backtrace[%d]: (%p)\n", cnt, symbols); \
    for (i = 0; i < cnt; i++) { \
        vx_print(zone, "\t[%p] %s\n", stack[i], (symbols ? symbols[i] : NULL)); \
    } \
    free(symbols);\
}

#elif defined(_WIN32) && !defined(__MINGW32__)

#define VX_BACKTRACE(zone) { \
    PVOID stack[50]; \
    USHORT i, cnt = CaptureStackBackTrace(0, dimof(stack), stack, NULL); \
    for (i = 0; i < cnt; i++) { \
        vx_print(zone, "\t[%p]\n", stack[i]); \
    } \
}

#else

#define VX_BACKTRACE(zone)

#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! \brief Internal Printing Function.
 * \param [in] zone The debug zone from \ref vx_debug_zone_e.
 * \param [in] format The format string to print.
 * \param [in] ... The variable list of arguments.
 * \ingroup group_int_debug
 */
void vx_print(vx_enum zone, const char *format, ...);

/*! \brief Sets a zone bit in the debug mask
 * \param [in] zone The debug zone from \ref vx_debug_zone_e.
 * \ingroup group_int_debug
 */
void vx_set_debug_zone(vx_enum zone);

/*! \brief Clears the zone bit in the mask.
 * \param [in] zone The debug zone from \ref vx_debug_zone_e.
 * \ingroup group_int_debug
 */
void vx_clr_debug_zone(vx_enum zone);

/*! \brief Returns true or false if the zone bit is set or cleared.
 * \param [in] zone The debug zone from \ref vx_debug_zone_e.
 * \ingroup group_int_debug
 */
vx_bool vx_get_debug_zone(vx_enum zone);

/*! \brief Pulls the debug zone mask from the environment variables.
 * \ingroup group_int_debug
 */
void vx_set_debug_zone_from_env(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VX_DEBUG_H */
