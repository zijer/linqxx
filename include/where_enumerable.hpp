#pragma once

#include <memory>         // std::shared_ptr
#include <optional>       // std::optional
#include "enumerable.hpp" // linqxx::enumerable

namespace linqxx
{

template <typename T, typename TF>
class where_enumerator : public enumerator<T>
{
private:
    std::unique_ptr<enumerator<T>> source;
    TF pred;

public:
    where_enumerator(std::unique_ptr<enumerator<T>> source, TF pred);
    std::optional<T> next() override;
};

template <typename T, typename TF>
class where_enumerable : public enumerable<T>
{
private:
    std::shared_ptr<enumerable<T>> source;
    TF pred;

public:
    static std::shared_ptr<enumerable<T>> from(std::shared_ptr<enumerable<T>> source, TF pred);
    std::unique_ptr<enumerator<T>> enumerate() override;

protected:
    where_enumerable(std::shared_ptr<enumerable<T>> source, TF pred);
    std::shared_ptr<enumerable<T>> share() override;
};

template <typename T, typename TF>
where_enumerator<T, TF>::where_enumerator(std::unique_ptr<enumerator<T>> source, TF pred)
    : source(std::move(source)), pred(pred){};

template <typename T, typename TF>
std::optional<T> where_enumerator<T, TF>::next()
{
    std::optional<T> item;
    do
    {
        item = source->next();
    } while (item && !pred(item.value()));
    return item;
};

template <typename T, typename TF>
std::shared_ptr<enumerable<T>> where_enumerable<T, TF>::from(std::shared_ptr<enumerable<T>> source, TF pred)
{
    return std::shared_ptr<where_enumerable<T, TF>>(new where_enumerable<T, TF>(source, pred));
};

template <typename T, typename TF>
std::unique_ptr<enumerator<T>> where_enumerable<T, TF>::enumerate()
{
    return std::unique_ptr<where_enumerator<T, TF>>(new where_enumerator<T, TF>(source->enumerate(), pred));
};

template <typename T, typename TF>
where_enumerable<T, TF>::where_enumerable(std::shared_ptr<enumerable<T>> source, TF pred)
    : source(source), pred(pred){};

template <typename T, typename TF>
std::shared_ptr<enumerable<T>> where_enumerable<T, TF>::share()
{
    return where_enumerable<T, TF>::from(source, pred);
};

template <typename T>
template <typename TF>
std::shared_ptr<enumerable<T>> enumerable<T>::where(TF pred)
{
    return where_enumerable<T, TF>::from(this->share(), pred);
};

} // namespace linqxx
