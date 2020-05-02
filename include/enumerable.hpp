#pragma once

#include <memory>   // std::shared_ptr
#include <optional> // std::optional
#include <utility>  // std::declval
#include <vector>   // std::vector

namespace linqxx
{
template <typename T>
class enumerator;
template <typename T>
class iterator
{
private:
    std::shared_ptr<enumerator<T>> source; 
    std::optional<T> last;
    iterator() : source(nullptr), last(std::nullopt){};

public:
    static iterator<T> end()
    {
        return iterator<T>();
    };
    iterator(std::unique_ptr<enumerator<T>> source) : source(std::move(source))
    {
        last = std::optional<T>(this->source->next());
    };
    iterator<T> operator++()
    {
        last = std::optional<T>(source->next());
        return *this;
    };
    bool operator==(iterator<T> other) const
    {
        return (!last.has_value() && !other.last.has_value()) || source.get() == other.source.get();
    };
    bool operator!=(iterator<T> other) const
    {
        return !(this->operator==(other));
    };
    T operator*()
    { 
        return last.value(); 
    }
    using value_type = T;
    using iterator_category = std::input_iterator_tag;
};

template <typename T>
class enumerator
{
public:
    virtual std::optional<T> next() = 0;
    virtual ~enumerator(){};
};

template <typename T, typename TKey>
class grouping_enumerable;

template <typename T>
class enumerable
{
public:
    using value_type = T;
    virtual std::unique_ptr<enumerator<T>> enumerate() = 0;

    template <typename TF>
    std::shared_ptr<enumerable<T>> where(TF pred);

    template <typename TF, typename TResult = typename std::invoke_result<TF, const T &>::type>
    std::shared_ptr<enumerable<TResult>> select(TF selector);

    template <typename TF, typename TKey = typename std::invoke_result<TF, const T &>::type, class Hash = std::hash<TKey>, class Pred = std::equal_to<TKey>>
    std::shared_ptr<enumerable<std::shared_ptr<grouping_enumerable<T, TKey>>>> groupby(TF selector);

    template <typename TF, typename TResult = typename std::invoke_result<TF, const T &>::type::element_type::value_type>
    std::shared_ptr<enumerable<TResult>> selectmany(TF selector);

    std::vector<T> to_vector();
    void for_each(void (*action)(T &));

    iterator<T> begin()
    {
        return iterator<T>(enumerate());
    };

    iterator<T> end()
    {
        return iterator<T>::end();
    }

protected:
    virtual std::shared_ptr<enumerable<T>> share() = 0;
};

} // namespace linqxx