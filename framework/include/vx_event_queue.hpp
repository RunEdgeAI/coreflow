/**
 * @file vx_event_queue.hpp
 * @brief
 * @version 0.1
 * @date 2025-05-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <chrono>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include <VX/vx_khr_pipelining.h>

/**
 * @brief Internal Event Queue Object
 *
 */
namespace corevx {

class EventQueue
{
    // Registration structure that stores additional event parameters for a vx_reference.
    struct RegistrationEntry
    {
        vx_reference ref;      // vx_reference being registered
        vx_event_type_e type;  // Event type associated with this registration
        vx_uint32 param;       // Optional extra parameter (e.g., graph parameter index)
        vx_uint32 app_value;   // Application-defined value to associate with events
    };

public:
    /**
     * @brief Construct a new Event Queue object
     *
     * @param enabled
     * @param max_size
     */
    explicit EventQueue(bool enabled = false, size_t max_size = 128)
        : enabled_(enabled), max_size_(max_size) {}


    /**
     * @brief Clear the event queue
     *
     */
    void clear()
    {
        std::unique_lock lock(mutex_);
        queue_.clear();
    }

    /**
     * @brief Update the event queue status
     *
     * @param status    true to enable, false to disable
     */
    vx_status status(bool status)
    {
        std::lock_guard lock(mutex_);
        enabled_ = status;
        cv_.notify_all();
        return VX_SUCCESS;
    }

    /**
     * @brief Check if the event queue is enabled
     *
     * @return true if enabled
     * @return false if disabled
     */
    bool isEnabled() const
    {
        return enabled_;
    }

    /**
     * @brief Push an event to the queue
     *
     * @param type Event type
     * @param data Optional event value
     * @param info Optional event info pointer
     * @param ref  Optional vx_reference used to look up additional registration data
     * @return true if successful
     * @return false if failed
     */
    vx_status push(vx_event_type_e type, vx_uint32 data = 0, vx_event_info_t* info = nullptr,
                   vx_reference ref = nullptr)
    {
        vx_event_t evt{};
        evt.type = type;
        evt.timestamp = getTimestamp();
        evt.app_value = data;
        if (info != nullptr)
        {
            evt.event_info = *info;
        }

        // If a reference is provided, look up additional registration info
        if (ref != nullptr)
        {
            const RegistrationEntry* reg = lookupRegistration(ref, type);
            if (reg && evt.type == reg->type)
            {
                // override the app_value with that in the registration.
                evt.app_value = reg->app_value;
            }
        }

        return push(evt);
    }

    /**
     * @brief Push an event to the queue
     *
     * @param event Event to push
     * @return true if successful
     * @return false if failed
     */
    vx_status push(vx_event_t event)
    {
        vx_status status = VX_SUCCESS;
        std::unique_lock lock(mutex_);

        if (!enabled_)
        {
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            if (queue_.size() >= max_size_)
            {
                queue_.pop_front();  // Drop the oldest event
            }
            queue_.emplace_back(std::move(event));
            cv_.notify_one();
        }

        return status;
    }

    vx_status wait(vx_event_t* event, vx_bool do_not_block)
    {
        vx_status status = VX_SUCCESS;
        std::optional<vx_event_t> evt;

        if (!enabled_)
        {
            status = VX_FAILURE;
        }

        if (VX_SUCCESS == status)
        {
            if (do_not_block)
            {
                evt = wait_and_pop(std::chrono::milliseconds(0));
            }
            else
            {
                evt = wait_and_pop();
            }

            if (std::nullopt == evt)
            {
                status = VX_FAILURE;
            }
        }

        if (VX_SUCCESS == status)
        {
            *event = std::move(*evt);
        }

        return status;
    }

    vx_status registerEvent(vx_reference ref, vx_event_type_e type, vx_uint32 param,
                           vx_uint32 app_value)
    {
        if (ref == nullptr) return VX_ERROR_INVALID_REFERENCE;
        std::unique_lock lock(mutex_);

        RegistrationEntry entry;
        entry.ref = ref;
        entry.type = type;
        entry.param = param;
        entry.app_value = app_value;

        registrations_.push_back(entry);
        return VX_SUCCESS;
    }

private : bool enabled_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<vx_event_t> queue_;
    size_t max_size_;
    static constexpr int timeout_ms_ = 10000;
    std::vector<RegistrationEntry> registrations_;

    /**
     * @brief Lookup a registration entry matching the given vx_reference and event type.
     *
     * @param ref The vx_reference to look up.
     * @param type The event type.
     * @return const RegistrationEntry* Pointer to the matching entry, or nullptr if not found.
     */
    const RegistrationEntry* lookupRegistration(vx_reference ref, vx_event_type_e type) const
    {
        for (const auto& entry : registrations_)
        {
            if (entry.ref == ref && entry.type == type)
            {
                return &entry;
            }
        }

        return nullptr;
    }

    /**
     * @brief Wait for an event and pop it from the queue
     *
     * @param timeout Timeout duration
     * @return std::optional<vx_event_t> Event if available, otherwise std::nullopt
     */
    std::optional<vx_event_t> wait_and_pop(
        std::chrono::milliseconds timeout = std::chrono::milliseconds(timeout_ms_))
    {
        std::unique_lock lock(mutex_);
        cv_.wait_for(lock, timeout, [this] { return !queue_.empty(); });
        if (queue_.empty())
        {
            return std::nullopt;  // Timeout
        }

        vx_event_t evt = std::move(queue_.front());
        queue_.pop_front();
        return evt;
    }

    /**
     * @brief Get the current timestamp in nanoseconds
     *
     * @return vx_uint64 Current timestamp
     */
    vx_uint64 getTimestamp() const
    {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    }
};

} // namespace corevx
