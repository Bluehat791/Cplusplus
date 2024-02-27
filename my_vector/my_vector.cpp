#include <vector>
#include <iostream>
template <typename T>
class Vector {
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
    size_t cap()
    {
        return cap;
    }

    void reserve(size_t n) {
        if(n<=cap) return;
        newarr = reinterpret_cast<T*>(new int8_t[n*sizeof(T)]) // need some bytes
        //T* newarr =  new T[n]; faulty
        for(size_t i =0;i<sz;++i)
        {   
            new(newarr+i) T(arr[i]) //placement new
            newarr[i] == arr[i]
        }
        for (size_t i = 0; i < sz; ++i)
        {
            (arr+i)->~T;
        }
        
        delete[] reinterpret_cast<int8_t*>(arr);
        arr = newarr;
        cap=n;
    }

    void resize(size_t n, const T& value = T())
    {   
        if (n> cap) reserve(n);
        for (size_t i = sz; i < n; ++i)
        {
            new(arr+i) T(value);

        }
        if(n<sz) {
            sz = n;
        }
        
    }


    void push_back(const T& value)
    {
        if(cap == sz)
        {
            reserve(2*sz);
        }
        new(arr+sz) T(value);
        ++sz;
    }

    void pop_back(){
        --sz;
    }
};

int main()
{
    std::vector<int> v;
    for (int i = 0; i < 10; ++i)
    {
        v.push_back(i);
        std::cout << v.size() << " " << v.capacity() << "\n";
    }
    
        return 0;
}