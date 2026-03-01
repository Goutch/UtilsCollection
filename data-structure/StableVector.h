#pragma once
#include "RawVector.h"

namespace HBE
{
    template <typename T, size_t PAGE_SIZE>
    class StableVector
    {
        static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
        static_assert(PAGE_SIZE > 0, "Page size must be greater than 0");
        static_assert(PAGE_SIZE > 0 && (PAGE_SIZE & (PAGE_SIZE - 1)) == 0, "Page size must be power of 2");

        constexpr size_t log2_constexpr(size_t n, size_t count = 0)
        {
            return (n <= 1) ? count : log2_constexpr(n >> 1, count + 1);
        }

        static constexpr size_t SHIFT = log2_constexpr(PAGE_SIZE);
        static constexpr size_t MASK = PAGE_SIZE - 1;

        struct Page
        {
            T data[PAGE_SIZE];
        };

        RawVector<Page*> pages;
        size_t m_size = 0;

    private :
        inline size_t getPageIndex(size_t i)
        {
            return i >> SHIFT;
        }

        inline size_t getLocalIndex(size_t i)
        {
            return i & MASK;
        }

    public:
        void clear()
        {
            for (int i = 0; i < pages.size(); i++)
            {
                delete pages[i];
            }
            pages.clear();
            m_size = 0;
        }

        StableVector() = default;

        ~StableVector()
        {
            clear();
        }

        StableVector(StableVector&& Other) noexcept
        {
            pages = std::move(Other.pages);
            Other.clear();
        }

        StableVector(const StableVector& other) = delete;
        StableVector& operator=(const StableVector& other) = delete;

        void push_back(const T& item)
        {
            reserve(m_size + 1);
            (*this)[m_size] = item;
            m_size++;
        }

        void emplace_back(const T& item)
        {
            push_back(item);
        }

        void erase(size_t i)
        {
            if (i >= m_size)
                return;
            if (i == size - 1)
            {
                m_size--;
            }
            (*this)[i] = {};
        }

        void reserve(size_t new_capacity)
        {
            if (new_capacity <= capacity())
                return;

            size_t required_pages = getPageIndex(new_capacity) + 1;

            while (pages.size() < required_pages)
                pages.push_back(new Page());
        }

        void resize(size_t new_size)
        {
            if (new_size > m_size)
            {
                reserve(new_size);

                // zero initialize new elements (since trivially copyable)
                size_t old_size = m_size;
                m_size = new_size;

                for (size_t i = old_size; i < new_size; ++i)
                    (*this)[i] = T{};
            }
            else
            {
                // shrinking only changes logical size (stable storage)
                m_size = new_size;
            }
        }

        size_t size()
        {
            return m_size;
        }

        T* data()
        {
            if (size() > 0)
                return pages[0].data;
            else
                return nullptr;
        }

        size_t capacity() const
        {
            return pages.size() * PAGE_SIZE;
        }

        T& operator[](size_t i)
        {
            const size_t page_index = getPageIndex(i);
            const size_t local_index = getLocalIndex(i);
            return pages[page_index]->data[local_index];
        }

        const T& operator[](size_t i) const
        {
            const size_t page_index = getPageIndex(i);
            const size_t local_index = getLocalIndex(i);
            return pages[page_index]->data[local_index];
        }
    };
}
