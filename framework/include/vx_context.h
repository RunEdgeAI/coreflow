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

#ifndef VX_CONTEXT_H
#define VX_CONTEXT_H

#include <memory>
#include <vector>

#include "vx_event_queue.hpp"
#include "vx_internal.h"
#include "vx_reference.h"

/*!
 * \file
 * \brief
 *
 * \defgroup group_int_context Internal Context API
 * \ingroup group_internal
 * \brief The Internal Context API
 */

/*! \brief The top level context data for the entire OpenVX instance
 * \ingroup group_int_context
 */
namespace corevx {

class Context final : public Reference
{
private:
    /**
     * @brief Launch worker graph thread
     *
     * @param arg         Optional argument to pass as parameter.
     * @return vx_value_t Thread return value.
     * @ingroup group_int_context
     */
    static vx_value_t workerGraph(void* arg);

    /**
     * @brief Launch worker node
     *
     * @param worker   The threadpool of the worker.
     * @return vx_bool vx_true_e if ran successful, vx_false_e otherwise
     * @ingroup group_int_context
     */
    static vx_bool workerNode(vx_threadpool_worker_t* worker);

public:
    /**
     * @brief Construct a new Context object
     * @ingroup group_int_context
     */
    Context();

    /**
     * @brief Delete copy constructor and assignment operator to prevent copying
     * @ingroup group_int_context
     */
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    /**
     * @brief Destroy the Context object
     * @ingroup group_int_context
     */
    ~Context();

    /**
     * @brief Create a new context
     * @ingroup group_int_context
     */
    static vx_context createContext();

    /**
     * @brief Get vendor id
     *
     * @return vx_uint16 The vendor id.
     * @ingroup group_int_context
     */
    vx_uint16 vendorId() const;

    /**
     * @brief Get version number
     *
     * @return vx_uint16 The version number.
     * @ingroup group_int_context
     */
    vx_uint16 version() const;

    /**
     * @brief Get number of loaded modules
     *
     * @return vx_uint32 The number of loaded modules.
     * @ingroup group_int_context
     */
    vx_uint32 numModules() const;

    /**
     * @brief Get number of references
     *
     * @return vx_uint32 The number of tracked references.
     * @ingroup group_int_context
     */
    vx_uint32 numReferences() const;

    /**
     * @brief Get the implementation name
     *
     * @return const vx_char* The implementation name.
     * @ingroup group_int_context
     */
    const vx_char* implName() const;

    /**
     * @brief Get the names of the extensions supported
     *
     * @return const vx_char* The names of the extensions supported
     * @ingroup group_int_context
     */
    const vx_char* extensions() const;

    /**
     * @brief Get the max dimensions of a convolution supported
     *
     * @return vx_size The max dimensions of a convolution supported.
     * @ingroup group_int_context
     */
    vx_size convolutionMaxDim() const;

    /**
     * @brief Get the max dimensions of a non linear supported
     *
     * @return vx_size The max dimensions of a non linear supported.
     * @ingroup group_int_context
     */
    vx_size nonLinearMaxDim() const;

    /**
     * @brief Get the optical flow max window dimension supported
     *
     * @return vx_size The optical flow max window dimension supported.
     * @ingroup group_int_context
     */
    vx_size opticalFlowMaxWindowDim() const;

    /**
     * @brief Get the immediate border
     *
     * @return vx_border_t The immediate border.
     * @ingroup group_int_context
     */
    vx_border_t immediateBorder() const;

    /**
     * @brief Get the immediate border policy
     *
     * @return vx_enum The immediate border policy.
     * @ingroup group_int_context
     */
    vx_enum immediateBorderPolicy() const;

    /**
     * @brief Get the number of unique kernels
     *
     * @return vx_uint32 The number of unique kernels.
     * @ingroup group_int_context
     */
    vx_uint32 numUniqueKernels() const;

    /**
     * @brief Get the max tensor dimensions supported
     *
     * @return vx_size The max tensor dimensions supported.
     * @ingroup group_int_context
     */
    vx_size maxTensorDims() const;

    /**
     * @brief Get the unique kernel information
     *
     * @return std::vector<vx_kernel_info_t> The unique kernel information table.
     * @ingroup group_int_context
     */
    std::vector<vx_kernel_info_t> uniqueKernelTable();

#ifdef OPENVX_USE_OPENCL_INTEROP
    /**
     * @brief Get the OpenCL context
     *
     * @return cl_context The OpenCL context.
     * @ingroup group_int_context
     */
    cl_context clContext() const;

    /**
     * @brief Get the OpenCL command queue
     *
     * @return cl_command_queue The OpenCL command queue.
     * @ingroup group_int_context
     */
    cl_command_queue clCommandQueue() const;
#endif /** OPENVX_USE_OPENCL_INTEROP */

    /**
     * @brief Set the logging enabled state
     *
     * @param flag  vx_bool indicating whether to enable or disable logging
     * @ingroup group_int_context
     */
    void setLoggingEnabled(vx_bool flag);

    /**
     * @brief Set the perf enabled state
     *
     * @param flag vx_bool indicating whether to enable or disable performance tracking
     * @ingroup group_int_context
     */
    void setPerfEnabled(vx_bool flag);

    /*! \brief This determines if a context is valid.
     * \param [in] context The pointer to the context to test.
     * \retval vx_true_e The context is valid.
     * \retval vx_false_e The context is not valid.
     * \ingroup group_int_context
     */
    static vx_bool isValidContext(vx_context context);

    /*! \brief This returns true if the type is within the definition of types in OpenVX.
     * \note VX_TYPE_INVALID is not valid for determining a type.
     * \param [in] type The \ref vx_type_e value.
     * \ingroup group_int_context
     */
    static vx_bool isValidType(vx_enum type);

    /*! \brief This determines if the import type is supported.
     * \param [in] type The \ref vx_import_type_e value.
     * \ingroup group_int_context
     */
    static vx_bool isValidImport(vx_enum type);

    /**
     * @brief Find target by string
     *
     * @param target_string   target string
     * @return vx_target*
     * @ingroup group_int_context
     */
    vx_target* findTargetByString(const char* target_string);

    /*! \brief This allows the implementation to load a target interface into OpenVX.
     * \param [in] targetName The shortened name of the target module.
     * \ingroup group_int_target
     */
    vx_status loadTarget(const vx_char* targetName);

    /*! \brief This unloads a specific target in the targets list.
     * \param [in] targetName The string name of the target module.
     * \ingroup group_int_target
     */
    vx_status unloadTarget(const vx_char* targetName);

    /*! \brief This unloads a specific target in the targets list.
     * \param [in] index The index into the context's target array.
     * \param [in] unload_module If true, the module will be unloaded.
     * \ingroup group_int_target
     */
    vx_status unloadTarget(vx_uint32 index, vx_bool unload_module);

    /*! \brief Searches the accessors list to find an open spot and then
     * will allocate memory if needed.
     * \ingroup group_int_context
     */
    vx_bool addAccessor(vx_size size, vx_enum usage, void*& ptr, vx_reference ref,
                        vx_uint32* pIndex, void* extra_data);

    /*! \brief Finds and removes an accessor from the list.
     * \ingroup group_int_context
     */
    void removeAccessor(vx_uint32 index);

    /*! \brief Finds the accessor in the list and returns the index.
     * \ingroup group_int_context
     */
    vx_bool findAccessor(const void* ptr, vx_uint32* pIndex);

    /*! \brief Used to add a reference to the context.
     * \param [in] ref The pointer to the reference object.
     * \ingroup group_int_reference
     */
    vx_bool addReference(const vx_reference& ref);

    /*! \brief Used to remove a reference from the context.
     * \param [in] ref The pointer to the reference object.
     * \ingroup group_int_reference
     */
    vx_bool removeReference(vx_reference& ref);

    /*! \brief Searches the memory maps list to find an open slot and
     *  allocate memory for mapped buffer.
     * \ingroup group_int_context
     */
    vx_bool memoryMap(
        vx_reference ref,
        vx_size      size,
        vx_enum      usage,
        vx_enum      mem_type,
        vx_uint32    flags,
        void*        extra_data,
        void**       ptr,
        vx_map_id*   map_id);

    /*! \brief Checks the consistency of given ref & map_id by looking
     *  into memory maps list.
     * \ingroup group_int_context
     */
    vx_bool findMemoryMap(
        vx_reference ref,
        vx_map_id    map_id);

    /*! \brief Finds and removes a map_id from the list.
     * \ingroup group_int_context
     */
    void memoryUnmap(vx_uint32 map_id);

    /**
     * @brief Validate border mode supported
     *
     * @param mode     border mode
     * @return vx_bool vx_true_e if valid, vx_false_e otherwise
     * @ingroup group_int_context
     */
    static vx_bool isValidBorderMode(vx_enum mode);

    /**
     * @brief Register a user struct with a certain number of bytes
     *
     * @param size     The size in bytes of the user struct.
     * @return vx_enum The user struct enumeration.
     * @ingroup group_int_context
     */
    vx_enum registerUserStruct(vx_size size);

    /**
     * @brief Get the User Struct By Name object
     *
     * @param name     The user struct name.
     * @return vx_enum The user struct enumeration.
     * @ingroup group_int_context
     */
    vx_enum getUserStructByName(const vx_char* name);

    /**
     * @brief Get the User Struct Name By Enum object
     *
     * @param user_struct_type Enumeration for user struct type.
     * @param type_name        Name of user struct type.
     * @param name_size        Size of user struct name.
     * @return vx_status       VX_SUCCESS if successful, otherwise a return status with an error
     * code.
     * @ingroup group_int_context
     */
    vx_status getUserStructNameByEnum(vx_enum user_struct_type, vx_char* type_name,
                                      vx_size name_size);

    /**
     * @brief Get the User Struct Enum By Name object
     *
     * @param type_name        The user struct name.
     * @param user_struct_type The user struct type enumeration.
     * @return vx_status       VX_SUCCESS if successful, otherwise a return status with an error
     * code.
     * @ingroup group_int_context
     */
    vx_status getUserStructEnumByName(const vx_char* type_name, vx_enum* user_struct_type);

    /**
     * @brief Register user struct with name
     *
     * @param size      Size of user struct name.
     * @param type_name The user struct name.
     * @return vx_enum  Enumeration of registered user struct
     * @ingroup group_int_context
     */
    vx_enum registerUserStructWithName(vx_size size, const vx_char* type_name);

    /**
     * @brief Allocate a unique kernel id
     *
     * @param pKernelEnumId Pointer to allocated kernel id by the framework.
     * @return vx_status    VX_SUCCESS if successful, otherwise a return status with an error code.
     * @ingroup group_int_context
     */
    vx_status allocateKernelId(vx_enum* pKernelEnumId);

    /**
     * @brief Allocate a unique library id
     *
     * @param pLibraryId Pointer to allocated library id by the framework.
     * @return vx_status VX_SUCCESS if successful, otherwise a return status with an error code.
     * @ingroup group_int_context
     */
    vx_status allocateLibraryId(vx_enum* pLibraryId);

    /**
     * @brief Set the Immediate Mode Target
     *
     * @param target_enum   The target enumeration.
     * @param target_string The target string.
     * @return vx_status    VX_SUCCESS if successful, otherwise a return status with an error code.
     * @ingroup group_int_context
     */
    vx_status setImmediateModeTarget(vx_enum target_enum, const char* target_string);

    /*! \brief The pointer to process global lock */
    vx_sem_t*           p_global_lock;
    /*! \brief The reference table which contains the handle for later garage collection if needed */
    vx_reference        reftable[VX_INT_MAX_REF];
    /*! \brief The number of references in the table. */
    vx_uint32           num_references;
    /*! \brief The array of kernel modules. */
    vx_module_t         modules[VX_INT_MAX_MODULES];
    /*! \brief The number of kernel libraries loaded */
    vx_uint32           num_modules;
    /*! \brief The graph queue processor */
    vx_processor_t      proc;
    /*! \brief The combined number of unique kernels in the system */
    vx_uint32           num_kernels;
    /*! \brief The number of unique kernels */
    vx_uint32           num_unique_kernels;
    /*! \brief The number of available targets in the implementation */
    vx_uint32           num_targets;
    /*! \brief The list of implemented targets */
    vx_target           targets[VX_INT_MAX_NUM_TARGETS];
    /*! \brief The list of priority sorted target indexes */
    vx_uint32           priority_targets[VX_INT_MAX_NUM_TARGETS];
    /*! \brief The log callback for errors */
    vx_log_callback_f   log_callback;
    /*! \brief The log semaphore */
    vx_sem_t            log_lock;
    /*! \brief The log enable toggle. */
    vx_bool             log_enabled;
    /*! \brief If true the log callback is reentrant and doesn't need to be locked. */
    vx_bool             log_reentrant;
    /*! \brief The performance counter enable toggle. */
    vx_bool             perf_enabled;
    /*! \brief The list of externally accessed references */
    vx_external_t       accessors[VX_INT_MAX_REF];
    /*! \brief The memory mapping table lock */
    vx_sem_t            memory_maps_lock;
    /*! \brief The list of memory maps */
    vx_memory_map_t     memory_maps[VX_INT_MAX_REF];
    /*! \brief The list of user defined structs. */
    struct {
        /*! \brief Type constant */
        vx_enum type;
        /*! \brief Size in bytes */
        vx_size size;
        /*! \brief Name */
        vx_char name[VX_MAX_STRUCT_NAME];
    } user_structs[VX_INT_MAX_USER_STRUCTS];
    /*! \brief The worker pool used to parallelize the graph*/
    vx_threadpool_t*    workers;
#if defined(EXPERIMENTAL_USE_OPENCL)
#define CL_MAX_PLATFORMS (1)
#define CL_MAX_DEVICES   (2)
#define CL_MAX_KERNELS   (50)
    /*! \brief The array of platform ids */
    cl_platform_id      platforms[CL_MAX_PLATFORMS];
    /*! \brief The number of platform ids */
    cl_uint             num_platforms;
    cl_device_id        devices[CL_MAX_PLATFORMS][CL_MAX_DEVICES];
    cl_uint             num_devices[CL_MAX_PLATFORMS];
    cl_context          global[CL_MAX_PLATFORMS];
    cl_command_queue    queues[CL_MAX_PLATFORMS][CL_MAX_DEVICES];
#endif
    /*! \brief The immediate mode border */
    vx_border_t         imm_border;
    /*! \brief The unsupported border mode policy for immediate mode functions */
    vx_enum             imm_border_policy;
    /*! \brief The next available dynamic user kernel ID */
    vx_uint32           next_dynamic_user_kernel_id;
    /*! \brief The next available dynamic user library ID */
    vx_uint32           next_dynamic_user_library_id;
    /*! \brief The immediate mode enumeration */
    vx_enum             imm_target_enum;
    /*! \brief The immediate mode target string */
    vx_char             imm_target_string[VX_MAX_TARGET_NAME];
#ifdef OPENVX_USE_OPENCL_INTEROP
    cl_context opencl_context;
    cl_command_queue opencl_command_queue;
#endif
#ifdef OPENVX_USE_PIPELINING
    /*! \brief The event queue for the context */
    EventQueue event_queue;
#endif
    /*! \brief The graph queue for the context */
    vx_value_set_t graph_queue[VX_INT_MAX_QUEUE_DEPTH];
    /*! \brief The number of graphs in the queue */
    vx_size numGraphsQueued;
    /*! \brief The vendor id */
    const vx_uint16 vendor_id;
    /*! \brief The version number this implements */
    const vx_uint16 version_number;
    /*! \brief The implementation string which is of the format "<vendor>.<substring>" */
    const vx_char implementation[VX_MAX_IMPLEMENTATION_NAME];
    /*! \brief The name of additional extensions in this impleemntation */
    const vx_char* extension;
};

} // namespace corevx

#endif /* VX_CONTEXT_H */