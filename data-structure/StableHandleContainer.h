#pragma once
#include "StableVector.h"
#include "HandleProvider.h"

namespace HBE
{
    template <typename T, size_t PAGE_SIZE>
    class StableHandleContainer
    {
        StableVector<T, PAGE_SIZE> m_stable_data;
        HandleProvider handle_provider;

    public:
        StableHandleContainer() = default;
        ~StableHandleContainer() = default;

        Handle create()
        {
            Handle handle = handle_provider.create();
            m_stable_data.resize(handle_provider.size());
            return handle;
        }

        T& get(const Handle& handle)
        {
            return m_stable_data[handle_provider.index(handle)];
        }

        void release(const Handle& handle)
        {
            m_stable_data.erase(handle_provider.index(handle));
        }

        T& operator[](Handle handle)
        {
            return get(handle);
        }

        bool valid(const Handle& handle)
        {
            return handle_provider.valid(handle);
        }
    };
}
