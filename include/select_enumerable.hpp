#pragma once

#include <memory>         // std::shared_ptr
#include <optional>       // std::optional
#include "enumerable.hpp" // linqxx::enumerable

namespace linqxx
{

template <typename TSource, typename TResult>
class select_enumerator : public enumerator<TResult>
{
private:
    std::unique_ptr<enumerator<TSource>> source;
    TResult (*selector)(const TSource &);

public:
    select_enumerator(std::unique_ptr<enumerator<TSource>> source, TResult (*selector)(const TSource &));
    std::optional<TResult> next() override;
};

template <typename TSource, typename TResult>
class select_enumerable : public enumerable<TResult>
{
private:
    std::shared_ptr<enumerable<TSource>> source;
    TResult (*selector)(const TSource &);

public:
    static std::shared_ptr<enumerable<TResult>> from(
        std::shared_ptr<enumerable<TSource>> source,
        TResult (*selector)(const TSource &));
    std::unique_ptr<enumerator<TResult>> enumerate() override;

protected:
    select_enumerable(std::shared_ptr<enumerable<TSource>> source, TResult (*selector)(const TSource &));
    std::shared_ptr<enumerable<TResult>> share() override;
};

template <typename TSource, typename TResult>
select_enumerator<TSource, TResult>::select_enumerator(std::unique_ptr<enumerator<TSource>> source, TResult (*selector)(const TSource &))
    : source(std::move(source)), selector(selector){};

template <typename TSource, typename TResult>
std::optional<TResult> select_enumerator<TSource, TResult>::next()
{
    std::optional<TSource> item = source->next();
    return item ? std::optional<TResult>(selector(item.value())) : std::nullopt;
};

template <typename TSource, typename TResult>
std::shared_ptr<enumerable<TResult>> select_enumerable<TSource, TResult>::from(
    std::shared_ptr<enumerable<TSource>> source,
    TResult (*selector)(const TSource &))
{
    return std::shared_ptr<select_enumerable<TSource, TResult>>(new select_enumerable<TSource, TResult>(source, selector));
};

template <typename TSource, typename TResult>
std::unique_ptr<enumerator<TResult>> select_enumerable<TSource, TResult>::enumerate()
{
    return std::unique_ptr<select_enumerator<TSource, TResult>>(new select_enumerator<TSource, TResult>(source->enumerate(), selector));
};

template <typename TSource, typename TResult>
select_enumerable<TSource, TResult>::select_enumerable(std::shared_ptr<enumerable<TSource>> source, TResult (*selector)(const TSource &))
    : source(source), selector(selector){};

template <typename TSource, typename TResult>
std::shared_ptr<enumerable<TResult>> select_enumerable<TSource, TResult>::share()
{
    return std::shared_ptr<select_enumerable<TSource, TResult>>(new select_enumerable<TSource, TResult>(source, selector));
};

template <typename T>
template <typename TF>
auto enumerable<T>::select(TF selector) -> std::shared_ptr<enumerable<decltype(selector(std::declval<const T &>))>>
{
    using TResult = decltype(selector(std::declval<const T &>));
    return select_enumerable<T, TResult>::from(this->share(), selector);
};

} // namespace linqxx