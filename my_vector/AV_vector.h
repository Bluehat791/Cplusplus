#pragma once
#include <memory>
#include <cstddef>
#include <utility>
#include <stdexcept>
#include <iterator>

template <typename T>
class Iterator
{
public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::remove_cv_t<T>;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;
    // using iterator_concept = std::contiguous_iterator_tag;
public:
    explicit Iterator(T* ptr = nullptr);
    Iterator(const Iterator& iterator);

    ~Iterator() = default;

    bool operator==(const Iterator& lhs) const;
    bool operator!=(const Iterator& lhs) const;

    Iterator operator++();
    //  Post increment
    Iterator operator--();
    //  Post decrement
    Iterator operator+=(const int& tmp);
    Iterator operator-=(const int& tmp);
    bool operator<(const Iterator& lhs);
    bool operator<=(const Iterator& lhs);
    bool operator>(const Iterator& lhs);
    bool operator>=(const Iterator& lhs);
    T& operator*();
protected:
    T* date;
};

template <typename T>
Iterator<T>::Iterator(T* ptr) : date(ptr)
{

}

template <typename T>
Iterator<T>::Iterator(const Iterator& iterator) : Iterator(iterator.date)
{

}

template <typename T>
bool Iterator<T>::operator==(const Iterator& lhs) const
{
    return (date == lhs.date);
}

template <typename T>
bool Iterator<T>::operator!=(const Iterator& lhs) const
{
    return (date != lhs.date);
}

template <typename T>
Iterator<T> Iterator<T>::operator++()
{
    date++;
    return *this;
}

template <typename T>
Iterator<T> Iterator<T>::operator--()
{
    date--;
    return *this;
}

template <typename T>
Iterator<T> Iterator<T>::operator+=(const int& tmp)
{
    date += tmp;
    return *this;
}

template <typename T>
Iterator<T> Iterator<T>::operator-=(const int& tmp)
{
    date -= tmp;
    return *this;
}

template <typename T>
bool Iterator<T>::operator<(const Iterator& lhs)
{
    return date < lhs.date;
}

template <typename T>
bool Iterator<T>::operator<=(const Iterator& lhs)
{
    return date <= lhs.date;
}

template <typename T>
bool Iterator<T>::operator>(const Iterator& lhs)
{
    return date > lhs.date;
}

template <typename T>
bool Iterator<T>::operator>=(const Iterator& lhs)
{
    return date >= lhs.date;
}

template <typename T>
T& Iterator<T>::operator*()
{
    return date;
}







//-----------------------------------------------
template <typename T, typename Alloc = std::allocator<T>>
class AV_vector {
public:
    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;
    using reverse_iterator = std::reverse_iterator<Iterator<T>>;
    using const_reverse_iterator = std::reverse_iterator<Iterator<const T>>;

private:
    T* arr;
    size_t sz;

    //capacity
    size_t cap;
    Alloc alloc;
    using AllocTraits = std::allocator_traits<Alloc>;
public:

    size_t size();

    size_t capacity();

    AV_vector(int size_m);

    AV_vector(size_t count, const T& value, const Alloc alloc = Alloc());

    void reserve(size_t n);

    void resize(size_t n, const T& value = T());

    template<typename... Args>
    void emplace_back(const Args&... args);

    void push_back(const T& value);

    void push_back(T&& value);

    void pop_back();

    T& operator[](size_t i);

    T& at(size_t i);

    T& front();

    T& back();

    iterator begin() noexcept;

    iterator end() noexcept;


    const_iterator begin() const noexcept;

    const_iterator end() const noexcept;


    const_iterator cbegin() const noexcept;

    iterator cend() const noexcept;

    reverse_iterator rbegin() noexcept;

    reverse_iterator rend() noexcept;

};

template<typename T, typename Alloc>
size_t AV_vector<T, Alloc>::size()
{
    return size();
}

template<typename T, typename Alloc>
size_t AV_vector<T, Alloc>::capacity()
{
    return cap;
}

template<typename T, typename Alloc>
AV_vector<T, Alloc>::AV_vector(int size_m) : arr(nullptr), sz(size_m)
{   
    std::uninitialized_value_construct_n(arr,size_m);
}

template<typename T, typename Alloc>
AV_vector<T, Alloc>::AV_vector(size_t count, const T& value, const Alloc alloc) : alloc(alloc) {
    reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        AllocTraits::construct(alloc, arr + i, value);
    }

}

template<typename T, typename Alloc>
void AV_vector<T, Alloc>::reserve(size_t n)
{

    if (n <= cap) return;
    T* newarr = AllocTraits::allocate(alloc,n);

    try {
        std::uninitialized_copy(arr, arr + sz, newarr);
    }
    catch (...) {
        AllocTraits::deallocate(alloc, newarr, n);
        throw;
    }

    for (size_t i = 0; i < sz; ++i)
    {
        AllocTraits::destroy(alloc, arr + i);
    }

    AllocTraits::deallocate(alloc, arr,n);
    arr = newarr;
    cap = n;

}

template<typename T, typename Alloc>
void AV_vector<T, Alloc>::resize(size_t n, const T& value)
{
    if (n > cap) reserve(n);
    for (size_t i = sz; i < n; ++i)
    {
        new(arr + i) T(value);

    }
    if (n < sz) {
        sz = n;
    }

}

template<typename T, typename Alloc>
template<typename... Args>
void AV_vector<T, Alloc>::emplace_back(const Args&... args)
{
    if (cap == sz)
    {
        reserve(2 * sz);
    }
    AllocTraits::construct(alloc, arr + sz, std::forward<Args>(args)...);
    ++sz;
}

template<typename T, typename Alloc>
void AV_vector<T, Alloc>::push_back(const T& value)
{
    if (cap == sz)
    {
        reserve(2 * sz);
    }
    new(arr + sz) T(value);
    ++sz;
}

template<typename T, typename Alloc>
void AV_vector<T, Alloc>::push_back(T&& value)
{
    if (cap == sz)
    {
        reserve(2 * sz);
    }
    new(arr + sz) T(std::move(value));
    ++sz;
}

template<typename T, typename Alloc>
void AV_vector<T, Alloc>::pop_back() {
    --sz;
    AllocTraits::destroy(alloc, arr + sz);
}

template<typename T, typename Alloc>
T& AV_vector<T, Alloc>::operator[](size_t i)
{
    return arr[i];
}

template<typename T, typename Alloc>
T& AV_vector<T, Alloc>::at(size_t i)
{
    if (i >= sz)
        throw std::out_of_range("...");
    return arr[i];
}

template<typename T, typename Alloc>
T& AV_vector<T, Alloc>::front()
{
    return arr[sz];
}

template<typename T, typename Alloc>
T& AV_vector<T, Alloc>::back()
{
    return arr[0];
}

template<typename T, typename Alloc>
typename AV_vector<T, Alloc>::iterator AV_vector<T, Alloc>::begin() noexcept
{
    return arr;
}


template<typename T, typename Alloc>
typename AV_vector<T, Alloc>::iterator AV_vector<T, Alloc>::end() noexcept
{
    return arr + sz;
}

template<typename T, typename Alloc>
typename AV_vector<T, Alloc>::const_iterator AV_vector<T, Alloc>::begin() const noexcept
{
    return arr;
}

template<typename T, typename Alloc>
typename AV_vector<T, Alloc>::const_iterator AV_vector<T, Alloc>::end() const noexcept
{
    return arr + sz;
}

template<typename T, typename Alloc>
typename AV_vector<T, Alloc>::const_iterator AV_vector<T, Alloc>::cbegin() const noexcept
{
    return arr;
}

template<typename T, typename Alloc>
typename AV_vector<T, Alloc>::iterator AV_vector<T, Alloc>::cend() const noexcept
{
    return arr + sz;
}

template<typename T, typename Alloc>
typename AV_vector<T, Alloc>::reverse_iterator AV_vector<T, Alloc>::rbegin() noexcept
{
    return std::make_reverse_iterator(arr);
}

template<typename T, typename Alloc>
typename AV_vector<T, Alloc>::reverse_iterator AV_vector<T, Alloc>::rend() noexcept
{
    return std::make_reverse_iterator(arr + sz);
}


int main()
{
    AV_vector<int> vec(5,5);
    for (size_t i = 0; i < 10; i++)
    {
        vec.push_back(i);
        std::cout << vec[i];
    }

    return 0;
}