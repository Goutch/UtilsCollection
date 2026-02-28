#pragma once

#include <cstdint>
#include <vector>
#include "RawVector.h"
#include "Handle.h"

namespace HBE
{
    class HandleProvider
    {
        RawVector<Handle> handles;
        RawVector<uint32_t> free_handles_indices;

    public:
        HandleProvider() = default;

        HandleProvider(HandleProvider&& other)
        {
            free_handles_indices = std::move(other.free_handles_indices);
            handles = std::move(other.handles);
        }

        HandleProvider& operator=(HandleProvider& other) = delete;

        HandleProvider& operator=(HandleProvider&& other) noexcept
        {
            free_handles_indices = std::move(other.free_handles_indices);
            handles = std::move(other.handles);
            return *this;
        }

        void release(Handle handle)
        {
            if (!valid(handle))
                return;
            handles[handle.index].version++;
            free_handles_indices.push_back(handle.index);
        }

        bool valid(Handle handle)
        {
            return handles.size() > handle.index && handles[handle.index].version == handle.version;
        }

        size_t size()
        {
            return handles.size();
        }

        Handle create()
        {
            if (!free_handles_indices.empty())
            {
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
