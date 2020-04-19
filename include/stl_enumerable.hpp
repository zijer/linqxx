#pragma once

#include <memory>         // std::shared_ptr
#include <optional>       // std::optional
#include "enumerable.hpp" // linqxx::enumerable

namespace linqxx
{

template <typename TColl, typename T = typename TColl::value_type>
class stl_enumerator : public enumerator<T>
{
private:
    using iter_t = typename TColl::iterator;
    iter_t iter;
    iter_t end;

public:
    stl_enumerator(iter_t start, iter_t end);
    std::optional<T> next() override;
};

template <typename TColl, typename T = typename TColl::value_type>
class stl_enumerable : public enumerable<T>
{
private:
    TColl &source;

public:
    std::unique_ptr<enumerator<T>> enumerate() override;
    static std::shared_ptr<enumerable<T>> from(TColl &source);

protected:
    stl_enumerable(TColl &source);
    std::shared_ptr<enumerable<T>> share() override;
};

template <typename TColl, typename T = typename TColl::value_type>
class rstl_enumerable : public enumerable<T>
{
public:
    std::unique_ptr<enumerator<T>> enumerate() override;
    static std::shared_ptr<enumerable<T>> from(TColl &&source);
protected:
    TColl source;
    rstl_enumerable(TColl &&source);
    std::shared_ptr<enumerable<T>> share() override;
};

template <typename TColl, typename T>
stl_enumerator<TColl, T>::stl_enumerator(iter_t start, iter_t end)
    : iter(start), end(end){};

template <typename TColl, typename T>
std::optional<T> stl_enumerator<TColl, T>::next()
{
    return iter != end ? std::optional<T>(*iter++) : std::nullopt;
};

template <typename TColl, typename T>
std::unique_ptr<enumerator<T>> stl_enumerable<TColl, T>::enumerate()
{
    return std::unique_ptr<stl_enumerator<TColl>>(new stl_enumerator<TColl>(source.begin(), source.end()));
};

template <typename TColl, typename T>
std::shared_ptr<enumerable<T>> stl_enumerable<TColl, T>::from(TColl &source)
{
    return std::shared_ptr<stl_enumerable<TColl>>(new stl_enumerable<TColl>(source));
};

template <typename TColl, typename T>
std::shared_ptr<enumerable<T>> stl_enumerable<TColl, T>::share()
{
    return std::shared_ptr<stl_enumerable<TColl>>(new stl_enumerable<TColl>(source));
};

template <typename TColl, typename T>
stl_enumerable<TColl, T>::stl_enumerable(TColl &source)
    : source(source){};

template <typename TColl, typename T>
std::unique_ptr<enumerator<T>> rstl_enumerable<TColl, T>::enumerate()
{
    return std::unique_ptr<stl_enumerator<TColl>>(new stl_enumerator<TColl>(source.begin(), source.end()));
};

template <typename TColl, typename T>
std::shared_ptr<enumerable<T>> rstl_enumerable<TColl, T>::from(TColl &&source)
{
    return std::shared_ptr<rstl_enumerable<TColl>>(new rstl_enumerable<TColl>(std::move(source)));
};

template <typename TColl, typename T>
std::shared_ptr<enumerable<T>> rstl_enumerable<TColl, T>::share()
{
    TColl copy = source;
    return std::shared_ptr<rstl_enumerable<TColl>>(new rstl_enumerable<TColl>(std::move(copy)));
};

template <typename TColl, typename T>
rstl_enumerable<TColl, T>::rstl_enumerable(TColl &&source)
    : source(source){};

} // namespace linqxx