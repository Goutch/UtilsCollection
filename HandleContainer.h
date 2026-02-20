
#pragma once

#include <cstdint>
#include <vector>
#include "RawVector.h"

namespace HBE {
	struct Handle {
		uint32_t index = std::numeric_limits<uint32_t>::max();
		uint32_t version = std::numeric_limits<uint32_t>::max();

		bool operator==(Handle other) const {
			return index == other.index && version == other.version;
		}
	};

	template<typename T>
	class HandleContainer {
		RawVector<Handle> handles;
		std::vector<T> data;
		RawVector<uint32_t> free_handles_indices;
	public:
		HandleContainer() = default;
		HandleContainer(HandleContainer<T> &&other) {
			free_handles_indices = std::move(other.free_handles_indices);
			handles = std::move(other.handles);
			data = std::move(other.data);
		}
		HandleContainer &operator=(HandleContainer &other) = delete;
		HandleContainer &operator=(HandleContainer &&other) noexcept {
			free_handles_indices = std::move(other.free_handles_indices);
			handles = std::move(other.handles);
			data = std::move(other.data);
			return *this;
		}

		void release(Handle handle) {
			if (!valid(handle))
				return;
			handles[handle.index].version++;
			free_handles_indices.push_back(handle.index);
		}

		bool valid(Handle handle) {
			return handles.size() <= handle.index && handles[handle.index].version == handle.version;
		}

		size_t size() {
			return data.size();
		}

		T get(Handle handle) {
			return data[handle.index];
		}

		void set(Handle handle, T t) {
			if (!valid(handle))
				return;
			data[handle.index] = t;
		}

		Handle create() {
			if (!free_handles_indices.empty()) {
				Handle handle = handles[free_handles_indices[free_handles_indices.size() - 1]];
				free_handles_indices.resize(free_handles_indices.size() - 1);
				return handle;
			}
			Handle handle{};
			handle.index = handles.size();
			handle.version = 0;
			handles.push_back(handle);
			return handle;
		}
	};
}
