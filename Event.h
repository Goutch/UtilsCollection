#pragma once

#include <vector>
#include <algorithm>
#include "functional"
#include "data-structure/StableHandleContainer.h"

namespace HBE
{
    using event_subscription_id = Handle;

    template <typename... Args>
    class Event
    {
        struct Listener
        {
            event_subscription_id id{};
            std::function<void(Args...)> callback;
            int priority = 0;

            Listener() = default;

            Listener(const Listener& other)
                : id(other.id), callback(other.callback), priority(other.priority)
            {
            }

            Listener& operator=(const Listener& other)
            {
                id = other.id;
                callback = other.callback;
                priority = other.priority;
                return *this;
            }

            Listener(Listener&& other) noexcept
                : id(other.id), callback(std::move(other.callback)), priority(other.priority)
            {
                other.id = {};
                other.priority = 0;
            }

            Listener& operator=(Listener&& other) noexcept
            {
                id = other.id;
                callback = std::move(other.callback);
                priority = other.priority;

                other.id = {};
                other.priority = 0;
                return *this;
            }

            void reset()
            {
                id = {};
                callback = {};
                priority = 0;
            }
        };

        HandleProvider provider;
        std::vector<Listener> listeners;
        std::vector<Listener> sorted_listeners;
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
            listeners = std::move(other.listeners);
            sorted_listeners = std::move(other.sorted_listeners);
        }

        Event& operator=(Event&& other) noexcept
        {
            listeners = std::move(other.listeners);
            sorted_listeners = std::move(other.sorted_listeners);
            return *this;
        }

        void subscribe(event_subscription_id& id, std::function<void(Args...)> function, int priority)
        {
            id = provider.create();
            listeners.resize(provider.size());
            listeners[id.index].priority = priority;
            listeners[id.index].callback = std::move(function);
            listeners[id.index].id = id;

            sorted_listeners.push_back(listeners.at(id.index));
            std::stable_sort(sorted_listeners.begin(), sorted_listeners.end(),
                             [](auto& a, auto& b) { return a.priority < b.priority; });
        }

        bool valid(event_subscription_id id)
        {
            return provider.valid(id);
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
            subscribe(id, [instance, method](Args... args)
                      {
                          (instance->*method)(args...);
                      },
                      priority
            );
        }

        void unsubscribe(event_subscription_id id)
        {
            if (provider.valid(id))
            {
                listeners[id.index].reset();
                provider.release(id);
                listeners.resize(provider.size());
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
