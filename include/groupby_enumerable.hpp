#pragma once

#include <optional>           // std::optional
#include <functional>         // std::equal_to
#include <utility>            // std::hash
#include <memory>             // std::shared_ptr
#include <unordered_map>      // std::unordered_map
#include <vector>             // std::vector
#include "enumerable.hpp"     // linqxx::enumerable
#include "stl_enumerable.hpp" // linqxx::stl_enumerable

namespace linqxx
{
#define GROUP std::shared_ptr<grouping_enumerable<T, TKey>>

template <typename T, typename TKey>
class grouping_enumerable : public rstl_enumerable<std::vector<T>>
{
public:
    const TKey key;
    static GROUP from(TKey key, std::vector<T>&& items);
protected:
    grouping_enumerable(TKey key, std::vector<T>&& items);
    std::shared_ptr<enumerable<T>> share() override;
};

template <typename T, typename TKey>
class groupby_enumerator : public stl_enumerator<std::vector<GROUP>>
{
private:
    std::unique_ptr<std::vector<GROUP>> source_;

public:
    groupby_enumerator(std::unique_ptr<std::vector<GROUP>> source);
    std::optional<GROUP> next() override;
};

template <typename T, typename TKey, class Hash = std::hash<TKey>, class Pred = std::equal_to<TKey>>
class groupby_enumerable : public enumerable<GROUP>
{
private:
    std::shared_ptr<enumerable<T>> source;
    TKey (*selector)(const T &);

public:
    std::unique_ptr<enumerator<GROUP>> enumerate() override;
    static std::shared_ptr<enumerable<GROUP>> from(std::shared_ptr<enumerable<T>> source, TKey (*selector)(const T &));

protected:
    groupby_enumerable(std::shared_ptr<enumerable<T>> source, TKey (*selector)(const T &));
    std::shared_ptr<enumerable<GROUP>> share() override;
};

template <typename T, typename TKey>
GROUP grouping_enumerable<T, TKey>::from(TKey key, std::vector<T>&& items)
{
    return GROUP(new grouping_enumerable<T, TKey>(key, std::move(items)));
};

template <typename T, typename TKey>
grouping_enumerable<T, TKey>::grouping_enumerable(TKey key, std::vector<T>&& items)
    : rstl_enumerable<std::vector<T>>(std::move(items)), key(key){};

template <typename T, typename TKey>
std::shared_ptr<enumerable<T>> grouping_enumerable<T, TKey>::share()
{
    std::vector<T> copy = rstl_enumerable<std::vector<T>>::source;
    return GROUP(new grouping_enumerable<T, TKey>(key, std::move(copy)));
};

template <typename T, typename TKey>
groupby_enumerator<T, TKey>::groupby_enumerator(std::unique_ptr<std::vector<GROUP>> source)
    : stl_enumerator<std::vector<GROUP>>(source->begin(), source->end()), source_(std::move(source)){};

template <typename T, typename TKey>
std::optional<GROUP> groupby_enumerator<T, TKey>::next()
{
    return stl_enumerator<std::vector<GROUP>>::next();
};

template <typename T, typename TKey, class Hash, class Pred>
std::unique_ptr<enumerator<GROUP>> groupby_enumerable<T, TKey, Hash, Pred>::enumerate()
{
    auto groups = std::unique_ptr<std::vector<GROUP>>(new std::vector<GROUP>());
    std::vector<TKey> keys;
    std::unordered_map<TKey, std::vector<T>, Hash, Pred> groupings;

    auto en = source->enumerate();
    std::optional<T> item;
    while (item = en->next())
    {
        auto value = item.value();
        auto key = selector(value);
        if (groupings.find(key) == groupings.end())
        {
            keys.push_back(key);
        }
        groupings[key].push_back(value);
    };
    for (auto key : keys)
    {
        groups->push_back(grouping_enumerable<T, TKey>::from(key, std::move(groupings.at(key))));
        groupings.erase(key);
    }
    return std::unique_ptr<groupby_enumerator<T, TKey>>(new groupby_enumerator<T, TKey>(std::move(groups)));
};

template <typename T, typename TKey, class Hash, class Pred>
std::shared_ptr<enumerable<GROUP>> groupby_enumerable<T, TKey, Hash, Pred>::from(std::shared_ptr<enumerable<T>> source, TKey (*selector)(const T &))
{
    return std::shared_ptr<groupby_enumerable<T, TKey>>(new groupby_enumerable<T, TKey>(source, selector));
};

template <typename T, typename TKey, class Hash, class Pred>
groupby_enumerable<T, TKey, Hash, Pred>::groupby_enumerable(std::shared_ptr<enumerable<T>> source, TKey (*selector)(const T &))
    : source(source), selector(selector){};

template <typename T, typename TKey, class Hash, class Pred>
std::shared_ptr<enumerable<GROUP>> groupby_enumerable<T, TKey, Hash, Pred>::share()
{
    return groupby_enumerable<T, TKey>::from(source, selector);
};

template <typename T>
template <typename TF, typename TKey, class Hash, class Pred>
std::shared_ptr<enumerable<GROUP>> enumerable<T>::groupby(TF selector)
{
    return groupby_enumerable<T, TKey, Hash, Pred>::from(this->share(), selector);
};

#undef GROUP
} // namespace linqxx