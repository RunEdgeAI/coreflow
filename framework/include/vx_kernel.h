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

#ifndef VX_KERNEL_H
#define VX_KERNEL_H

#ifdef OPENVX_KHR_TILING
#include <VX/vx_khr_tiling.h>
#endif /* OPENVX_KHR_TILING */

#include "vx_internal.h"
#include "vx_reference.h"

/*!
 * \file
 * \brief The internal kernel implementation.
 *
 * \defgroup group_int_kernel Internal Kernel API
 * \ingroup group_internal
 * \brief The internal Kernel API.
 */

/*! \brief The internal representation of an abstract kernel.
 * \ingroup group_int_kernel
 */
namespace corevx {

class Kernel : public Reference
{
public:
    /**
     * @brief The parameter of the kernel
     *
     * @ingroup group_int_kernel
     */
    struct Param
    {
        vx_enum direction;  // VX_INPUT, VX_OUTPUT, etc.
        vx_enum type;       // VX_TYPE_SCALAR, VX_TYPE_IMAGE, etc.
        vx_enum state;      // VX_PARAMETER_STATE_REQUIRED, VX_PARAMETER_STATE_OPTIONAL, etc.
    };

    /**
     * @brief Construct a new Kernel object
     *
     * @param context   The context associated with this obj
     * @param scope     The parent ref of this obj
     * @ingroup group_int_kernel
     */
    Kernel(vx_context context, vx_reference scope);

    /**
     * @brief Construct a new Kernel object
     *
     * @param context
     * @param kenum
     * @param function
     * @param name
     * @param parameters
     * @param numParams
     * @param scope
     * @ingroup group_int_kernel
     */
    Kernel(vx_context context,
            vx_enum kenum,
            vx_kernel_f function,
            vx_char name[VX_MAX_KERNEL_NAME],
            vx_param_description_t *parameters,
            vx_uint32 numParams,
            vx_reference scope);

    /**
     * @brief Destroy the Kernel object
     * @ingroup group_int_kernel
     */
    ~Kernel() = default;

    /**
     * @brief Register a custom kernel
     *
     * @param context The context
     * @param name The name of the kernel
     * @param params The parameters of the kernel
     * @param function The function of the kernel
     * @param validate The validate function of the kernel
     * @param initialize The initialize function of the kernel
     * @param deinitialize The deinitialize function of the kernel
     * @return vx_kernel The kernel
     * @ingroup group_int_kernel
     */
    static vx_kernel registerCustomKernel(
        vx_context context,
        std::string name,
        const std::vector<Kernel::Param> &params,
        vx_kernel_f function,
        vx_kernel_validate_f validate = nullptr,
        vx_kernel_initialize_f initialize = nullptr,
        vx_kernel_deinitialize_f deinitialize = nullptr);

    /**
     * @brief Get the number of kernel parameters
     *
     * @return vx_uint32 The number of kernel parameters.
     * @ingroup group_int_kernel
     */
    vx_uint32 numParameters() const;

    /**
     * @brief Get the kernel name
     *
     * @return const vx_char* The kernel name.
     * @ingroup group_int_kernel
     */
    const vx_char *kernelName() const;

    /**
     * @brief Get the kernel enuemration
     *
     * @return vx_enum The kernel enuemration.
     * @ingroup group_int_kernel
     */
    vx_enum kernelEnum() const;

    /**
     * @brief Get the local data size
     *
     * @return vx_size The local data size.
     * @ingroup group_int_kernel
     */
    vx_size localDataSize() const;

#ifdef OPENVX_KHR_TILING
    /**
     * @brief Get the input neighborhood size
     *
     * @return vx_neighborhood_size_t The input neighborhood size.
     * @ingroup group_int_kernel
     */
    vx_neighborhood_size_t inputNeighborhood() const;

    /**
     * @brief Get the output tile block size
     *
     * @return vx_tile_block_size_t The output tile block size.
     * @ingroup group_int_kernel
     */
    vx_tile_block_size_t outputTileBlockSize() const;

    /**
     * @brief Get the kernel border object
     *
     * @return vx_border_t The kernel border object.
     * @ingroup group_int_kernel
     */
    vx_border_t border() const;
#endif /* OPENVX_KHR_TILING */

#ifdef OPENVX_USE_OPENCL_INTEROP
    /**
     * @brief Is Opencl in use
     *
     * @return vx_bool vx_true_e if used, otherwise vx_false_e.
     * @ingroup group_int_kernel
     */
    vx_bool useOpencl() const;
#endif /* OPENVX_USE_OPENCL_INTEROP */

    /**
     * @brief Get the pipeup input depth
     *
     * @return vx_uint32 The pipeup input depth.
     * @ingroup group_int_kernel
     */
    vx_uint32 pipeupInputDepth() const;

    /**
     * @brief Get the pipeup output depth
     *
     * @return vx_uint32 The pipeup output depth.
     * @ingroup group_int_kernel
     */
    vx_uint32 pipeupOutputDepth() const;

    /**
     * @brief Set the local data size
     *
     * @param size The local data size.
     * @ingroup group_int_kernel
     */
    void setLocalDataSize(vx_size size);

#ifdef OPENVX_KHR_TILING
    /**
     * @brief Set the input neighborhood size
     *
     * @param input The input neighborhood size.
     * @ingroup group_int_kernel
     */
    void setInputNeighborhood(vx_neighborhood_size_t input);

    /**
     * @brief Set the output tile block size
     *
     * @param tile_size The output tile block size.
     * @ingroup group_int_kernel
     */
    void setOutputTileBlockSize(vx_tile_block_size_t tile_size);

    /**
     * @brief Set the kernel border object
     *
     * @param border The kernel border object.
     * @ingroup group_int_kernel
     */
    void setBorder(vx_border_t border);
#endif /* OPENVX_KHR_TILING */

#ifdef OPENVX_USE_OPENCL_INTEROP
    /**
     * @brief Set Opencl access
     *
     * @param flag Opencl access flag.
     * @ingroup group_int_kernel
     */
    void setOpenclAccess(vx_bool flag);
#endif /* OPENVX_USE_OPENCL_INTEROP */

    /**
     * @brief Set the pipeup input depth
     *
     * @param depth The pipeup input depth.
     * @ingroup group_int_kernel
     */
    void setInputDepth(vx_uint32 depth);

    /**
     * @brief Set the pipeup output depth
     *
     * @param depth The pipeup output depth.
     * @ingroup group_int_kernel
     */
    void setOutputDepth(vx_uint32 depth);

    /*! \brief Determines if a kernel is unique in the system.
     * \param kernel The handle to the kernel.
     * \ingroup group_int_kernel
     */
    static vx_bool isKernelUnique(vx_kernel kernel);

    /*! \brief Used to initialize a kernel object in a target kernel list.
     * \param [in] kenum The kernel enumeration value.
     * \param [in] function The pointer to the function of the kernel.
     * \param [in] name The name of the kernel in dotted notation.
     * \param [in] parameters The list of parameters for each kernel.
     * \param [in] numParams The number of parameters in the list.
     * \param [in] validator The function pointer to the params validator.
     * \param [in] input_validator The function pointer to the input validator.
     * \param [in] output_validator The function pointer to the output validator.
     * \param [in] initialize The function to call to initialize the kernel.
     * \param [in] deinitialize The function to call to deinitialize the kernel.
     * \ingroup group_int_kernel
     */
    vx_status initializeKernel(vx_enum kenum,
                             vx_kernel_f function,
                             vx_char name[VX_MAX_KERNEL_NAME],
                             vx_param_description_t *parameters,
                             vx_uint32 numParams,
                             vx_kernel_validate_f validator,
                             vx_kernel_input_validate_f input_validator,
                             vx_kernel_output_validate_f output_validator,
                             vx_kernel_initialize_f initialize,
                             vx_kernel_deinitialize_f deinitialize);

    /**
     * @brief Add Kernel
     *
     * @param context           The global context
     * @param name              The kernel name value.
     * @param enumeration       The kernel enumeration value.
     * @param func_ptr          The pointer to the function of the kernel.
     * @param numParams         The number of parameters in the kernel.
     * @param validate          The function pointer to the params validator.
     * @param input             The function pointer to the input validator.
     * @param output            The function pointer to the output validator.
     * @param initialize        The function to call to initialize the kernel.
     * @param deinitialize      The function to call to deinitialize the kernel.
     * @param valid_rect_reset  The bool to reset the valid rect or not.
     * @return vx_kernel
     * @ingroup group_int_kernel
     */
    static vx_kernel addkernel(vx_context context,
                           const vx_char name[VX_MAX_KERNEL_NAME],
                           vx_enum enumeration,
                           vx_kernel_f func_ptr,
                           vx_uint32 numParams,
                           vx_kernel_validate_f validate,
                           vx_kernel_input_validate_f input,
                           vx_kernel_output_validate_f output,
                           vx_kernel_initialize_f initialize,
                           vx_kernel_deinitialize_f deinitialize,
                           vx_bool valid_rect_reset);

#ifdef OPENVX_KHR_TILING
    /**
     * @brief Allows a user to add a tile-able kernel to the framework.
     *
     * @param [in] context The handle to the implementation context.
     * @param [in] name The string to be used to match the kernel.
     * @param [in] enumeration The enumerated value of the kernel to be used by clients.
     * @param [in] flexible_func_ptr The process-local flexible function pointer to be invoked.
     * @param [in] fast_func_ptr The process-local fast function pointer to be invoked.
     * @param [in] num_params The number of parameters for this kernel.
     * @param [in] input The pointer to a function which will validate the
     * input parameters to this kernel.
     * @param [in] output The pointer to a function which will validate the
     * output parameters to this kernel.
     * @note Tiling Kernels do not have access to any of the normal node attributes listed
     * in @ref vx_node_attribute_e.
     * @post Call <tt>\ref addParameter</tt> for as many parameters as the function has,
     * then call <tt>\ref finalize</tt>.
     * @return vx_kernel Nullptr indicates that an error occurred when adding the kernel.
     * Note that the fast or flexible formula, but not both, can be NULL.
     * @ingroup group_int_kernel
     */
    static vx_kernel addTilingKernel(vx_context context, vx_char name[VX_MAX_KERNEL_NAME],
                                     vx_enum enumeration, vx_tiling_kernel_f flexible_func_ptr,
                                     vx_tiling_kernel_f fast_func_ptr, vx_uint32 num_params,
                                     vx_kernel_input_validate_f input,
                                     vx_kernel_output_validate_f output);
#endif /* OPENVX_KHR_TILING */

    /**
     * @brief This API is called after all parameters have been added to the
     * kernel and the kernel is \e ready to be used. Notice that the reference to the kernel created
     * by addKernel is still valid after the call to finalize.
     * If an error occurs, the kernel is not available for usage by the clients of the framework.
     * Typically this is due to a mismatch between the number of parameters requested and given.
     *
     * @return vx_status VX_SUCCESS if successful, any other value indicates failure.
     * @ingroup group_int_kernel
     */
    vx_status finalize();

    /**
     * @brief Allows users to set the signatures of the custom kernel.
     *
     * @param [in] index The index of the parameter to add.
     * @param [in] dir The direction of the parameter. This must be either <tt>\ref VX_INPUT</tt> or
     * <tt>\ref VX_OUTPUT</tt>.
     * @param [in] data_type The type of parameter. This must be a value from <tt>\ref
     * vx_type_e</tt>.
     * @param [in] state The state of the parameter (required or not). This must be a value from
     * <tt>\ref vx_parameter_state_e</tt>.
     *
     * @return vx_status VX_SUCCESS if successful, any other value indicates failure.
     * @ingroup group_int_kernel
     */
    vx_status addParameter(vx_uint32 index, vx_enum dir, vx_enum data_type, vx_enum state);

    /**
     * @brief Removes a custom kernel from its context and releases it.
     *
     * @param [in] kernel The reference to the kernel to remove. Returned from <tt>\ref
     * addkernel</tt>.
     * @note Any kernel enumerated in the base standard
     * cannot be removed; only kernels added through <tt>\ref vxAddUserKernel</tt> can
     * be removed.
     *
     * @return vx_status VX_SUCCESS if successful, any other value indicates failure.
     * @ingroup group_int_kernel
     */
    static vx_status removeKernel(vx_kernel kernel);

    /**
     * @brief Loads a library of kernels, called module, into the context.
     *
     * @note When all references to loaded kernels are released, the module
     * may be automatically unloaded.
     * @param [in] context The reference to the context the kernels must be added to.
     * @param [in] name    The short name of the module to load. On systems where
     * there are specific naming conventions for modules, the name passed
     * should ignore such conventions. For example: \c libxyz.so should be
     * passed as just \c xyz and the implementation will <i>do the right thing</i> that
     * the platform requires.
     *
     * @return vx_status VX_SUCCESS if successful, any other value indicates failure.
     * @ingroup group_int_kernel
     */
    static vx_status loadKernels(vx_context context, const vx_char *name);

    /**
     * @brief Unloads all kernels from the context that had been loaded from
     * the module using the \ref loadKernels function.
     *
     * @param [in] context The reference to the context the kernels must be removed from.
     * @param [in] name    The short name of the module to unload. On systems where
     * there are specific naming conventions for modules, the name passed
     * should ignore such conventions. For example: \c libxyz.so should be
     * passed as just \c xyz and the implementation will <i>do the right thing</i>
     * that the platform requires.
     * @note This API uses the system pre-defined paths for modules.
     *
     * @return vx_status VX_SUCCESS if successful, any other value indicates failure.
     * @ingroup group_int_kernel
     */
    static vx_status unloadKernels(vx_context context, const vx_char *name);

    /**
     * @brief Get the Kernel By Name
     *
     * @param [in] context The reference to the implementation context.
     * @param [in] string  The string of the name of the kernel to get.
     * @return vx_kernel   A <tt>\ref vx_kernel</tt> reference. Any possible errors preventing a
     * successful completion of the function should be checked using <tt>\ref Error::getStatus</tt>.
     * @ingroup group_int_kernel
     */
    static vx_kernel getKernelByName(vx_context context, const vx_char string[VX_MAX_KERNEL_NAME]);

    /**
     * @brief Get the Kernel By Enum
     *
     * @param context    The reference to the implementation context.
     * @param kernelenum A value from a vendor or client-defined value.
     * @return vx_kernel A <tt>\ref vx_kernel</tt> reference. Any possible errors preventing a
     * successful completion of the function should be checked using <tt>\ref Error::getStatus</tt>.
     * @ingroup group_int_kernel
     */
    static vx_kernel getKernelByEnum(vx_context context, vx_enum kernelenum);

    /*! \brief Used to deinitialize a kernel object in a target kernel list.
     * \return vx_status VX_SUCCESS if successful, any other value indicates failure.
     * \ingroup group_int_kernel
     */
    vx_status deinitializeKernel();

    /**
     * @brief Print kernel object
     *
     * @param kernel
     * @ingroup group_int_kernel
     */
    static void printKernel(vx_kernel kernel);

    /*! \brief The name of the kernel */
    vx_char        name[VX_MAX_KERNEL_NAME];
    /*! \brief The kernel enum ID */
    vx_enum        enumeration;
    /*! \brief The kernel function pointer */
    vx_kernel_f    function;
    /*! \brief The kernel signature */
    vx_signature_t signature;
    /*! \brief Indicates that the kernel is not yet enabled. */
    vx_bool        enabled;
    /*! \brief Indicates that this kernel is added by user. */
    vx_bool        user_kernel;
    /*! \brief The kernel validate function pointer */
    vx_kernel_validate_f validate;
    /*! \brief The kernel input validate function pointer */
    vx_kernel_input_validate_f validate_input;
    /*! \brief The kernel output validate function pointer */
    vx_kernel_output_validate_f validate_output;
    /*! \brief The kernel init function pointer */
    vx_kernel_initialize_f initialize;
    /*! \brief The kernel deinit function pointer */
    vx_kernel_deinitialize_f deinitialize;
    /*! \brief The collection of attributes of a kernel */
    vx_kernel_attr_t attributes;
    /*! \brief Target Index, back reference for the later nodes to inherit affinity */
    vx_uint32 affinity;
#ifdef OPENVX_KHR_TILING
    /*! \brief The tiling function pointer interface */
    vx_tiling_kernel_f tilingfast_function;
    vx_tiling_kernel_f tilingflexible_function;
#endif /* OPENVX_KHR_TILING */
    /*! \brief The pointer to the kernel object deinitializer. */
    vx_kernel_object_deinitialize_f kernel_object_deinitialize;
    /*! \brief The kernel's input depth required to start */
    vx_uint32 input_depth;
    /*! \brief The kernel's output depth required to start */
    vx_uint32 output_depth;
    /*! \brief Indicates whether kernel has piped up */
    vx_uint32 pipeUpCounter;
};

} // namespace corevx

#endif /* VX_KERNEL_H */