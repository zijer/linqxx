#pragma once

#include <memory>   // std::shared_ptr
#include <optional> // std::optional
#include <utility>  // std::declval
#include <vector>   // std::vector

#include "enumerable.hpp"        // linqxx::enumerable
#include "stl_enumerable.hpp"    // linqxx::stl_enumerable
#include "where_enumerable.hpp"  // linqxx::where_enumerable
#include "select_enumerable.hpp" // linqxx::select_enumerable

namespace linqxx
{

template <typename TColl, typename T = typename TColl::value_type>
std::shared_ptr<enumerable<T>> from(TColl &source)
{
    return stl_enumerable<TColl, T>::from(source);
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