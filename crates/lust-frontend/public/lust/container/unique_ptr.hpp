#pragma once

#include <type_traits>
#include <utility>

namespace lust
{
    template<typename T>
    class UniquePtr {
    public:
        UniquePtr() : ptr(nullptr) {}
        UniquePtr(std::nullptr_t) : ptr(nullptr) {}

        template <typename U = T>
        explicit UniquePtr(U* p) : ptr(p) {}

        UniquePtr(const UniquePtr&) = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        UniquePtr(UniquePtr&& moving) noexcept : ptr(moving.ptr) {
            moving.ptr = nullptr;
        }

        template <typename U>
        UniquePtr(UniquePtr<U>&& other) noexcept {
            ptr = other.ptr;
            other.ptr = nullptr;
        }

        template <typename U>
        UniquePtr<T>& operator=(UniquePtr<U>&& moving) noexcept {
            if ((void*)this != (void*)&moving) {
                delete ptr;
                ptr = moving.ptr;
                moving.ptr = nullptr;
            }
            return *this;
        }

        ~UniquePtr() {
            delete ptr;
        }

        T& operator*() const { return *ptr; }
        T* operator->() const { return ptr; }

        T* get() const { return ptr; }

        T* release() {
            T* temp = ptr;
            ptr = nullptr;
            return temp;
        }

        void reset(T* p = nullptr) {
            T* old = ptr;
            ptr = p;
            delete old;
        }

        bool is_null() const { return ptr == nullptr; }

        operator bool() const {
            return !is_null();
        }

        template <typename U>
        operator UniquePtr<typename std::enable_if<std::is_assignable<U, T>::value, U>::type>() {
            return UniquePtr<U>(release());
        }

        T* ptr;
    };

    template <typename T, typename... Args>
    UniquePtr<T> make_unique(Args&&... args) {
        return UniquePtr<T>(new T(std::forward<Args>(args)...));
    }

}
