#pragma once

#include <memory>   // std::shared_ptr
#include <optional> // std::optional
#include <utility>  // std::declval
#include <vector>   // std::vector

namespace linqxx
{

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

    template<typename TF, typename TResult = typename std::invoke_result<TF, const T &>::type::element_type::value_type>
    std::shared_ptr<enumerable<TResult>> selectmany(TF selector);

    std::vector<T> to_vector();
    void for_each(void (*action)(T &));

protected:
    virtual std::shared_ptr<enumerable<T>> share() = 0;
};

} // namespace linqxx