/**
 * @file onnx_runner.cpp
 * @brief ONNX Runtime Model Runner
 * @version 0.2
 * @date 2025-01-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <core/session/onnxruntime_cxx_api.h>
#include <VX/vx.h>

class OnnxRuntimeRunner
{
public:
    OnnxRuntimeRunner() : model_loaded(false) {}

    // Initialize the kernel (load the model)
    vx_status init(const std::string& model_path)
    {
        try
        {
            Ort::SessionOptions session_options;
            session_options.SetGraphOptimizationLevel(ORT_ENABLE_ALL);

            // Load the model
            session = std::make_unique<Ort::Session>(env, model_path.c_str(), session_options);
            model_loaded = true;
            this->model_path = model_path;

            // Cache input/output names and shapes
            for (std::size_t i = 0; i < session->GetInputCount(); i++)
            {
                input_names.emplace_back(session->GetInputNameAllocated(i, allocator).get());
                auto shape = session->GetInputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
                // some models might have negative shape values to indicate dynamic shape, e.g., for variable batch size.
                for (auto& s : shape)
                {
                    if (s < 0)
                    {
                        s = 1;
                    }
                }
                input_shapes.emplace_back(shape);
            }
            for (std::size_t i = 0; i < session->GetOutputCount(); i++)
            {
                output_names.emplace_back(session->GetOutputNameAllocated(i, allocator).get());
                output_shapes.emplace_back(session->GetOutputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape());
            }

            return VX_SUCCESS;
        }
        catch (const Ort::Exception& e)
        {
            std::cerr << "Error loading model or initializing IO: " << e.what() << std::endl;
            return VX_FAILURE;
        }
    }

    // Validate input/output parameters
    vx_status validate(std::vector<std::vector<size_t>>& inputDims, std::vector<std::vector<size_t>>& outputDims)
    {
        // if (num < 3) return VX_FAILURE;

        // vx_tensor input = (vx_tensor)parameters[1];
        // vx_tensor output = (vx_tensor)parameters[2];

        // // Validate tensor dimensions
        // vx_size input_dims[4];
        // vx_size output_dims[4];
        // vxQueryTensor(input, VX_TENSOR_DIMS, input_dims, sizeof(input_dims));
        // vxQueryTensor(output, VX_TENSOR_DIMS, output_dims, sizeof(output_dims));

        // // Compare dimensions with model input/output shapes
        // if (input_shapes[0] != -1 && input_dims[0] != static_cast<vx_size>(input_shapes[0]))
        // {
        //     std::cerr << "Input tensor dimensions do not match the model's input shape!" << std::endl;
        //     return VX_FAILURE;
        // }
        // if (output_shapes[0] != -1 && output_dims[0] != static_cast<vx_size>(output_shapes[0]))
        // {
        //     std::cerr << "Output tensor dimensions do not match the model's output shape!" << std::endl;
        //     return VX_FAILURE;
        // }

        return VX_SUCCESS;
    }

    // Run the kernel (execute the model)
    vx_status run(const vx_reference* parameters, __attribute__((unused)) vx_uint32 num)
    {
        if (!model_loaded) return VX_FAILURE;

        std::vector<Ort::Value> input_tensors;
        std::vector<Ort::Value> output_tensors;
        // Convert OpenVX tensors to std::vector<float>
        std::vector<float> input_data;
        std::vector<float> output_data;
        if (!extract_tensor_data((vx_tensor)parameters[1], input_data)) return VX_FAILURE;
        if (!extract_tensor_data((vx_tensor)parameters[2], output_data)) return VX_FAILURE;

        // Prepare ORT tensors for inputs
        Ort::MemoryInfo mem_info =
        Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
        for (std::size_t i = 0; i < input_names.size(); ++i)
        {
            input_tensors.emplace_back(Ort::Value::CreateTensor<float>(
                mem_info, input_data.data(), input_data.size(), input_shapes[i].data(), input_shapes[i].size()));
        }

        // Prepare ORT tensors for outputs
        for (std::size_t i = 0; i < output_names.size(); ++i)
        {
            output_tensors.emplace_back(Ort::Value::CreateTensor<float>(
                mem_info, output_data.data(), output_data.size(), output_shapes[i].data(), output_shapes[i].size()));
        }

        // Run inference
        try
        {
            session->Run(Ort::RunOptions{nullptr},
                input_names.data(), input_tensors.data(), input_names.size(),
                output_names.data(), output_tensors.data(), output_names.size());
        }
        catch (const Ort::Exception& e)
        {
            std::cerr << "Error during inference: " << e.what() << std::endl;
            return VX_FAILURE;
        }

        // Write the output back to the OpenVX tensor
        if (!populate_tensor_data((vx_tensor)parameters[2], output_data)) return VX_FAILURE;

        return VX_SUCCESS;
    }

    void debugPrint()
    {
        // print name/shape of inputs
        std::cout << "Input Node Name/Shape (" << input_names.size() << "):" << std::endl;
        for (std::size_t i = 0; i < session->GetInputCount(); i++)
        {
            auto input_shape = session->GetInputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
            std::cout << "\t" << input_names.at(i) << " : " << print_shape(input_shape) << std::endl;
        }

        // print name/shape of outputs
        std::cout << "Output Node Name/Shape (" << output_names.size() << "):" << std::endl;
        for (std::size_t i = 0; i < session->GetOutputCount(); i++)
        {
            auto output_shape = session->GetOutputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
            std::cout << "\t" << output_names.at(i) << " : " << print_shape(output_shape) << std::endl;
        }
    }

private:
    bool model_loaded;
    std::string model_path;
    Ort::Env env{ORT_LOGGING_LEVEL_WARNING, "OnnxRuntimeRunner"};
    Ort::AllocatorWithDefaultOptions allocator;
    std::unique_ptr<Ort::Session> session;
    std::vector<const char*> input_names;
    std::vector<const char*> output_names;
    std::vector<std::vector<int64_t>> input_shapes;
    std::vector<std::vector<int64_t>> output_shapes;

    // pretty prints a shape dimension vector
    std::string print_shape(const std::vector<std::int64_t>& v)
    {
        std::stringstream ss("");
        for (std::size_t i = 0; i < v.size() - 1; i++) ss << v[i] << "x";
        ss << v[v.size() - 1];
        return ss.str();
    }

    // Utility function to extract tensor data from OpenVX tensor to std::vector
    bool extract_tensor_data(vx_tensor vx_tensor, std::vector<float>& data)
    {
        vx_size dims[4];
        vxQueryTensor(vx_tensor, VX_TENSOR_DIMS, dims, sizeof(dims));

        size_t num_elements = dims[0] * dims[1] * dims[2] * dims[3];
        data.resize(num_elements);

        void* data_ptr;
        // vxAccessTensor(vx_tensor, 0, nullptr, nullptr, nullptr, &data_ptr, VX_READ_ONLY);
        memcpy(data.data(), data_ptr, num_elements * sizeof(float));
        // vxCommitTensor(vx_tensor, nullptr, nullptr, data_ptr);

        return true;
    }

    // Utility function to populate data into OpenVX tensor from std::vector
    bool populate_tensor_data(vx_tensor vx_tensor, const std::vector<float>& data)
    {
        vx_size dims[4];
        vxQueryTensor(vx_tensor, VX_TENSOR_DIMS, dims, sizeof(dims));

        size_t num_elements = dims[0] * dims[1] * dims[2] * dims[3];

        void* data_ptr = nullptr;
        // vxAccessTensor(vx_tensor, 0, nullptr, nullptr, nullptr, &data_ptr, VX_WRITE_ONLY);
        memcpy(data_ptr, data.data(), num_elements * sizeof(float));
        // vxCommitTensor(vx_tensor, nullptr, nullptr, data_ptr);

        return true;
    }
};
