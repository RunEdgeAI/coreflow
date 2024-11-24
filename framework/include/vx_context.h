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

#include <VX/vx.h>

#include "vx_internal.h"
#include "vx_reference.h"

/*!
 * \file
 * \brief
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_context Internal Context API
 * \ingroup group_internal
 * \brief Internal Context API
 */

/*! \brief The implementation string which is of the format "<vendor>.<substring>" */
extern const vx_char implementation[];

/*! \brief The top level context data for the entire OpenVX instance
 * \ingroup group_int_context
 */
class Context : public Reference
{
public:
    /**
     * @brief Construct a new Context object
     * @ingroup group_int_context
     */
    Context();

    /**
     * @brief Destroy the Context object
     * @ingroup group_int_context
     */
    ~Context();

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
     * \param [in] context The overall context pointer.
     * \param [in] name The shortened name of the target module.
     * \ingroup group_int_target
     */
    vx_status loadTarget(const vx_char* targetName);

    /*! \brief This unloads a specific target in the targets list.
     * \param [in] targetName The string name of the target module.
     * \ingroup group_int_target
     */
    vx_status unloadTarget(const vx_char* targetName);

    /*! \brief This unloads a specific target in the targets list.
     * \param [in] context The overall context pointer.
     * \param [in] index The index into the context's target array.
     * \ingroup group_int_target
     */
    vx_status unloadTarget(vx_uint32 index, vx_bool unload_module);

    /*! \brief Searches the accessors list to find an open spot and then
     * will allocate memory if needed.
     * \ingroup group_int_context
     */
    VX_INT_API vx_bool addAccessor(
                                 vx_size size,
                                 vx_enum usage,
                                 void *ptr,
                                 vx_reference ref,
                                 vx_uint32 *pIndex,
                                 void *extra_data);

    /*! \brief Finds and removes an accessor from the list.
     * \ingroup group_int_context
     */
    VX_INT_API void removeAccessor(vx_uint32 index);

    /*! \brief Finds the accessor in the list and returns the index.
     * \ingroup group_int_context
     */
    VX_INT_API vx_bool findAccessor(const void* ptr, vx_uint32* pIndex);

    /*! \brief Used to add a reference to the context.
     * \param [in] ref The pointer to the reference object.
     * \ingroup group_int_reference
     */
    vx_bool addReference(const vx_reference& ref);

    /*! \brief Used to remove a reference from the context.
     * \param [in] context The system context.
     * \param [in] ref The pointer to the reference object.
     * \ingroup group_int_reference
     */
    vx_bool removeReference(vx_reference& ref);

    /*! \brief Searches the memory maps list to find an open slot and
     *  allocate memory for mapped buffer.
     * \ingroup group_int_context
     */
    VX_INT_API vx_bool memoryMap(
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
    VX_INT_API vx_bool findMemoryMap(
        vx_reference ref,
        vx_map_id    map_id);

    /*! \brief Finds and removes a map_id from the list.
     * \ingroup group_int_context
     */
    VX_INT_API void memoryUnmap(vx_uint32 map_id);

    /**
     * @brief Validate border mode supported
     *
     * @param mode     border mode
     * @return VX_INT_API
     * @ingroup group_int_context
     */
    VX_INT_API static vx_bool isValidBorderMode(vx_enum mode);

    /**
     * @brief Launch worker graph thread
     *
     * @param arg
     * @return vx_value_t
     * @ingroup group_int_context
     */
    VX_INT_API static vx_value_t workerGraph(void *arg);

    /**
     * @brief Launch worker node thread
     *
     * @param worker
     * @return vx_bool
     * @ingroup group_int_context
     */
    VX_INT_API static vx_bool workerNode(vx_threadpool_worker_t *worker);

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
    vx_target         targets[VX_INT_MAX_NUM_TARGETS];
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
};

#endif /* VX_CONTEXT_H */
