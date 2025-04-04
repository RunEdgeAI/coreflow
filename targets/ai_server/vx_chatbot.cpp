/**
 * @file vx_chatbot.cpp
 * @brief OpenVX Interface Into AI Model Server
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <iostream>
#include <string>

#include <VX/vx.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_helper.h>
#include <VX/vx_lib_debug.h>

#include "vx_internal.h"

class VxRemoteModelClient
{
public:
    static constexpr vx_param_description_t kernelParams[] = {
        {VX_INPUT, VX_TYPE_STRING, VX_PARAMETER_STATE_REQUIRED},  // Parameter 0: Input text
        {VX_OUTPUT, VX_TYPE_STRING, VX_PARAMETER_STATE_REQUIRED}, // Parameter 1: Output text
    };

    static vx_status VX_CALLBACK init(vx_node node, const vx_reference parameters[], vx_uint32 num)
    {
        (void)node;
        (void)parameters;
        (void)num;
        return VX_SUCCESS;
    }

    static vx_status VX_CALLBACK validate(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
    {
        (void)node;
        (void)parameters;
        (void)num;
        (void)metas;
        return VX_SUCCESS;
    }

    static vx_status VX_CALLBACK run(vx_node node, const vx_reference *parameters, vx_uint32 num)
    {
        (void)node;
        (void)parameters;
        (void)num;
        return VX_SUCCESS;
    }
};

/**
 * @brief Ai Model Server Chatbot Kernel description structure
 */
vx_kernel_description_t chatbot_kernel = {
    VX_KERNEL_AIS_CHATBOT,    // Unique kernel ID
    "remote.model.chat",      // Kernel name
    VxRemoteModelClient::run, // Kernel execution function
    const_cast<vx_param_description_t *>(VxRemoteModelClient::kernelParams),
    dimof(VxRemoteModelClient::kernelParams), // Number of parameters
    VxRemoteModelClient::validate,            // Kernel validation function
    nullptr,
    nullptr,
    VxRemoteModelClient::init, // Kernel initialization function
    nullptr};