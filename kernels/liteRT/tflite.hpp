/**
 * @file tflite.hpp
 * @brief
 * @version 0.1
 * @date 2025-04-19
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <cstdio>
#include <cstdlib>
#include <memory>

#include "tensorflow/lite/core/interpreter_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/optional_debug_tools.h"

#define TFLITE_MINIMAL_CHECK(x)                                  \
    if (!(x))                                                    \
    {                                                            \
        fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
        return VX_FAILURE;                                       \
    }

/**
 * @brief Class to run TFLite models
 *
 */
class TFLiteRunner
{
public:
    /**
     * @brief TFLiteRunner Constructor
     */
    TFLiteRunner() : modelLoaded(false) {};

    /**
     * @brief Initialize the TFLite interpreter (load the model)
     * @param filename Path to the ONNX model file
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status init(std::string &filename)
    {
        TFLITE_MINIMAL_CHECK(false == filename.empty())

        if (!modelLoaded)
        {
            // Load model
            model = tflite::FlatBufferModel::BuildFromFile(filename.c_str());
            TFLITE_MINIMAL_CHECK(model != nullptr);

            // Build the interpreter with the InterpreterBuilder.
            // Note: all Interpreters should be built with the InterpreterBuilder,
            // which allocates memory for the Interpreter and does various set up
            // tasks so that the Interpreter can read the provided model.
            tflite::ops::builtin::BuiltinOpResolver resolver;
            tflite::InterpreterBuilder builder(*model, resolver);
            builder(&interpreter);
            TFLITE_MINIMAL_CHECK(interpreter != nullptr);

            printf("=== Pre-invoke Interpreter State ===\n");
            tflite::PrintInterpreterState(interpreter.get());
        }

        return VX_SUCCESS;
    }

    /**
     * @brief Validate input/output parameters
     * @param inputDims  Input tensor dimensions
     * @param outputDims Output tensor dimensions
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status validate(std::vector<std::vector<size_t>> &inputDims, std::vector<std::vector<size_t>> &outputDims)
    {
        vx_status status = VX_SUCCESS;

        // Validate input dimensions
        if (inputDims.size() != interpreter->inputs().size())
        {
            fprintf(stderr, "Mismatch in number of input tensors: expected %zu, got %zu\n",
                    inputDims.size(), interpreter->inputs().size());
            return VX_FAILURE;
        }

        for (std::size_t i = 0; i < interpreter->inputs().size(); ++i)
        {
            TfLiteTensor *input_tensor = interpreter->tensor(interpreter->inputs()[i]);
            if (input_tensor == nullptr)
            {
                fprintf(stderr, "Input tensor at index %zu is null.\n", i);
                return VX_FAILURE;
            }

            // Get the shape of the input tensor
            std::vector<size_t> tensor_shape(input_tensor->dims->size);
            for (int j = 0; j < input_tensor->dims->size; ++j)
            {
                tensor_shape[j] = input_tensor->dims->data[j];
            }

            // Compare with the expected shape
            if (tensor_shape != inputDims[i])
            {
                fprintf(stderr, "Mismatch in input tensor %zu shape: expected {", i);
                for (size_t dim : inputDims[i])
                    fprintf(stderr, "%zu,", dim);
                fprintf(stderr, "} but got {");
                for (size_t dim : tensor_shape)
                    fprintf(stderr, "%zu,", dim);
                fprintf(stderr, "}\n");
                return VX_FAILURE;
            }
        }

        // Validate output dimensions
        if (outputDims.size() != interpreter->outputs().size())
        {
            fprintf(stderr, "Mismatch in number of output tensors: expected %zu, got %zu\n",
                    outputDims.size(), interpreter->outputs().size());
            return VX_FAILURE;
        }

        for (std::size_t i = 0; i < interpreter->outputs().size(); ++i)
        {
            TfLiteTensor *output_tensor = interpreter->tensor(interpreter->outputs()[i]);
            if (output_tensor == nullptr)
            {
                fprintf(stderr, "Output tensor at index %zu is null.\n", i);
                return VX_FAILURE;
            }

            // Get the shape of the output tensor
            std::vector<size_t> tensor_shape(output_tensor->dims->size);
            for (int j = 0; j < output_tensor->dims->size; ++j)
            {
                tensor_shape[j] = output_tensor->dims->data[j];
            }

            // Compare with the expected shape
            if (tensor_shape != outputDims[i])
            {
                fprintf(stderr, "Mismatch in output tensor %zu shape: expected {", i);
                for (size_t dim : outputDims[i])
                    fprintf(stderr, "%zu,", dim);
                fprintf(stderr, "} but got {");
                for (size_t dim : tensor_shape)
                    fprintf(stderr, "%zu,", dim);
                fprintf(stderr, "}\n");
                return VX_FAILURE;
            }
        }

        return status;
    }

    /**
     * @brief Allocate memory for input and output tensors
     * @param inputTensors  Input tensors
     * @param outputTensors Output tensors
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status allocate(std::vector<std::pair<float *, vx_size>> &inputTensors, std::vector<std::pair<float *, vx_size>> &outputTensors)
    {
        vx_status status = VX_SUCCESS;

        // Fill input buffers
        // TODO(user): Insert code to fill input tensors.
        // Note: The buffer of the input tensor with index `i` of type T can
        // be accessed with `T* input = interpreter->typed_input_tensor<T>(i);`
        for (std::size_t i = 0; i < interpreter->inputs().size(); ++i)
        {
            status = bindMemory(interpreter->inputs()[i], inputTensors[i].first, inputTensors[i].second);
        }

        // Read output buffers
        // TODO(user): Insert getting data out code.
        // Note: The buffer of the output tensor with index `i` of type T can
        // be accessed with `T* output = interpreter->typed_output_tensor<T>(i);`
        for (std::size_t i = 0; i < interpreter->outputs().size(); ++i)
        {
            status |= bindMemory(interpreter->outputs()[i], outputTensors[i].first, outputTensors[i].second);
        }

        // Allocate tensor buffers.
        TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);

        return status;
    }

    /**
     * @brief Run the kernel (execute the model)
     * @param inputTensors  Input tensors
     * @param outputTensosrs Output tensors
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status run()
    {
        // Run inference
        TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);
        printf("\n\n=== Post-invoke Interpreter State ===\n");
        tflite::PrintInterpreterState(interpreter.get());
        return VX_SUCCESS;
    }

private:
    bool modelLoaded = false;
    std::unique_ptr<tflite::FlatBufferModel> model;
    // Pointer to the TFLite interpreter
    std::unique_ptr<tflite::Interpreter> interpreter;

    /**
     * @brief Bind pre-allocated memory to a tensor
     * @param tensor_index Index of the tensor to bind
     * @param pre_allocated_memory Pointer to the pre-allocated memory
     * @param size_in_bytes Size of the pre-allocated memory in bytes
     * @return VX_SUCCESS on success, VX_FAILURE otherwise
     */
    vx_status bindMemory(int tensor_index, void* pre_allocated_memory, size_t size_in_bytes)
    {
        vx_status status = VX_SUCCESS;

        // Get the tensor
        TfLiteTensor* tensor = interpreter->tensor(tensor_index);

        // Check if the tensor exists
        if (tensor == nullptr)
        {
            fprintf(stderr, "Tensor at index %d does not exist.\n", tensor_index);
            status = VX_FAILURE;
        }

        // Ensure the tensor type and size match your pre-allocated memory
        if (VX_SUCCESS == status &&
            tensor->bytes != size_in_bytes)
        {
            fprintf(stderr, "Pre-allocated memory size (%ld) does not match tensor size (%ld).\n",
                    size_in_bytes, tensor->bytes);
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            // Bind the pre-allocated memory to the tensor
            TFLITE_MINIMAL_CHECK(kTfLiteOk == interpreter->SetCustomAllocationForTensor(
                                                  tensor_index,
                                                  {pre_allocated_memory, size_in_bytes},
                                                  kTfLiteCustomAllocationFlagsSkipAlignCheck));
        }

        return status;
    }
};
