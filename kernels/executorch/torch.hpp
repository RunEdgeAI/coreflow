/**
 * @file torch.hpp
 * @brief
 * @version 0.1
 * @date 2025-04-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <iostream>
#include <fstream>
#include <memory>

#include <executorch/extension/module/module.h>
#include <executorch/extension/tensor/tensor.h>
#include <executorch/devtools/etdump/etdump_flatcc.h>

using namespace ::executorch::extension;

/**
 * @brief Class to run TFLite models
 *
 */
class TorchRunner
{
public:
    /**
     * @brief TorchRunner Constructor
     */
    TorchRunner() : _modelLoaded(false), _traceEnabled(false), _module(nullptr) {};

    /**
     * @brief Initialize the TFLite interpreter (load the model)
     * @param filename Path to the ONNX model file
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status init(std::string &filename)
    {
        vx_status status = VX_SUCCESS;

        // Initialize the module
        if (!filename.empty() && !_modelLoaded)
        {
            // Load model
            _module = std::make_unique<Module>(filename, Module::LoadMode::MmapUseMlock, std::make_unique<executorch::etdump::ETDumpGen>());
            const auto error = _module->load(executorch::runtime::Program::Verification::InternalConsistency);

            if (!_module->is_loaded())
            {
                std::cerr << "Failed to load module: " << filename << std::endl;
                status = VX_FAILURE;
            }

            if (VX_SUCCESS == status)
            {
                // Set the model loaded flag
                _modelLoaded = true;
            }
        }

        return status;
    }

    /**
     * @brief Allocate memory for input and output tensors
     * @param inputTensors  Input tensors
     * @param inputDims  Input tensor dimensions
     * @param outputTensors Output tensors
     * @param outputDims Output tensor dimensions
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status allocate(
        std::vector<std::pair<float *, vx_size>> &inputTensors, std::vector<std::vector<size_t>> &inputDims,
        std::vector<std::pair<float *, vx_size>> &outputTensors, std::vector<std::vector<size_t>> &outputDims)
    {
        vx_status status = VX_SUCCESS;

        // Check if the model is loaded
        if (!_modelLoaded)
        {
            std::cerr << "Model not loaded" << std::endl;
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Allocate tensor pointers and bind with pre-allocated memory
            for (std::size_t i = 0; i < inputTensors.size(); ++i)
            {
                std::vector<executorch::aten::SizesType> dims;
                std::transform(inputDims[i].begin(), inputDims[i].end(), std::back_inserter(dims),
                               [](size_t n)
                               { return static_cast<executorch::aten::SizesType>(n); });
                auto tensor = make_tensor_ptr(dims, inputTensors[i].first);
                // Bind input tensor to the module
                _module->set_input(tensor, i);
            }

            for (std::size_t i = 0; i < outputTensors.size(); ++i)
            {
                std::vector<executorch::aten::SizesType> dims;
                std::transform(outputDims[i].begin(), outputDims[i].end(), std::back_inserter(dims),
                               [](size_t n)
                               { return static_cast<executorch::aten::SizesType>(n); });
                auto tensor = make_tensor_ptr(dims, outputTensors[i].first);
                // Bind output tensor to the module
                _module->set_output(tensor, i);
            }
        }

        return status;
    }

    /**
     * @brief Run the kernel (execute the model)
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status run()
    {
        vx_status status = VX_SUCCESS;

        // Check if the model is loaded
        if (!_modelLoaded)
        {
            std::cerr << "Model not loaded" << std::endl;
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            try
            {
                // Run inference
                _module->load_forward();
                const auto result = _module->forward();

                // Check the result
                if (!result.ok())
                {
                    std::cerr << "Failed to run inference" << std::endl;
                    if (_traceEnabled)
                    {
                        dumpTrace();
                    }
                    status = VX_FAILURE;
                }
            }
            catch (...)
            {
                // std::cerr << "Failed to set trace enabled: " << e.what() << std::endl;
                status = VX_FAILURE;
            }
        }

        return status;
    }

private:
    bool _modelLoaded;
    bool _traceEnabled;
    std::unique_ptr<Module> _module;

    /**
     * @brief Dump the profile trace data to a file
     */
    void dumpTrace()
    {
        if (auto *etdump = dynamic_cast<executorch::etdump::ETDumpGen *>(_module->event_tracer()))
        {
            const auto trace = etdump->get_etdump_data();

            if (trace.buf && trace.size > 0)
            {
                std::unique_ptr<void, decltype(&free)> guard(trace.buf, free);
                std::ofstream file("trace.etdump", std::ios::binary);

                if (file)
                {
                    file.write(static_cast<const char *>(trace.buf), trace.size);
                }
            }
        }
    }
};
