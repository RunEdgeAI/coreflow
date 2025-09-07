/**
 * @file circular_queue.hpp
 * @brief Circular queue implementation
 * @version 0.1
 * @date 2025-05-13
 *
 * @copyright Copyright (c) 2025
 * @ingroup group_corevx_ext
 */
#include <atomic>
#include <array>
#include <cstddef>

/**
 * @brief Circular queue implementation
 *
 * @tparam T Type of elements in the queue
 * @tparam MaxDepth Maximum depth of the queue
 * @ingroup group_corevx_ext
 */
template<typename T, std::size_t MaxDepth>
class CircularQueue
{
public:
    /**
     * @brief Construct a new Circular Queue object
     * @see group_corevx_ext
     */
    CircularQueue() : head_(0), tail_(0) {}

    /**
     * @brief Enqueue an element into the queue
     *
     * @param ref Element to enqueue
     * @return true if successful, false if the queue is full
     */
    bool enqueue(const T& ref)
    {
        size_t tail = tail_.load(std::memory_order_relaxed);
        size_t next_tail = (tail + 1) % MaxDepth;
        if (next_tail == head_.load(std::memory_order_acquire))
        {
            return false; // full
        }
        buffer_[tail] = ref;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    /**
     * @brief Dequeue an element from the queue
     *
     * @param out Reference to store the dequeued element
     * @return true if successful, false if the queue is empty
     */
    bool dequeue(T& out)
    {
        size_t head = head_.load(std::memory_order_relaxed);
        if (head == tail_.load(std::memory_order_acquire))
        {
            return false; // empty
        }
        out = buffer_[head];
        head_.store((head + 1) % MaxDepth, std::memory_order_release);
        return true;
    }

    /**
     * @brief Peek at the element at the front of the queue without removing it
     *
     * @param out Reference to store the peeked element
     * @return true if successful, false if the queue is empty
     */
    bool peek(T& out) const
    {
        size_t head = head_.load(std::memory_order_relaxed);
        if (head == tail_.load(std::memory_order_acquire))
        {
            return false;  // empty
        }
        out = buffer_[head];
        return true;
    }

    /**
     * @brief Get the size of the queue
     *
     * @return std::size_t Size of the queue
     */
    std::size_t size() const
    {
        size_t head = head_.load(std::memory_order_acquire);
        size_t tail = tail_.load(std::memory_order_acquire);
        return (tail + MaxDepth - head) % MaxDepth;
    }

    /**
     * @brief Check if the queue is empty
     *
     * @return true if empty, false otherwise
     */
    bool empty() const { return size() == 0; }

    /**
     * @brief Check if the queue is full
     *
     * @return true if full, false otherwise
     */
    bool full() const { return ((tail_.load(std::memory_order_acquire) + 1) % MaxDepth) == \
        head_.load(std::memory_order_acquire); }

private:
    std::array<T, MaxDepth> buffer_;
    std::atomic<size_t> head_, tail_;
};