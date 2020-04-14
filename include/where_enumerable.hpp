#pragma once

#include <memory>         // std::shared_ptr
#include <optional>       // std::optional
#include "enumerable.hpp" // linqxx::enumerable

namespace linqxx
{

template <typename T>
class where_enumerator : public enumerator<T>
{
private:
    std::unique_ptr<enumerator<T>> source;
    bool (*pred)(const T &);

public:
    where_enumerator(std::unique_ptr<enumerator<T>> source, bool (*pred)(const T &));
    std::optional<T> next() override;
};

template <typename T>
class where_enumerable : public enumerable<T>
{
private:
    std::shared_ptr<enumerable<T>> source;
    bool (*pred)(const T &);

public:
    static std::shared_ptr<enumerable<T>> from(std::shared_ptr<enumerable<T>> source, bool (*pred)(const T &));

    std::unique_ptr<enumerator<T>> enumerate() override;

protected:
    where_enumerable(std::shared_ptr<enumerable<T>> source, bool (*pred)(const T &));
    std::shared_ptr<enumerable<T>> share() override;
};

template <typename T>
where_enumerator<T>::where_enumerator(std::unique_ptr<enumerator<T>> source, bool (*pred)(const T &))
    : source(std::move(source)), pred(pred){};

template <typename T>
std::optional<T> where_enumerator<T>::next()
{
    std::optional<T> item;
    do
    {
        item = source->next();
    } while (item && !pred(item.value()));
    return item;
};

template <typename T>
std::shared_ptr<enumerable<T>> where_enumerable<T>::from(std::shared_ptr<enumerable<T>> source, bool (*pred)(const T &))
{
    return std::unique_ptr<where_enumerable<T>>(new where_enumerable<T>(source, pred));
};

template <typename T>
std::unique_ptr<enumerator<T>> where_enumerable<T>::enumerate()
{
    return std::unique_ptr<where_enumerator<T>>(new where_enumerator<T>(source->enumerate(), pred));
};

template <typename T>
where_enumerable<T>::where_enumerable(std::shared_ptr<enumerable<T>> source, bool (*pred)(const T &))
    : source(source), pred(pred){};

template <typename T>
std::shared_ptr<enumerable<T>> where_enumerable<T>::share()
{
    return std::unique_ptr<where_enumerable<T>>(new where_enumerable<T>(source, pred));
};

template <typename T>
std::shared_ptr<enumerable<T>> enumerable<T>::where(bool (*pred)(const T &))
{
    return where_enumerable<T>::from(this->share(), pred);
};

} // namespace linqxx
