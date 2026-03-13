#pragma once

#include <cstdint>
#include <vector>
#include "RawVector.h"
#include "Handle.h"

namespace HBE {
	class HandleProvider {
		RawVector<Handle> handles;
		RawVector<uint32_t> free_handles_indices;
		static const uint64_t VERSION_INCREMENT = 1ULL << 32;

	public:
		HandleProvider() = default;

		HandleProvider(HandleProvider &&other) {
			free_handles_indices = std::move(other.free_handles_indices);
			handles = std::move(other.handles);
		}

		HandleProvider &operator=(HandleProvider &other) = delete;

		HandleProvider &operator=(HandleProvider &&other) noexcept {
			free_handles_indices = std::move(other.free_handles_indices);
			handles = std::move(other.handles);
			return *this;
		}

		void clear() {
			handles.clear();
			free_handles_indices.clear();
		}

		static inline uint32_t index(Handle handle) {
			return handle & 0xFFFFFFFF;
		}

		inline uint32_t version(Handle handle) {
			return (handle >> 32) & 0xFFFFFFFF;
		}

		void release(Handle handle) {
			if (!valid(handle))
				return;
			handles[index(handle)] += VERSION_INCREMENT;
			free_handles_indices.push_back(index(handle));
		}

		bool valid(Handle handle) {
			uint32_t index = this->index(handle);
			return handles.size() > index && version(handles[index]) == version(handle);
		}

		size_t size() {
			return handles.size();
		}

		Handle create() {
			if (!free_handles_indices.empty()) {
				Handle handle = handles[free_handles_indices[free_handles_indices.size() - 1]];
				free_handles_indices.resize(free_handles_indices.size() - 1);
				return handle;
			}
			Handle handle = 0;
			handle = static_cast<uint64_t>(handles.size());
			handles.push_back(handle);
			return handle;
		}
	};
}
