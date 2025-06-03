/**
 * @file execution_queue.hpp
 * @brief Execution queue implementation
 * @version 0.1
 * @date 2025-05-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <condition_variable>
#include <mutex>

#include "circular_queue.hpp"

template<typename T, std::size_t MaxDepth>
class ExecutionQueue
{
public:
    /**
     * @brief Enqueue an item into the "pending" queue
     *
     * @param item The item to enqueue
     * @return true if successful, false if the queue is full
     */
    bool enqueuePending(const T& item)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return pending_queue_.enqueue(item);
    }

    /**
     * @brief Dequeue an item from the "pending" queue
     *
     * @param item Reference to store the dequeued item
     * @return true if successful, false if the queue is empty
     */
    bool dequeuePending(T& item)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return pending_queue_.dequeue(item);
    }

    /**
     * @brief Move an item from the "pending" queue to the "ready" queue
     *
     * @return true if successful, false if the "pending" queue is empty or the "ready" queue is
     * full
     */
    bool movePendingToReady()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        T item;
        if (!pending_queue_.dequeue(item))
        {
            return false;  // "Pending" queue is empty
        }
        if (!ready_queue_.enqueue(item))
        {
            return false;  // "Ready" queue is full
        }
        return true;
    }

    /**
     * @brief Enqueue an item into the "ready" queue
     *
     * @param item The item to enqueue
     * @return true if successful, false if the queue is full
     */
    bool enqueueReady(const T& item)
    {
        std::unique_lock<std::mutex> lock(mutex_);
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
        std::unique_lock<std::mutex> lock(mutex_);
        return ready_queue_.dequeue(item);
    }

    /**
     * @brief Peek at the item at the front of the "ready" queue without removing it
     *
     * @param item Reference to store the peeked item
     * @return true if successful, false if the queue is empty
     */
    bool peekReady(T& item) const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return ready_queue_.peek(item);
    }

    /**
     * @brief Enqueue an item into the "done" queue after processing
     *
     * @param item The item to enqueue
     * @return true if successful, false if the queue is full
     */
    bool enqueueDone(const T& item)
    {
        std::unique_lock<std::mutex> lock(mutex_);
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
        std::unique_lock<std::mutex> lock(mutex_);
        return done_queue_.dequeue(item);
    }

    /**
     * @brief Block until at least one item is available in the "done" queue
     */
    void waitForDoneRef()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait_for(lock, std::chrono::milliseconds(timeout_ms_),
                           [this]() { return !done_queue_.empty(); });
    }

    /**
     * @brief Move an item from the "ready" queue to the "done" queue
     *
     * @return true if successful, false if the "ready" queue is empty or the "done" queue is full
     */
    bool moveReadyToDone()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        T item;
        if (!ready_queue_.dequeue(item))
        {
            return false;  // "Ready" queue is empty
        }
        if (!done_queue_.enqueue(item))
        {
            return false;  // "Done" queue is full
        }
        cond_var_.notify_one();  // Notify that an item is available in the "done" queue
        return true;
    }

    /**
     * @brief Check if the "pending" queue is empty
     *
     * @return true if empty, false otherwise
     */
    bool isPendingEmpty() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return pending_queue_.empty();
    }

    /**
     * @brief Check if the "ready" queue is empty
     *
     * @return true if empty, false otherwise
     */
    bool isReadyEmpty() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return ready_queue_.empty();
    }

    /**
     * @brief Check if the "done" queue is empty
     *
     * @return true if empty, false otherwise
     */
    bool isDoneEmpty() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return done_queue_.empty();
    }

    /**
     * @brief Get the size of the "pending" queue
     *
     * @return std::size_t Size of the "pending" queue
     */
    std::size_t pendingQueueSize() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return pending_queue_.size();
    }

    /**
     * @brief Get the size of the "ready" queue
     *
     * @return std::size_t Size of the "ready" queue
     */
    std::size_t readyQueueSize() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return ready_queue_.size();
    }

    /**
     * @brief Get the size of the "done" queue
     *
     * @return std::size_t Size of the "done" queue
     */
    std::size_t doneQueueSize() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return done_queue_.size();
    }
private:
    static constexpr int timeout_ms_ = 10000;
    CircularQueue<T, MaxDepth> pending_queue_;
    CircularQueue<T, MaxDepth> ready_queue_;
    CircularQueue<T, MaxDepth> done_queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_var_;
};