#pragma once
#include <optional>
#include <functional>
#include <utility>
#include <memory>

#include "stl_enumerable.hpp" // linqxx::stl_enumerable
#include "enumerable.hpp"     // linqxx::enumerable

namespace linqxx
{
//, class Hash = std::hash<TKey>, class Pred = std::equal_to<TKey>
template <typename T, typename TKey>
class grouping_enumerable : public stl_enumerable<std::vector<T>>
{
private:
    std::vector<T> items_;

public:
    const TKey key;
    static std::shared_ptr<grouping_enumerable<T, TKey>> from(TKey key, std::vector<T> items)
    {
        return std::shared_ptr<grouping_enumerable<T, TKey>>(new grouping_enumerable<T, TKey>(key, items));
    };
    std::unique_ptr<enumerator<T>> enumerate() override
    {
        return std::move(stl_enumerable<std::vector<T>>::enumerate());
    };

protected:
    grouping_enumerable(TKey key, std::vector<T> items)
        : stl_enumerable<std::vector<T>>(items_), key(key), items_(items){};
    std::shared_ptr<enumerable<T>> share()
    {
        return std::shared_ptr<grouping_enumerable<T, TKey>>(new grouping_enumerable<T, TKey>(key, items_));
    };
};
template <typename T, typename TKey>
class groupby_enumerator : public stl_enumerator<std::vector<std::shared_ptr<grouping_enumerable<T, TKey>>>>
{
private:
    std::unique_ptr<std::vector<std::shared_ptr<grouping_enumerable<T, TKey>>>> source_;

public:
    groupby_enumerator(std::unique_ptr<std::vector<std::shared_ptr<grouping_enumerable<T, TKey>>>> source)
        : stl_enumerator<std::vector<std::shared_ptr<grouping_enumerable<T, TKey>>>>(source->begin(), source->end()), source_(std::move(source)){};
    std::optional<std::shared_ptr<grouping_enumerable<T, TKey>>> next() override
    {
        return stl_enumerator<std::vector<std::shared_ptr<grouping_enumerable<T, TKey>>>>::next();
    };
};
template <typename T, typename TKey, class Hash = std::hash<TKey>, class Pred = std::equal_to<TKey>>
class groupby_enumerable : public enumerable<std::shared_ptr<grouping_enumerable<T, TKey>>>
{
private:
    std::shared_ptr<enumerable<T>> source;
    TKey (*selector)(const T &);

public:
    std::unique_ptr<enumerator<std::shared_ptr<grouping_enumerable<T, TKey>>>> enumerate() override
    {
        auto groups = std::unique_ptr<std::vector<std::shared_ptr<grouping_enumerable<T, TKey>>>>(new std::vector<std::shared_ptr<grouping_enumerable<T, TKey>>>());
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
            groups->push_back(grouping_enumerable<T, TKey>::from(key, groupings[key]));
        }
        return std::unique_ptr<groupby_enumerator<T, TKey>>(new groupby_enumerator<T, TKey>(std::move(groups)));
    };
    static std::shared_ptr<enumerable<std::shared_ptr<grouping_enumerable<T, TKey>>>> from(std::shared_ptr<enumerable<T>> source, TKey (*selector)(const T &))
    {
        return std::shared_ptr<groupby_enumerable<T, TKey>>(new groupby_enumerable<T, TKey>(source, selector));
    };

protected:
    groupby_enumerable(std::shared_ptr<enumerable<T>> source, TKey (*selector)(const T &))
        : source(source), selector(selector){};
    std::shared_ptr<enumerable<std::shared_ptr<grouping_enumerable<T, TKey>>>> share() override
    {
        return std::shared_ptr<groupby_enumerable<T, TKey>>(new groupby_enumerable<T, TKey>(source, selector));
    };
};
template <typename T>
template <typename TKey, class Hash, class Pred>
auto enumerable<T>::group_by(TKey (*selector)(const T &)) -> std::shared_ptr<enumerable<std::shared_ptr<grouping_enumerable<T, TKey>>>>
{
    return linqxx::groupby_enumerable<T, TKey, Hash, Pred>::from(this->share(), selector);
};

// template <typename T>
// template <typename TF, class Hash, class Pred>
// auto enumerable<T>::group_by(TF selector) -> std::shared_ptr<enumerable<std::shared_ptr<grouping_enumerable<T, decltype(selector(std::declval<const T &>))>>>>
// {
//     using TKey = decltype(selector(std::declval<const T &>));
//     return linqxx::groupby_enumerable<T, TKey, Hash, Pred>::from(this->share(), selector);
// };

} // namespace linqxx