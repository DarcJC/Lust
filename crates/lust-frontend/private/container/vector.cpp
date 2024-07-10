#include "container/vector.hpp"
#include <vector>
#include <utility> // for std::move
#include <stdexcept> // for std::out_of_range
#include <type_traits>

#include "container/simple_string.hpp"
#include "container/unique_ptr.hpp"
#include "grammar.hpp"

namespace lust {
    template <typename T>
    class vector<T>::Impl {
    public:
        std::vector<T> data;
    };

    template <typename T>
    vector<T>::vector() : pimpl(new Impl()) {}

    template <typename T>
    vector<T>::vector(const vector<T>& other)  {
        if constexpr (std::is_copy_constructible<T>::value) {
            pimpl = new Impl(*other.pimpl);
        }
    }

    template <typename T>
    vector<T>::vector(vector<T>&& other) noexcept : pimpl(other.pimpl) {
        other.pimpl = nullptr;
    }

    template <typename T>
    vector<T>& vector<T>::operator=(const vector<T>& other) {
        if constexpr (std::is_copy_assignable<T>::value) {
            if (this != &other) {  // 防止自赋值
                *pimpl = *other.pimpl;
            }
        }
        return *this;
    }

    template <typename T>
    vector<T>& vector<T>::operator=(vector<T>&& other) noexcept {
        std::swap(pimpl, other.pimpl);
        return *this;
    }

    template <typename T>
    vector<T>::~vector() {
        delete pimpl;
    }

    template <typename T>
    bool vector<T>::empty() const {
        return pimpl->data.empty();
    }

    template <typename T>
    size_t vector<T>::size() const {
        return pimpl->data.size();
    }

    template <typename T>
    void vector<T>::resize(size_t count) {
        pimpl->data.resize(count);
    }

    template <typename T>
    void vector<T>::reserve(size_t new_cap) {
        pimpl->data.reserve(new_cap);
    }

    template <typename T>
    size_t vector<T>::capacity() const {
        return pimpl->data.capacity();
    }

    template <typename T>
    void vector<T>::clear() {
        pimpl->data.clear();
    }

    template <typename T>
    T& vector<T>::operator[](size_t pos) {
        return pimpl->data[pos];
    }

    template <typename T>
    const T& vector<T>::operator[](size_t pos) const {
        return pimpl->data[pos];
    }

    template <typename T>
    T& vector<T>::at(size_t pos) {
        return pimpl->data.at(pos);  // at() 会检查边界并可能抛出 std::out_of_range 异常
    }

    template <typename T>
    const T& vector<T>::at(size_t pos) const {
        return pimpl->data.at(pos);
    }

    template <typename T>
    T& vector<T>::front() {
        return pimpl->data.front();
    }

    template <typename T>
    const T& vector<T>::front() const {
        return pimpl->data.front();
    }

    template <typename T>
    T& vector<T>::back() {
        return pimpl->data.back();
    }

    template <typename T>
    const T& vector<T>::back() const {
        return pimpl->data.back();
    }

    template <typename T>
    void vector<T>::push_back(const T& value) {
        if constexpr (std::is_copy_constructible<T>::value) {
            pimpl->data.push_back(value);
        }
    }

    template <typename T>
    void vector<T>::push_back(T&& value) {
        pimpl->data.push_back(std::move(value));
    }

    template <typename T>
    void vector<T>::pop_back() {
        pimpl->data.pop_back();
    }

    template <typename T>
    void vector<T>::extend(vector &&other) {
        for (int32_t i = 0; i < other.size(); ++i) {
            push_back(std::move(other.at(i)));
        }
    }

    template <typename T>
    template <typename... Args>
    void vector<T>::emplace_back(Args &&...args)
    {
        pimpl->data.emplace_back(std::forward<Args>(args)...);
    }

    // 显式实例化模板
    template class vector<int>;
    template class vector<double>;
    template class vector<simple_string>;
    template class vector<UniquePtr<grammar::ASTNode_Statement>>;
    template class vector<UniquePtr<grammar::ASTNode_Attribute>>;
    // 添加其他需要支持的类型

} // namespace lust