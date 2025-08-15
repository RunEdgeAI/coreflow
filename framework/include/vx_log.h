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

#ifndef VX_LOG_H
#define VX_LOG_H

#include <VX/vx.h>

/*!
 * \file
 * \brief The internal log implementation
 *
 * \defgroup group_int_log Internal Log API
 * \brief The Internal Log API
 * \ingroup group_internal
 */

/**
 * @brief Logger utility class that wraps spdlog functionality
 * @ingroup group_int_log
 */
namespace corevx {

class Logger
{
public:
    /**
     * @brief Registers a callback facility to the OpenVX implementation to receive error logs.
     *
     * @param [in] context The overall context to OpenVX.
     * @param [in] callback The callback function. If NULL, the previous callback is removed.
     * @param [in] reentrant If reentrancy flag is <tt>\ref vx_true_e</tt>, then the callback may be
     * entered from multiple simultaneous tasks or threads (if the host OS supports this).
     * @ingroup group_int_log
     */
    static void registerLogCallback(vx_context context, vx_log_callback_f callback, vx_bool reentrant);

    /**
     * @brief Adds a line to the log.
     *
     * @param [in] ref The reference to add the log entry against. Some valid value must be
     * provided.
     * @param [in] status The status code. <tt>\ref VX_SUCCESS</tt> status entries are ignored and
     * not added.
     * @param [in] message The human readable message to add to the log.
     * @param [in] ap a list of variable arguments to the message.
     * @note Messages may not exceed <tt>\ref VX_MAX_LOG_MESSAGE_LEN</tt> bytes and will be
     * truncated in the log if they exceed this limit.
     * @ingroup group_int_log
     */
    static void addLogEntry(vx_reference ref, vx_status status, const char *message, va_list ap);
};

} // namespace corevx

#endif /* VX_LOG_H */