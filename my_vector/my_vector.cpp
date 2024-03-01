#include <memory>
#include <iostream>


template <typename T>
class My_vector {
private:
    T* arr;
    size_t sz;

    //capacity
    size_t cap;

public:
    size_t size()
    {
        return sz;
    }

    size_t capacity()
    {
        return cap;
    }

    void reserve(size_t n)
    {

        if (n <= cap) return;
        T* newarr = reinterpret_cast<T*>(new int8_t[n * sizeof(T)]); // need some bytes
        //T* newarr =  new T[n]; faulty

        try {
            std::uninitialized_copy(arr, arr + sz, newarr);
        }
        catch (...) {
            delete[] reinterpret_cast<int8_t*>(newarr); //?
            throw;
        }
        /*size_t i =0;
        try {
        for( i =0; i<sz;++i)
        {
            new(newarr+i) T(arr[i]) //placement new
            newarr[i] == arr[i]
        }
        } catch(...) {
            for (size_t j = 0; j < i; ++j)
            {
                (newarr+i)->~T();
            }
            delete[] reinterpret_cast<int8_t*>(newarr); //?
            throw;
        }*/
        for (size_t i = 0; i < sz; ++i)
        {
            (arr + i)->~T;
        }

        delete[] reinterpret_cast<int8_t*>(arr);
        arr = newarr;
        cap = n;
        
    }

    void resize(size_t n, const T& value = T())
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

    void push_back(const T& value)
    {
        if (cap == sz)
        {
            reserve(2 * sz);
        }
        new(arr + sz) T(value);
        ++sz;
    }

    void pop_back() {
        --sz;
        (arr + sz)->~T;
    }

    T& operator[](size_t i)
    {
        return arr[i];
    }

    /*const T& operator[](size_t i)
    {
        return arr[i];
    }*/

    T& at(size_t i)
    {
        if (i >= sz)
            throw std::out_of_range("...");
        return arr[i];
    }

    T& front()
    {
        return arr[sz];
    }

    T& back()
    {
        return arr[0];
    }
};

int main()
{
    return 0;
}