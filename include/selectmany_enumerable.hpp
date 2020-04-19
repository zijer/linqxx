#pragma once

#include <optional> // std::optional
#include <memory>   // std::shared_ptr
#include "enumerable.hpp"

namespace linqxx
{
template <typename TSource, typename TResult>
class selectmany_enumerator : public enumerator<TResult>
{
private:
    std::shared_ptr<enumerable<TSource>> source;
    std::shared_ptr<enumerable<TResult>> (*selector)(const TSource &);
    std::unique_ptr<enumerator<TSource>> sourceEnumerator;
    std::unique_ptr<enumerator<TResult>> enumerator_;

public:
    selectmany_enumerator(std::shared_ptr<enumerable<TSource>> source, std::shared_ptr<enumerable<TResult>> (*selector)(const TSource &));
    std::optional<TResult> next() override;
};

template <typename TSource, typename TResult>
selectmany_enumerator<TSource, TResult>::selectmany_enumerator(std::shared_ptr<enumerable<TSource>> source, std::shared_ptr<enumerable<TResult>> (*selector)(const TSource &))
    : source(source), selector(selector){};

template <typename TSource, typename TResult>
std::optional<TResult> selectmany_enumerator<TSource, TResult>::next()
{
    if (!sourceEnumerator)
        sourceEnumerator = source->enumerate();
    std::optional<TResult> item;
    while (!enumerator_ || !(item = enumerator_->next()))
    {
        auto nextEnumerator = sourceEnumerator->next();
        if (!nextEnumerator)
            return std::nullopt;
        enumerator_ = selector(nextEnumerator.value())->enumerate();
    }
    return item;
};

template <typename TSource, typename TResult>
class selectmany_enumerable : public enumerable<TResult>
{
private:
    std::shared_ptr<enumerable<TSource>> source;
    std::shared_ptr<enumerable<TResult>> (*selector)(const TSource &);

public:
    std::unique_ptr<enumerator<TResult>> enumerate() override;
    static std::shared_ptr<enumerable<TResult>> from(std::shared_ptr<enumerable<TSource>> source, std::shared_ptr<enumerable<TResult>> (*selector)(const TSource &));

protected:
    selectmany_enumerable(std::shared_ptr<enumerable<TSource>> source, std::shared_ptr<enumerable<TResult>> (*selector)(const TSource &));
    std::shared_ptr<enumerable<TResult>> share() override;
};

template <typename TSource, typename TResult>
std::unique_ptr<enumerator<TResult>> selectmany_enumerable<TSource, TResult>::enumerate()
{
    return std::unique_ptr<selectmany_enumerator<TSource, TResult>>(new selectmany_enumerator<TSource, TResult>(source, selector));
};

template <typename TSource, typename TResult>
std::shared_ptr<enumerable<TResult>> selectmany_enumerable<TSource, TResult>::from(std::shared_ptr<enumerable<TSource>> source, std::shared_ptr<enumerable<TResult>> (*selector)(const TSource &))
{
    return std::shared_ptr<selectmany_enumerable<TSource, TResult>>(new selectmany_enumerable<TSource, TResult>(source, selector));
};

template <typename TSource, typename TResult>
selectmany_enumerable<TSource, TResult>::selectmany_enumerable(std::shared_ptr<enumerable<TSource>> source, std::shared_ptr<enumerable<TResult>> (*selector)(const TSource &))
    : source(source), selector(selector){};

template <typename TSource, typename TResult>
std::shared_ptr<enumerable<TResult>> selectmany_enumerable<TSource, TResult>::share()
{
    return selectmany_enumerable<TSource, TResult>::from(source, selector);
};

template <typename T>
template <typename TF, typename TResult>
std::shared_ptr<enumerable<TResult>> enumerable<T>::selectmany(TF selector)
{
    return selectmany_enumerable<T, TResult>::from(this->share(), selector);
};

} // namespace linqxx