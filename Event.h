#pragma once

#include <vector>
#include <deque>
#include <algorithm>
#include "unordered_set"
#include "functional"
#include "unordered_map"
#include "data-structure/StableHandleContainer.h"

namespace HBE
{
    using event_subscription_id = Handle;

    template <typename... Args>
    class Event
    {
        struct Listener
        {
            event_subscription_id id;
            int priority;
            std::function<void(Args...)> callback;
        };

        StableHandleContainer<Listener, 4> container;
        RawVector<Listener> sorted_listeners;
        size_t next_id = 0;
        uint32_t invoke_index = 0;

    protected:
        bool consumed;

    public:
        Event() = default;

        Event(const Event&) = delete;

        Event& operator=(const Event&) = delete;

        Event(Event&& other) noexcept
        {
            container = std::move(other.container);
            sorted_listeners = std::move(other.sorted_listeners);
        }

        Event& operator=(Event&& other) noexcept
        {
            container = std::move(other.container);
            sorted_listeners = std::move(other.sorted_listeners);
            return *this;
        }

        void subscribe(event_subscription_id& id, std::function<void(Args...)> function, int priority)
        {
            id = container.create();
            container.set(id, Listener{id, priority, std::move(function)});
            sorted_listeners.push_back(container.get(id));
            std::stable_sort(sorted_listeners.begin(), sorted_listeners.end(),
                             [](auto& a, auto& b) { return a.priority < b.priority; });
        }

        bool valid(event_subscription_id id)
        {
            return container.valid(id);
        }

        void subscribe(event_subscription_id& id, void (*fn)(Args...), int priority = 0)
        {
            subscribe(id,
                      [fn](Args... args) { fn(args...); },
                      priority
            );
        }

        template <typename T>
        void subscribe(event_subscription_id& id, T* instance, void (T::*method)(Args...), int priority = 0)
        {
            subscribe(id,
                      [instance, method](Args... args)
                      {
                          (instance->*method)(args...);
                      },
                      priority
            );
        }

        void unsubscribe(event_subscription_id id)
        {
            if (container.valid(id))
            {
                container.release(id);
                for (int i = sorted_listeners.size() - 1; i >= 0; --i)
                {
                    if (sorted_listeners[i].id == id)
                    {
                        sorted_listeners.erase(sorted_listeners.begin() + i);
                        //make sure that deletion of the listener does not affect the invoke loop
                        if (invoke_index >= i)
                        {
                            invoke_index--;
                        }
                        break;
                    }
                }
            }
            else
            {
                printf("Trying to unsubscribe from an event with invalid subscription id ");
            }
        }

        void consume()
        {
            consumed = true;
        }

        template <typename... CallArgs>
        void invoke(CallArgs&&... args)
        {
            consumed = false;
            for (invoke_index = 0; invoke_index < sorted_listeners.size(); ++invoke_index)
            {
                if (consumed)
                    break;
                sorted_listeners[invoke_index].callback(std::forward<CallArgs>(args)...);
            }
        }

        uint32_t listenerCount() const
        {
            return sorted_listeners.size();
        }
    };
}
