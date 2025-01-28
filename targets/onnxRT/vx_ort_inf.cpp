/**
 * @file vx_ort_inf.cpp
 * @brief OpenVX Interface Into ORT
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

#include "ort_runner.hpp"
#include "vx_internal.h"

class VxOrtRunner
{
public:
    static constexpr vx_param_description_t kernelParams[3] = {
        {VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED},         // Parameter 0: Model path
        {VX_INPUT, VX_TYPE_OBJECT_ARRAY, VX_PARAMETER_STATE_REQUIRED},  // Parameter 1: Input tensors
        {VX_OUTPUT, VX_TYPE_OBJECT_ARRAY, VX_PARAMETER_STATE_REQUIRED}  // Parameter 2: Output tensors
    };

    // Initialization function
    static vx_status VX_CALLBACK ortInitWrapper(vx_node node, const vx_reference parameters[], vx_uint32 num)
    {
        vx_status status = VX_SUCCESS;
        // Create an instance of PyTorchRunnerKernel
        auto* kernel = new OnnxRuntimeRunner();
        char* modelPathCharArray = nullptr;

        if (nullptr == node ||
            nullptr == parameters ||
            num != dimof(kernelParams))
        {
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Get the model path from the first parameter
            vx_array array = (vx_array)parameters[0];
            status = readStringFromVxArray(array, (void*)modelPathCharArray);

            if (VX_SUCCESS == status)
            {
                std::string modelPath(modelPathCharArray);
                // Initialize the kernel with the model path
                status |= kernel->init(modelPath);
            }
        }

        if (VX_SUCCESS != status)
        {
            delete kernel;
        }

        if (VX_SUCCESS == status)
        {
            // Store the kernel instance in the node's local data
            status |= vxSetNodeAttribute(node, VX_NODE_ATTRIBUTE_LOCAL_DATA_PTR, &kernel, sizeof(kernel));
        }


        return status;
    }

    // Validation function
    static vx_status VX_CALLBACK ortValidateWrapper(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
    {
        vx_status status = VX_SUCCESS;
        OnnxRuntimeRunner* kernel = nullptr;

        if (nullptr != node ||
            nullptr != parameters ||
            num != dimof(kernelParams) ||
            nullptr != metas)
        {
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Retrieve the kernel instance from the node's local data
            status = vxQueryNode(node, VX_NODE_ATTRIBUTE_LOCAL_DATA_PTR, &kernel, sizeof(kernel));

            if (VX_SUCCESS != status ||
                !kernel)
            {
                std::cerr << "Error: Kernel instance is null during validation!" << std::endl;
                status = VX_FAILURE;
            }
        }

        if (VX_SUCCESS == status)
        {
            vx_object_array inputObjArr = reinterpret_cast<vx_object_array>(parameters[1]);
            vx_object_array outputObjArr = reinterpret_cast<vx_object_array>(parameters[2]);
            vx_size numItems = 0;
            std::vector<std::vector<vx_size>> inputDims;
            std::vector<std::vector<vx_size>> outputDims;

            status = vxQueryObjectArray(inputObjArr, VX_OBJECT_ARRAY_NUMITEMS, &numItems, sizeof(numItems));
            status |= vxSetMetaFormatAttribute(metas[1], VX_OBJECT_ARRAY_NUMITEMS, &numItems, sizeof(numItems));
            // Iterate through input object array
            for (vx_uint32 i = 0; i < numItems; ++i)
            {
                // Extract tensor dims
                vx_tensor tensor = reinterpret_cast<vx_tensor>(vxGetObjectArrayItem(inputObjArr, i));
                std::vector<vx_size> dims;
                dims.reserve(VX_MAX_TENSOR_DIMENSIONS);
                status |= vxQueryTensor(tensor, VX_TENSOR_DIMS, dims.data(), sizeof(vx_size) * dims.size());
                inputDims.emplace_back(dims);
            }

            status |= vxQueryObjectArray(outputObjArr, VX_OBJECT_ARRAY_NUMITEMS, &numItems, sizeof(numItems));
            status |= vxSetMetaFormatAttribute(metas[2], VX_OBJECT_ARRAY_NUMITEMS, &numItems, sizeof(numItems));
            // Iterate through input object array
            for (vx_uint32 i = 0; i < numItems; ++i)
            {
                // Extract tensor dims
                vx_tensor tensor = reinterpret_cast<vx_tensor>(vxGetObjectArrayItem(outputObjArr, i));
                std::vector<vx_size> dims;
                dims.reserve(VX_MAX_TENSOR_DIMENSIONS);
                status |= vxQueryTensor(tensor, VX_TENSOR_DIMS, dims.data(), sizeof(vx_size) * dims.size());
                outputDims.emplace_back(dims);
            }

            if (VX_SUCCESS == status)
            {
                // Call the validate member function
                status = kernel->validate(inputDims, outputDims);
            }
        }

        return status;
    }

    // Execution function
    static vx_status VX_CALLBACK ortRunWrapper(vx_node node, const vx_reference* parameters, vx_uint32 num)
    {
        vx_status status = VX_SUCCESS;
        OnnxRuntimeRunner* kernel = nullptr;

        if (num < dimof(kernelParams))
        {
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Retrieve the kernel instance from the node's local data
            vxQueryNode(node, VX_NODE_ATTRIBUTE_LOCAL_DATA_PTR, &kernel, sizeof(kernel));

            if (!kernel)
            {
                std::cerr << "Error: Kernel instance is null during execution!" << std::endl;
                status = VX_FAILURE;
            }

        }

        // Call the run member function
        return kernel->run(parameters, num);
    }

    // Deinitialization function
    static vx_status VX_CALLBACK ortDeinitWrapper(vx_node node, const vx_reference* parameters, vx_uint32 num)
    {
        vx_status status = VX_SUCCESS;
        OnnxRuntimeRunner* kernel = nullptr;

        if (num != dimof(parameters))
        {
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Retrieve the kernel instance from the node's local data
            vxQueryNode(node, VX_NODE_ATTRIBUTE_LOCAL_DATA_PTR, &kernel, sizeof(kernel));

            if (!kernel)
            {
                std::cerr << "Error: Kernel instance is null during deinitialization!" << std::endl;
                status = VX_FAILURE;
            }
        }

        if (VX_SUCCESS == status)
        {
            // Delete the kernel instance
            delete kernel;
        }

        return status;
    }
private:
    static vx_status readStringFromVxArray(vx_array array, void* ptr)
    {
        vx_status status = VX_SUCCESS;
        vx_size num_items = 0u, stride = 0u;
        vx_map_id map_id;

        status = vxQueryArray(array, VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items, sizeof(num_items));
        status |= vxQueryArray(array, VX_ARRAY_ATTRIBUTE_ITEMSIZE, &stride, sizeof(num_items));

        if (VX_SUCCESS == status)
        {
            status = vxMapArrayRange(array, 0, num_items, &map_id, &stride, &ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
            status |= vxUnmapArrayRange(array, map_id);
        }

        return status;
    }
};

// Kernel description structure
vx_kernel_description_t onnxrt_cpu_inf_kernel =
{
    VX_KERNEL_ORT_CPU_INF,                  // Unique kernel ID
    "ort.cpu.runner",                       // Kernel name
    VxOrtRunner::ortRunWrapper,             // Kernel execution function
    const_cast<vx_param_description_t*>(VxOrtRunner::kernelParams),
    dimof(VxOrtRunner::kernelParams),       // Number of parameters
    VxOrtRunner::ortValidateWrapper,        // Kernel validation function
    nullptr,
    nullptr,
    VxOrtRunner::ortInitWrapper,            // Kernel initialization function
    VxOrtRunner::ortDeinitWrapper           // Kernel deinitialization function
};