#pragma once

#include <memory>   // std::shared_ptr
#include <optional> // std::optional
#include <utility>  // std::declval
#include <vector>   // std::vector

#include "enumerable.hpp"            // linqxx::enumerable
#include "stl_enumerable.hpp"        // linqxx::stl_enumerable
#include "where_enumerable.hpp"      // linqxx::where_enumerable
#include "select_enumerable.hpp"     // linqxx::select_enumerable
#include "groupby_enumerable.hpp"    // linqxx::groupby_enumerable
#include "selectmany_enumerable.hpp" // linqxx::selectmany_enumerable
#include "iterator.hpp"              // linqxx::iterator

namespace linqxx
{

template <typename TColl, typename T = typename TColl::value_type>
std::shared_ptr<enumerable<T>> from(TColl &source)
{
    return stl_enumerable<TColl, T>::from(source);
};

template <typename TColl, typename T = typename TColl::value_type>
std::shared_ptr<enumerable<T>> from(TColl &&source)
{
    return rstl_enumerable<TColl, T>::from(std::move(source));
};

template <typename T>
T enumerable<T>::first()
{
    auto en = this->enumerate();
    auto item = en->next();
    return item.value(); 
};

template <typename T>
T enumerable<T>::last()
{
    auto en = this->enumerate();
    std::optional<T> item = std::nullopt;
    while (auto temp = en->next())
    {
        item = temp.value();
    };
    return item.value();
};

template <typename T>
std::vector<T> enumerable<T>::to_vector()
{
    std::vector<T> res;
    auto en = this->enumerate();
    std::optional<T> item;
    while (item = en->next())
    {
        res.push_back(item.value());
    };
    return res;
};

template <typename T>
void enumerable<T>::for_each(void (*action)(T &))
{
    auto en = this->enumerate();
    std::optional<T> item;
    while (item = en->next())
    {
        action(item.value());
    };
};

} // namespace linqxx

template <typename T>
struct std::iterator_traits<linqxx::iterator<T>>
{
    using iterator_category = typename linqxx::iterator<T>::iterator_category;
    using value_type = typename linqxx::iterator<T>::value_type;
    using difference_type = void;
};