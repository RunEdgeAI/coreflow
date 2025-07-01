/**
 * @file ort_runner.hpp
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

#include <VX/vx.h>
#include <coreml_provider_factory.h>
#include <onnxruntime_c_api.h>
#include <onnxruntime_cxx_api.h>

/**
 * @brief Onnx Runtime Model Runner Object
 */
class OnnxRuntimeRunner
{
public:
    /**
     * @brief Onnx Runtime Model Runner Constructor
     */
    OnnxRuntimeRunner() : model_loaded(false) {}

    /**
     * @brief Onnx Runtime Model Runner Destructor
     */
    virtual ~OnnxRuntimeRunner()
    {
        for (auto& ptr : input_names)
        {
            delete(ptr);
        }
        for (auto& ptr : output_names)
        {
            delete(ptr);
        }
    }

    /**
     * @brief Initialize the kernel (load the model)
     * @param model_path Path to the ONNX model file
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status init(const std::string& model_path)
    {
        try
        {
            Ort::SessionOptions session_options;
            std::string input_name, output_name;
            // Forces single-threaded execution within operators
            session_options.SetIntraOpNumThreads(1);
            session_options.SetGraphOptimizationLevel(ORT_ENABLE_ALL);

#if defined(__linux__) || defined(_WIN32) || defined(UNDER_CE)
            // Register TensorRT Execution Provider
            OrtSessionOptionsAppendExecutionProvider_Tensorrt(
                session_options.operator OrtSessionOptions*(), 0);
#endif
#if defined(__APPLE__)
            // Register CoreML Execution Provider
            OrtSessionOptionsAppendExecutionProvider_CoreML(session_options, 0);
#endif

            // Load the model
            session = std::make_unique<Ort::Session>(getEnv(), model_path.c_str(), session_options);
            model_loaded = true;
            this->model_path = model_path;

            // Cache input/output names and shapes
            for (std::size_t i = 0; i < session->GetInputCount(); ++i)
            {
                input_name = session->GetInputNameAllocated(i, allocator).get();
                input_names.emplace_back(strdup(input_name.c_str()));
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
            for (std::size_t i = 0; i < session->GetOutputCount(); ++i)
            {
                output_name = session->GetOutputNameAllocated(i, allocator).get();
                output_names.emplace_back(strdup(output_name.c_str()));
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

    /**
     * @brief Validate input/output parameters
     * @param inputDims  Input tensor dimensions
     * @param outputDims Output tensor dimensions
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status validate(std::vector<std::vector<size_t>>& inputDims, std::vector<std::vector<size_t>>& outputDims)
    {
        vx_status status = VX_SUCCESS;

        if (!model_loaded)
        {
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            if (inputDims.size() != input_shapes.size() ||
                outputDims.size() != output_shapes.size())
            {
                std::cerr << "Number of input/output tensors do not match the model's input/output shape count!" << std::endl;
                status = VX_FAILURE;
            }
        }

        // Compare input/output VX tensor dimensions with model input/output ORT shapes
        if (VX_SUCCESS == status)
        {
            // Check input tensor dimensions
            for (std::size_t i = 0; i < inputDims.size() && VX_SUCCESS == status; ++i)
            {
                if (inputDims[i].size() != input_shapes[i].size())
                {
                    std::cerr << "Input tensor dimension mismatch for input " << i << "!" << std::endl
                                << "VX: " << inputDims[i].size() << " ORT: " << input_shapes[i].size() << std::endl;
                    status = VX_FAILURE;
                    break;
                }
                for (std::size_t j = 0; j < inputDims[i].size(); ++j)
                {
                    if (inputDims[i][j] != input_shapes[i][j])
                    {
                        std::cerr << "Input tensor dimension mismatch for input " << i << "!" << std::endl
                                    << "VX: " << inputDims[i][j] << " ORT: " << input_shapes[i][j] << std::endl;
                        status = VX_FAILURE;
                        break;
                    }
                }
            }

            // Check output tensor dimensions
            for (std::size_t i = 0; i < outputDims.size() && VX_SUCCESS == status; ++i)
            {
                if (outputDims[i].size() != output_shapes[i].size())
                {
                    std::cerr << "Output tensor dimension mismatch for output " << i << "!" << std::endl
                                << "VX: " << outputDims[i].size() << " ORT: " << output_shapes[i].size() << std::endl;
                    status = VX_FAILURE;
                    break;
                }
                for (std::size_t j = 0; j < outputDims[i].size(); ++j)
                {
                    if (outputDims[i][j] != output_shapes[i][j])
                    {
                        std::cerr << "Output tensor dimension mismatch for output " << i << "!" << std::endl
                                    << "VX: " << outputDims[i][j] << " ORT: " << output_shapes[i][j] << std::endl;
                        status = VX_FAILURE;
                        break;
                    }
                }
            }
        }

        return status;
    }

    /**
     * @brief Run the kernel (execute the model)
     * @param inputTensors  Input tensors
     * @param outputTensors Output tensors
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status run(std::vector<std::pair<float*, vx_size>>& inputTensors, std::vector<std::pair<float*, vx_size>>& outputTensors)
    {
        if (!model_loaded) return VX_FAILURE;

        std::vector<Ort::Value> input_tensors;
        std::vector<Ort::Value> output_tensors;

        // Prepare ORT tensors for inputs
        Ort::MemoryInfo mem_info =
            Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
        for (std::size_t i = 0; i < input_names.size(); ++i)
        {
            input_tensors.emplace_back(Ort::Value::CreateTensor<float>(
                mem_info, inputTensors[i].first, inputTensors[i].second, input_shapes[i].data(), input_shapes[i].size()));
        }

        // Prepare ORT tensors for outputs
        for (std::size_t i = 0; i < output_names.size(); ++i)
        {
            output_tensors.emplace_back(Ort::Value::CreateTensor<float>(
                mem_info, outputTensors[i].first, outputTensors[i].second, output_shapes[i].data(), output_shapes[i].size()));
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

        return VX_SUCCESS;
    }

private:
    bool model_loaded;
    std::string model_path;
    Ort::AllocatorWithDefaultOptions allocator;
    std::unique_ptr<Ort::Session> session;
    std::vector<const char*> input_names;
    std::vector<const char*> output_names;
    std::vector<std::vector<int64_t>> input_shapes;
    std::vector<std::vector<int64_t>> output_shapes;

    /**
     * @brief Get the ONNX runtime environment
     * @return Ort::Env& ONNX runtime environment reference
     */
    static Ort::Env& getEnv()
    {
        static Ort::Env env{ORT_LOGGING_LEVEL_WARNING, "OnnxRuntimeRunner"};
        return env;
    }

    /**
     * @brief pretty prints a shape dimension vector
     * @param v Shape dimension vector
     * @return std::string Pretty printed shape
     */
    std::string print_shape(const std::vector<std::int64_t>& v)
    {
        std::stringstream ss("");
        for (std::size_t i = 0; i < v.size() - 1; i++) ss << v[i] << "x";
        ss << v[v.size() - 1];
        return ss.str();
    }

    /**
     * @brief Pretty print the input/output tensor names and shapes for debugging
     */
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
};
