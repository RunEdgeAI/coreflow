/**
 * @file vx_chatbot.cpp
 * @brief OpenVX Interface Into AI Model Server
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <string>
#include <unordered_map>

#include <VX/vx.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_helper.h>
#include <VX/vx_lib_debug.h>

#include "chatbot.hpp"
#include "vx_internal.h"

// Create an instance of ORT runner
static const std::shared_ptr<RemoteModelClient> kernel = std::make_shared<RemoteModelClient>();

static std::unordered_map<std::string, const std::string> api_map = {
    {"chat", "/v1/chat/completions"},
};

class VxRemoteModelClient
{
private:
    static vx_status store_vx_string_to_array(vx_array arr, const vx_string &in)
    {
        vx_status status = vxTruncateArray(arr, 0); // clear existing contents
        if (status != VX_SUCCESS)
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to clear existing contents out of string\n");
            return status;
        }

        // Get array capacity
        vx_size capacity;
        status = vxQueryArray(arr, VX_ARRAY_CAPACITY, &capacity, sizeof(capacity));
        if (status != VX_SUCCESS)
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to query array capacity\n");
            return status;
        }

        // Truncate input string to array capacity
        vx_string truncated = in.substr(0, capacity);

        // Add the truncated string to array
        status = vxAddArrayItems(arr, truncated.length(), truncated.c_str(), sizeof(char));
        if (status != VX_SUCCESS)
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to add string to array\n");
            return status;
        }

        return VX_SUCCESS;
    }

    static vx_status load_vx_string_from_array(vx_array arr, vx_string &out)
    {
        vx_size size = 0;
        vx_status status = vxQueryArray(arr, VX_ARRAY_ATTRIBUTE_NUMITEMS, &size, sizeof(size));
        if (status != VX_SUCCESS || size == 0)
            return VX_FAILURE;

        out.resize(size); // allocate space directly in std::string
        status = vxCopyArrayRange(arr, 0, size, sizeof(char), out.data(), VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
        return status;
    }

public:
    static constexpr vx_param_description_t kernelParams[] = {
        {VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED},  // Parameter 0: Input text
        {VX_OUTPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED}, // Parameter 1: Output text
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
        vx_status status = VX_SUCCESS;

        if (nullptr == node || nullptr == parameters || num != dimof(kernelParams) ||
            nullptr == metas)
        {
            VX_PRINT(VX_ZONE_ERROR, "Error: Invalid parameters during validation!\n");
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Retrieve the kernel instance from the node's local data
            if (!kernel)
            {
                VX_PRINT(VX_ZONE_ERROR, "Error: Kernel instance is null during validation!\n");
                status = VX_FAILURE;
            }
        }

        if (VX_SUCCESS == status)
        {
            vx_array outputArr = reinterpret_cast<vx_array>(parameters[1]);
            vx_size capacity;
            vx_enum itemType;

            status = vxQueryArray(outputArr, VX_ARRAY_CAPACITY, &capacity, sizeof(capacity));
            status |= vxQueryArray(outputArr, VX_ARRAY_ITEMTYPE, &itemType, sizeof(itemType));
            status |=
                vxSetMetaFormatAttribute(metas[1], VX_ARRAY_CAPACITY, &capacity, sizeof(capacity));
            status |=
                vxSetMetaFormatAttribute(metas[1], VX_ARRAY_ITEMTYPE, &itemType, sizeof(itemType));
        }

        if (VX_SUCCESS != status)
        {
            VX_PRINT(VX_ZONE_ERROR, "Failed to validate kernel\n");
        }
        return status;
    }

    static vx_status VX_CALLBACK run(vx_node node, const vx_reference *parameters, vx_uint32 num)
    {
        (void)node;
        (void)parameters;
        (void)num;
        vx_status status = VX_SUCCESS;
        vx_string input_text, output_text;

        status = load_vx_string_from_array((vx_array)parameters[0], input_text);
        // std::cout << "[input]: " << input_text << std::endl;
        status |= kernel->AiServerQuery(input_text,        // Input text
                                        output_text,       // Output text
                                        api_map["chat"]);  // API path
        // std::cout << "[output]: " << output_text << std::endl;
        status |= store_vx_string_to_array((vx_array)parameters[1], output_text);

        return status;
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