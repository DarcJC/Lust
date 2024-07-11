#pragma once

#include <cstddef>

namespace lust
{
    template <typename T>
    class LUSTFRONTEND_API vector {
    public:
        // 构造函数和析构函数
        vector();
        vector(const vector& other);
        vector(vector&& other) noexcept;
        vector& operator=(const vector& other);
        vector& operator=(vector&& other) noexcept;
        ~vector();

        // 容量相关
        bool empty() const;
        size_t size() const;
        void resize(size_t count);
        void reserve(size_t new_cap);
        size_t capacity() const;
        void clear();

        // 元素访问
        T& operator[](size_t pos);
        const T& operator[](size_t pos) const;
        T& at(size_t pos);
        const T& at(size_t pos) const;
        T& front();
        const T& front() const;
        T& back();
        const T& back() const;

        // 修改器
        void push_back(const T& value);
        void push_back(T&& value);
        void pop_back();
        template <typename... Args>
        void emplace_back(Args&&... args);
        void extend(vector&& other);

        // 迭代器
        const T* begin() const;
        const T* end() const;         
        T* begin();
        T* end();

        // 工具函数
        vector<T> slice(size_t begin_pos, size_t end_pos) const;

    private:
        class Impl;  // 声明一个私有的实现类
        Impl* pimpl; // 实现指针
    };
}
