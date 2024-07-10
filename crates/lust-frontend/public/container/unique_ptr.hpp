#pragma once

#include <type_traits>

namespace lust
{
    template<typename T>
    class UniquePtr {
    private:
        T* ptr;

    public:
        UniquePtr(std::nullptr_t) : ptr(nullptr) {}

        template <typename U = T>
        explicit UniquePtr(U* p = nullptr) : ptr(p) {}

        UniquePtr(const UniquePtr&) = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        UniquePtr(UniquePtr&& moving) noexcept : ptr(moving.ptr) {
            moving.ptr = nullptr;
        }

        UniquePtr& operator=(UniquePtr&& moving) noexcept {
            if (this != &moving) {
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

        operator bool() {
            return is_null();
        }

        template <typename U, typename = std::enable_if<std::is_assignable<U, T>::value>::type>
        operator UniquePtr<U>() {
            return UniquePtr<U>(release());
        }
    };

    template <typename T, typename... Args>
    UniquePtr<T> make_unique(Args&&... args) {
        return UniquePtr<T>(new T(std::forward<Args>(args)...));
    }

}
