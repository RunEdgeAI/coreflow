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

#ifndef VX_OSAL_H
#define VX_OSAL_H

#include <VX/vx.h>

#include "vx_internal.h"

/*!
 * \file
 * \brief The internal operating system abstraction layer.
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_osal Internal OSAL API
 * \ingroup group_internal
 * \brief The Internal Operating System Abstraction Layer API.
 */

/**
 * @brief OS Abstraction Layer Utility Class
 * @ingroup group_int_osal
 */
class Osal
{
public:
    /*! \brief Creates a semaphore object and sets to a given count.
     * \ingroup group_int_osal
     * \param[in] sem The pointer to the semaphore object.
     * \param[in] count The count to set the semaphore to.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool createSem(vx_sem_t *sem, vx_uint32 count);

    /*! \brief Releases a semaphore object.
     * \ingroup group_int_osal
     * \param[in] sem The pointer to the semaphore object.
     */
    static void destroySem(vx_sem_t *sem);

    /*! \brief Posts a semaphore object.
     * \ingroup group_int_osal
     * \param[in] sem The pointer to the semaphore object.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool semPost(vx_sem_t *sem);

    /*! \brief Waits on a semaphore object.
     * \ingroup group_int_osal
     * \param[in] sem The pointer to the semaphore object.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool semWait(vx_sem_t *sem);

    /*! \brief Tries to wait on a semaphore object.
     * \ingroup group_int_osal
     * \param[in] sem The pointer to the semaphore object.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool semTryWait(vx_sem_t *sem);

    /*! \brief Joins a thread.
     * \ingroup group_int_osal
     * \param[in] thread The thread to join.
     * \param[out] value The return value of the thread.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool joinThread(vx_thread_t thread, vx_value_t *value);

    /*! \brief Creates a thread.
     * \ingroup group_int_osal
     * \param[in] func The function to run in the thread.
     * \param[in] arg The argument to pass to the function.
     * \return The thread handle.
     */
    static vx_thread_t createThread(vx_thread_f func, void *arg);

    /*! \brief Sleeps the current thread for a given number of milliseconds.
     * \ingroup group_int_osal
     * \param[in] milliseconds The number of milliseconds to sleep.
     */
    static void sleepThread(vx_uint32 milliseconds);

    /*! \brief Initializes an event.
     * \ingroup group_int_osal
     * \param[in] e The pointer to the event object.
     * \param[in] autoreset The flag to autoreset the event.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool initEvent(vx_internal_event_t *e, vx_bool autoreset);

    /*! \brief Waits on an event.
     * \ingroup group_int_osal
     * \param[in] e The pointer to the event object.
     * \param[in] timeout The timeout in milliseconds.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool waitEvent(vx_internal_event_t *e, vx_uint32 timeout);

    /*! \brief Sets an event.
     * \ingroup group_int_osal
     * \param[in] e The pointer to the event object.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool setEvent(vx_internal_event_t *e);

    /*! \brief Resets an event.
     * \ingroup group_int_osal
     * \param[in] e The pointer to the event object.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool resetEvent(vx_internal_event_t *e);

    /**
     * @brief Wait on an internal event
     * @ingroup group_int_osal
     * @param e The pointer to the event object.
     * @param ms The timeout in milliseconds.
     * @return vx_bool vx_true_e if successful, else vx_false_e.
     */
    static vx_bool waitEventInternal(vx_internal_event_t *e, vx_uint32 ms);

    /*! \brief Deinitializes an event.
     * \ingroup group_int_osal
     * \param[in] e The pointer to the event object.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool deinitEvent(vx_internal_event_t *e);

    /*! \brief Create and allocate thread pool.
     * \ingroup group_int_osal
     * \param[in] numThreads The number of threads in the pool.
     * \param[in] numWorkItems The number of work items in the pool.
     * \param[in] sizeWorkItem The size of each work item.
     * \param[in] worker The worker function.
     * \param[in] arg The argument to pass to the worker function.
     * \return The pointer to the thread pool.
     */
    static vx_threadpool_t *createThreadpool(vx_uint32 numThreads,
                                        vx_uint32 numWorkItems,
                                        vx_size sizeWorkItem,
                                        vx_threadpool_f worker,
                                        void *arg);

    /*! \brief Start and issue tasks to thread pool.
     * \ingroup group_int_osal
     * \param[in] pool The pointer to the thread pool.
     * \param[in] workitems The work items to issue.
     * \param[in] numWorkItems The number of work items to issue.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool issueThreadpool(vx_threadpool_t *pool, vx_value_set_t workitems[], uint32_t numWorkItems);

    /*! \brief Complete and join thread pool.
     * \ingroup group_int_osal
     * \param[in] pool The pointer to the thread pool.
     * \param[in] blocking The flag to block until all threads are done.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool completeThreadpool(vx_threadpool_t *pool, vx_bool blocking);

    /**
     * @brief Launch worker thread pool.
     * @ingroup group_int_osal
     * @param arg The argument to pass to the worker function.
     * @return vx_value_t The return value of the worker function.
     */
    static vx_value_t workerThreadpool(void *arg);

    /*! \brief Destroy thread pool.
     * \ingroup group_int_osal
     * \param[in] ppool The pointer to the thread pool.
     */
    static void destroyThreadpool(vx_threadpool_t **ppool);

private:
    /*! \brief Gets the capture time in nanoseconds.
     * \ingroup group_int_osal
     * \return The time in nanoseconds.
     */
    static vx_uint64 getCaptureTime();

    /*! \brief Gets the clock rate in nanoseconds.
     * \ingroup group_int_osal
     * \return The clock rate in nanoseconds.
     */
    static vx_uint64 getClockRate();

public:
    /*! \brief Starts the performance capture.
     * \ingroup group_int_osal
     * \param [in] perf The pointer to the performance information.
     */
    static void startCapture(vx_perf_t *perf);

    /*! \brief Stops the performance capture.
     * \ingroup group_int_osal
     * \param[in] perf The pointer to the performance information.
     */
    static void stopCapture(vx_perf_t *perf);

    /*! \brief Converts a vx_uint64 to a float in milliseconds.
     * \ingroup group_int_osal
     * \param[in] c The time in nanoseconds.
     * \return The time in milliseconds.
     */
    static vx_float32 timeToMS(vx_uint64 c);

    /*! \brief Initializes the performance information.
     * \ingroup group_int_osal
     * \param[in] perf The pointer to the performance information.
     */
    static void initPerf(vx_perf_t *perf);

    /*! \brief Prints the performance information.
     * \ingroup group_int_osal
     * \param[in] perf The pointer to the performance information.
     */
    static void printPerf(vx_perf_t *perf);

    /*! \brief Initializes the queue.
     * \ingroup group_int_osal
     * \param[in] q The pointer to the queue object.
     */
    static void initQueue(vx_queue_t *q);

    /*! \brief Creates a queue object.
     * \ingroup group_int_osal
     * \return The pointer to the queue object.
     */
    static vx_queue_t *createQueue();

    /*! \brief Creates a queue object of specific size.
     * \ingroup group_int_osal
     * \param[in] numItems The number of items in the queue.
     * \param[in] itemSize The size of each item in the queue.
     * \return The pointer to the queue object.
     */
    vx_queue_t *createQueue(vx_uint32 numItems, vx_size itemSize);

    /*! \brief Reads from a queue object.
     * \ingroup group_int_osal
     * \param[in] q The pointer to the queue object.
     * \param[out] data The pointer to the data read.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool readQueue(vx_queue_t *q, vx_value_set_t **data);

    /*! \brief Writes to a queue object.
     * \ingroup group_int_osal
     * \param[in] q The pointer to the queue object.
     * \param[in] data The pointer to the data to write.
     * \return vx_true_e if successful, else vx_false_e.
     */
    static vx_bool writeQueue(vx_queue_t *q, vx_value_set_t *data);

    /*! \brief Pops a queue object.
     * \ingroup group_int_osal
     * \param[in] q The pointer to the queue object.
     */
    static void popQueue(vx_queue_t *q);

    /*! \brief Destroys a queue object.
     * \ingroup group_int_osal
     * \param[in] pq The pointer to the queue object.
     */
    static void destroyQueue(vx_queue_t **pq);

    /*! \brief Deinitializes a queue object.
     * \ingroup group_int_osal
     * \param[in] q The pointer to the queue object.
     */
    static void deinitQueue(vx_queue_t *q);

    /**
     * @brief Print the queue object
     * @ingroup group_int_osal
     * @param[in] q
     */
    static void printQueue(vx_queue_t *q);

    /*! \brief Loads a module.
     * \ingroup group_int_osal
     * \param[in] name The name of the module.
     * \return The handle to the module.
     */
    static vx_module_handle_t loadModule(vx_char * name);

    /*! \brief Unloads a module.
     * \ingroup group_int_osal
     * \param[in] mod The handle to the module.
     */
    static void unloadModule(vx_module_handle_t mod);

    /*! \brief Gets a symbol from a module.
     * \ingroup group_int_osal
     * \param[in] mod The handle to the module.
     * \param[in] name The name of the symbol.
     * \return The symbol.
     */
    static vx_symbol_t getSymbol(vx_module_handle_t mod, const vx_char * name);
};

#endif /* VX_OSAL_H */
