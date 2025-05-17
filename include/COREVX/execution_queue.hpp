/**
 * @file execution_queue.hpp
 * @brief Execution queue implementation
 * @version 0.1
 * @date 2025-05-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "circular_queue.hpp"

template<typename T, std::size_t MaxDepth>
class ExecutionQueue
{
public:
    /**
     * @brief Enqueue an item into the "ready" queue
     *
     * @param item The item to enqueue
     * @return true if successful, false if the queue is full
     */
    bool enqueueReady(const T& item)
    {
        return ready_queue_.enqueue(item);
    }

    /**
     * @brief Dequeue an item from the "ready" queue for processing
     *
     * @param item Reference to store the dequeued item
     * @return true if successful, false if the queue is empty
     */
    bool dequeueReady(T& item)
    {
        return ready_queue_.dequeue(item);
    }

    /**
     * @brief Enqueue an item into the "done" queue after processing
     *
     * @param item The item to enqueue
     * @return true if successful, false if the queue is full
     */
    bool enqueueDone(const T& item)
    {
        bool ans = done_queue_.enqueue(item);
        cond_var_.notify_one();
        return ans;
    }

    /**
     * @brief Dequeue an item from the "done" queue
     *
     * @param item Reference to store the dequeued item
     * @return true if successful, false if the queue is empty
     */
    bool dequeueDone(T& item)
    {
        return done_queue_.dequeue(item);
    }

    /**
     * @brief Block until at least one item is available in the "done" queue
     */
    void waitForDoneRef()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [this]() { return !done_queue_.empty(); });
    }

    /**
     * @brief Check if the "ready" queue is empty
     *
     * @return true if empty, false otherwise
     */
    bool isReadyEmpty() const
    {
        return ready_queue_.empty();
    }

    /**
     * @brief Check if the "done" queue is empty
     *
     * @return true if empty, false otherwise
     */
    bool isDoneEmpty() const
    {
        return done_queue_.empty();
    }

    /**
     * @brief Get the size of the "ready" queue
     *
     * @return std::size_t Size of the "ready" queue
     */
    std::size_t readyQueueSize() const
    {
        return ready_queue_.size();
    }

    /**
     * @brief Get the size of the "done" queue
     *
     * @return std::size_t Size of the "done" queue
     */
    std::size_t doneQueueSize() const
    {
        return done_queue_.size();
    }
private:
    CircularQueue<T, MaxDepth> ready_queue_;
    CircularQueue<T, MaxDepth> done_queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_var_;
};