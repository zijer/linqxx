#pragma once
#include <memory>   // std::shared_ptr
#include <optional> // std::optional
#include <iterator> // std::iterator_traits

#include "enumerable.hpp"
namespace linqxx
{
template <typename T>
class iterator
{
private:
    std::shared_ptr<enumerator<T>> source;
    std::optional<T> last;
    iterator();

public:
    using value_type = T;
    using iterator_category = std::input_iterator_tag;

    iterator(std::unique_ptr<enumerator<T>> source);

    iterator<T> operator++();
    bool operator==(iterator<T> other) const;
    bool operator!=(iterator<T> other) const;
    explicit operator bool() const;
    T operator*();

    static iterator<T> end();
};

template <typename T>
iterator<T>::iterator()
    : source(nullptr), last(std::nullopt){};

template <typename T>
iterator<T>::iterator(std::unique_ptr<enumerator<T>> source) 
    : source(std::move(source))
{
    last = std::optional<T>(this->source->next());
};

template <typename T>
iterator<T> iterator<T>::operator++()
{
    last = std::optional<T>(source->next());
    return *this;
};

template <typename T>
bool iterator<T>::operator==(iterator<T> other) const
{
    return (!last.has_value() && !other.last.has_value()) || source.get() == other.source.get();
};

template <typename T>
bool iterator<T>::operator!=(iterator<T> other) const
{
    return !(this->operator==(other));
};

template <typename T>
iterator<T>::operator bool() const
{
    return last.has_value();
};

template <typename T>
T iterator<T>::operator*()
{
    return last.value();
};

template <typename T>
iterator<T> iterator<T>::end()
{
    return iterator<T>();
};

template <typename T>
iterator<T> enumerable<T>::begin()
{
    return iterator<T>(enumerate());
};

template <typename T>
iterator<T> enumerable<T>::end()
{
    return iterator<T>::end();
}
} // namespace linqxx
