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
private:
    std::optional<T> item;

public:
    virtual std::optional<T> next() = 0;
    virtual ~enumerator(){};
    // bool move();
    // T current();
};
template <typename T, typename TKey>
class grouping_enumerable;
template <typename T>
class enumerable
{
public:
    virtual std::unique_ptr<enumerator<T>> enumerate() = 0;
    std::shared_ptr<enumerable<T>> where(bool (*pred)(const T &));
    template <typename TF, typename TResult = typename std::invoke_result<TF, const T &>::type>
    std::shared_ptr<enumerable<TResult>> select(TF selector);

    template <typename TF, typename TKey = typename std::invoke_result<TF, const T &>::type, class Hash = std::hash<TKey>, class Pred = std::equal_to<TKey>>
    std::shared_ptr<enumerable<std::shared_ptr<grouping_enumerable<T, TKey>>>> group_by(TF selector);

    std::vector<T> to_vector();
    void for_each(void (*action)(T &));

protected:
    virtual std::shared_ptr<enumerable<T>> share() = 0;
};

/*
template <typename T>
bool enumerator<T>::move()
{
    auto temp = next();
    if (temp)
    {
        item = temp;
        return true;
    }
    else
        return false;
};

template <typename T>
T enumerator<T>::current()
{
    return item.value();
};
*/

} // namespace linqxx