#pragma once

#include <vector>
#include <algorithm>
#include <cstdio>
#include "functional"
#include "data-structure/StableHandleContainer.h"

namespace HBE {
	using event_subscription_id = Handle;

	// Listeners run by priority, lowest first, then in subscription order. A listener may subscribe, unsubscribe and invoke the same
	// event from its callback: subscriptions made during an invoke run from the next invoke, an unsubscribed listener is not called again.
	template<typename... Args>
	class Event {
		struct Listener {
			event_subscription_id id{};
			std::function<void(Args...)> callback;
			int priority = 0;
			bool unsubscribed = false;
		};

		HandleProvider provider;
		std::vector<Listener> listeners; // sorted by priority, then subscription order
		std::vector<Listener> listeners_subscribed_while_invoking;
		uint32_t invoke_depth = 0;
		bool listeners_unsubscribed_while_invoking = false;
		bool consumed = false;

		void insertByPriority(Listener &&listener) {
			auto after_same_priority = std::upper_bound(listeners.begin(), listeners.end(), listener.priority,
			                                            [](int priority, const Listener &other) { return priority < other.priority; });
			listeners.insert(after_same_priority, std::move(listener));
		}

		void applyChangesMadeWhileInvoking() {
			if (listeners_unsubscribed_while_invoking) {
				std::erase_if(listeners, [](const Listener &listener) { return listener.unsubscribed; });
				listeners_unsubscribed_while_invoking = false;
			}
			for (Listener &listener: listeners_subscribed_while_invoking) {
				insertByPriority(std::move(listener));
			}
			listeners_subscribed_while_invoking.clear();
		}

	public:
		Event() = default;

		Event(const Event &) = delete;

		Event &operator=(const Event &) = delete;

		Event(Event &&other) noexcept
			: provider(std::move(other.provider)), listeners(std::move(other.listeners)) {
		}

		Event &operator=(Event &&other) noexcept {
			provider = std::move(other.provider);
			listeners = std::move(other.listeners);
			return *this;
		}

		void clear() {
			if (invoke_depth > 0) {
				for (Listener &listener: listeners) {
					listener.unsubscribed = true;
				}
				listeners_unsubscribed_while_invoking = true;
			} else {
				listeners.clear();
			}
			listeners_subscribed_while_invoking.clear();
			provider.clear();
		}

		void subscribe(event_subscription_id &id, std::function<void(Args...)> function, int priority) {
			id = provider.create();
			Listener listener{id, std::move(function), priority};
			if (invoke_depth > 0) {
				listeners_subscribed_while_invoking.push_back(std::move(listener));
			} else {
				insertByPriority(std::move(listener));
			}
		}

		void subscribe(event_subscription_id &id, void (*fn)(Args...), int priority = 0) {
			subscribe(id, [fn](Args... args) { fn(args...); }, priority);
		}

		template<typename T>
		void subscribe(event_subscription_id &id, T *instance, void (T::*method)(Args...), int priority = 0) {
			subscribe(id, [instance, method](Args... args) { (instance->*method)(args...); }, priority);
		}

		// Same subscription and place in the order, another callback: for an object moved to a new address.
		void replaceCallback(event_subscription_id id, std::function<void(Args...)> function) {
			for (std::vector<Listener> *list: {&listeners, &listeners_subscribed_while_invoking}) {
				for (Listener &listener: *list) {
					if (listener.id == id) {
						listener.callback = std::move(function);
						return;
					}
				}
			}
		}

		template<typename T>
		void replaceCallback(event_subscription_id id, T *instance, void (T::*method)(Args...)) {
			replaceCallback(id, [instance, method](Args... args) { (instance->*method)(args...); });
		}

		bool valid(event_subscription_id id) {
			return provider.valid(id);
		}

		void unsubscribe(event_subscription_id id) {
			if (!provider.valid(id)) {
				printf("Trying to unsubscribe from an event with invalid subscription id\n");
				return;
			}
			provider.release(id);
			if (invoke_depth == 0) {
				std::erase_if(listeners, [id](const Listener &listener) { return listener.id == id; });
				return;
			}
			for (Listener &listener: listeners) {
				if (listener.id == id) {
					listener.unsubscribed = true; // erased when the invoke ends: the loop may be running its callback
					listeners_unsubscribed_while_invoking = true;
					return;
				}
			}
			std::erase_if(listeners_subscribed_while_invoking, [id](const Listener &listener) { return listener.id == id; });
		}

		// Stops the invoke running now after the current listener.
		void consume() {
			consumed = true;
		}

		// Every listener gets the same arguments as lvalues: none can move them away from the next ones.
		template<typename... CallArgs>
		void invoke(CallArgs &&... args) {
			const bool outer_invoke_consumed = consumed;
			consumed = false;
			invoke_depth++;
			for (size_t listener_index = 0; listener_index < listeners.size() && !consumed; ++listener_index) {
				if (!listeners[listener_index].unsubscribed) {
					listeners[listener_index].callback(args...);
				}
			}
			invoke_depth--;
			consumed = outer_invoke_consumed;
			if (invoke_depth == 0) {
				applyChangesMadeWhileInvoking();
			}
		}

		uint32_t listenerCount() const {
			uint32_t count = static_cast<uint32_t>(listeners_subscribed_while_invoking.size());
			for (const Listener &listener: listeners) {
				if (!listener.unsubscribed) count++;
			}
			return count;
		}
	};
}
