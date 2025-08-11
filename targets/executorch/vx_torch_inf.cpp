/**
 * @file vx_torch_inf.cpp
 * @brief OpenVX Interface Into Executorch
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025 EdgeAI, LLC. All rights reserved.
 *
 */
#include <string>
#include <vector>

#include <VX/vx.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_helper.h>
#include <VX/vx_lib_debug.h>

#include "torch.hpp"
#include "vx_internal.h"

// Create an instance of Executorch runner
static const std::shared_ptr<TorchRunner> kernel = std::make_shared<TorchRunner>();

class VxTorchRunner
{
public:
    static constexpr vx_param_description_t kernelParams[] = {
        {VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED},        // Parameter 0: Model path
        {VX_INPUT, VX_TYPE_OBJECT_ARRAY, VX_PARAMETER_STATE_REQUIRED}, // Parameter 1: Input tensors
        {VX_OUTPUT, VX_TYPE_OBJECT_ARRAY, VX_PARAMETER_STATE_REQUIRED} // Parameter 2: Output tensors
    };

    // Initialization function
    static vx_status VX_CALLBACK torchInitWrapper(vx_node node, const vx_reference parameters[], vx_uint32 num)
    {
        vx_status status = VX_SUCCESS;
        std::string modelPath;
        // Get the tensor pointers, total size of each, and cache them in a vector of pairs
        std::vector<std::pair<float *, vx_size>> inputTensors;
        std::vector<std::pair<float *, vx_size>> outputTensors;
        // Get the tensor dimensions
        std::vector<std::vector<vx_size>> inputDims;
        std::vector<std::vector<vx_size>> outputDims;

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
            status = readStringFromVxArray(array, modelPath);

            if (VX_SUCCESS == status)
            {
                VX_PRINT(VX_ZONE_INFO, "Reading from model path: %s\n", modelPath.c_str());
                // Initialize the kernel with the model path
                status |= kernel->init(modelPath);
            }
        }

        if (VX_SUCCESS == status)
        {
            // Process input tensors
            status = processTensors((vx_object_array)parameters[1], inputTensors);
            // Process output tensors if input processing was successful
            status |= processTensors((vx_object_array)parameters[2], outputTensors);
        }

        if (VX_SUCCESS == status)
        {
            // Get the input tensor dimensions from the tensors
            status = processTensorDims(reinterpret_cast<vx_object_array>(parameters[1]), inputDims);
            // Get the output tensor dimensions from the tensors
            status = processTensorDims(reinterpret_cast<vx_object_array>(parameters[2]), outputDims);
        }

        if (VX_SUCCESS == status)
        {
            // Bind the input and output tensors
            status = kernel->allocate(inputTensors, inputDims, outputTensors, outputDims);
        }

        return status;
    }

    // Validation function
    static vx_status VX_CALLBACK torchValidateWrapper(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
    {
        vx_status status = VX_SUCCESS;

        if (nullptr == node ||
            nullptr == parameters ||
            num != dimof(kernelParams) ||
            nullptr == metas)
        {
            std::cerr << "Error: Invalid parameters during validation!" << std::endl;
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Retrieve the kernel instance from the node's local data
            if (!kernel)
            {
                std::cerr << "Error: Kernel instance is null during validation!" << std::endl;
                status = VX_FAILURE;
            }
        }

        if (VX_SUCCESS == status)
        {
            vx_object_array outputObjArr = reinterpret_cast<vx_object_array>(parameters[2]);
            vx_size numItems = 0;
            vx_enum itemType = VX_TYPE_TENSOR;

            status = vxQueryObjectArray(outputObjArr, VX_OBJECT_ARRAY_NUMITEMS, &numItems, sizeof(numItems));
            status |= vxSetMetaFormatAttribute(metas[2], VX_OBJECT_ARRAY_NUMITEMS, &numItems, sizeof(numItems));
            status |= vxSetMetaFormatAttribute(metas[2], VX_OBJECT_ARRAY_ITEMTYPE, &itemType, sizeof(vx_enum));
        }

        return status;
    }

    // Execution function
    static vx_status VX_CALLBACK torchRunWrapper(vx_node node, const vx_reference *parameters, vx_uint32 num)
    {
        vx_status status = VX_SUCCESS;

        if (nullptr == node ||
            nullptr == parameters ||
            num != dimof(kernelParams))
        {
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Retrieve the kernel instance from the node's local data
            if (!kernel)
            {
                std::cerr << "Error: Kernel instance is null during execution!" << std::endl;
                status = VX_FAILURE;
            }
        }

        if (VX_SUCCESS == status)
        {
            // Call the run member function
            status = kernel->run();
        }

        return status;
    }

private:
    /**
     * @brief Helper function to read a string from a VX char array
     *
     * @param[in]  array  openvx char array to read from
     * @param[out] str    Output string containing the read data
     * @return vx_status  VX_SUCCESS on success, otherwise an error code
     */
    static vx_status readStringFromVxArray(vx_array array, std::string &str)
    {
        vx_status status = VX_SUCCESS;
        vx_size num_items = 0u, stride = 0u;
        vx_map_id map_id = 0;
        void *ptr = nullptr;

        status = vxQueryArray(array, VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items, sizeof(num_items));
        status |= vxMapArrayRange(array, 0, num_items, &map_id, &stride, &ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);

        if (VX_SUCCESS == status)
        {
            str = std::string(static_cast<char *>(ptr));
            status |= vxUnmapArrayRange(array, map_id);
        }

        return status;
    }

    /**
     * @brief Helper function to process tensor dimensions from an object array
     *
     * @param[in]  objArr  Object array containing tensors
     * @param[out] dims    Vector of vectors containing tensor dimensions
     * @return vx_status   VX_SUCCESS on success, otherwise an error code
     */
    static vx_status processTensorDims(vx_object_array objArr, std::vector<std::vector<vx_size>> &dims)
    {
        vx_status status = VX_SUCCESS;
        vx_size numItems = 0, numDims = 0;
        std::vector<vx_size> tensorDims;

        status = vxQueryObjectArray(objArr, VX_OBJECT_ARRAY_NUMITEMS, &numItems, sizeof(numItems));

        for (vx_uint32 i = 0; i < numItems && status == VX_SUCCESS; ++i)
        {
            vx_tensor tensor = reinterpret_cast<vx_tensor>(vxGetObjectArrayItem(objArr, i));
            status |= vxQueryTensor(tensor, VX_TENSOR_NUMBER_OF_DIMS, &numDims, sizeof(numDims));
            tensorDims.resize(numDims);
            status |= vxQueryTensor(tensor, VX_TENSOR_DIMS, tensorDims.data(), sizeof(vx_size) * tensorDims.size());

            if (VX_SUCCESS != status)
            {
                std::cerr << "Error: Unable to query tensor in " << __func__ << " " << status << std::endl;
                break;
            }
            dims.push_back(tensorDims);
        }
        return status;
    }

    /**
     * @brief Helper function to process tensors from an object array
     *
     * @param[in]  objArr  Object array containing tensors
     * @param[out] tensors Vector of pairs containing tensor data and size
     * @return vx_status   VX_SUCCESS on success, otherwise an error code
     */
    static vx_status processTensors(vx_object_array objArr, std::vector<std::pair<float *, size_t>> &tensors)
    {
        vx_status status = VX_SUCCESS;
        vx_size numItems = 0;
        vxQueryObjectArray(objArr, VX_OBJECT_ARRAY_NUMITEMS, &numItems, sizeof(numItems));

        for (vx_uint32 i = 0; i < numItems && status == VX_SUCCESS; ++i)
        {
            vx_tensor tensor = (vx_tensor)vxGetObjectArrayItem(objArr, i);
            vx_size dims[VX_MAX_TENSOR_DIMENSIONS];
            vx_size stride[VX_MAX_TENSOR_DIMENSIONS];
            vx_size viewStart[VX_MAX_TENSOR_DIMENSIONS] = {0};
            void *ptr = nullptr;
            vx_size numDims = 0, size = 0;
            vx_map_id map_id = 0;

            status |= vxQueryTensor(tensor, VX_TENSOR_NUMBER_OF_DIMS, &numDims, sizeof(numDims));
            status |= vxQueryTensor(tensor, VX_TENSOR_DIMS, dims, sizeof(dims));
            status |= vxQueryTensor(tensor, VX_TENSOR_STRIDE, stride, sizeof(stride));
            status |= vxQueryTensor(tensor, VX_TENSOR_TOTAL_SIZE, &size, sizeof(size));
            status |= vxMapTensorPatch(tensor, numDims, viewStart, dims, &map_id, stride, &ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

            if (VX_SUCCESS != status)
            {
                std::cerr << "Error: Unable to prep tensor in " << __func__ << ", status: " << status << std::endl;
                break;
            }

            tensors.emplace_back((float *)ptr, size);
            status |= vxUnmapTensorPatch(tensor, map_id);
        }
        return status;
    }
};

/**
 * @brief torch CPU Inference Kernel description structure
 */
vx_kernel_description_t torch_cpu_inf_kernel =
    {
        VX_KERNEL_TORCH_CPU_INF,        // Unique kernel ID
        "torch.cpu.runner",             // Kernel name
        VxTorchRunner::torchRunWrapper, // Kernel execution function
        const_cast<vx_param_description_t *>(VxTorchRunner::kernelParams),
        dimof(VxTorchRunner::kernelParams),  // Number of parameters
        VxTorchRunner::torchValidateWrapper, // Kernel validation function
        nullptr,
        nullptr,
        VxTorchRunner::torchInitWrapper, // Kernel initialization function
        nullptr};