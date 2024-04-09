#include <iostream>
#include "AV_vector.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>
#include <functional>
#include "gtest/gtest.h"
#include <vector>
#include <cstdlib>



namespace {
    //::vector<int> v;

    // 
    inline const uint32_t DEFAULT_COOKIE = 0xdeadbeef;

    struct TestObj {
        TestObj() = default;
        TestObj(const TestObj& other) = default;
        TestObj& operator=(const TestObj& other) = default;
        TestObj(TestObj&& other) = default;
        TestObj& operator=(TestObj&& other) = default;
        ~TestObj() {
            cookie = 0;
        }
        [[nodiscard]] bool IsAlive() const noexcept {
            return cookie == DEFAULT_COOKIE;
        }
        uint32_t cookie = DEFAULT_COOKIE;
    };

    struct Obj {
        Obj() {
            if (default_construction_throw_countdown > 0) {
                if (--default_construction_throw_countdown == 0) {
                    throw std::runtime_error("Oops");
                }
            }
            ++num_default_constructed;
        }

        explicit Obj(int id)
            : id(id)  //
        {
            ++num_constructed_with_id;
        }

        Obj(int id, std::string name)
            : id(id)
            , name(std::move(name))  //
        {
            ++num_constructed_with_id_and_name;
        }

        Obj(const Obj& other)
            : id(other.id)  //
        {
            if (other.throw_on_copy) {
                throw std::runtime_error("Oops");
            }
            ++num_copied;
        }

        Obj(Obj&& other) noexcept
            : id(other.id)  //
        {
            ++num_moved;
        }

        Obj& operator=(const Obj& other) {
            if (this != &other) {
                id = other.id;
                name = other.name;
                ++num_assigned;
            }
            return *this;
        }

        Obj& operator=(Obj&& other) noexcept {
            id = other.id;
            name = std::move(other.name);
            ++num_move_assigned;
            return *this;
        }

        ~Obj() {
            ++num_destroyed;
            id = 0;
        }

        static int GetAliveObjectCount() {
            return num_default_constructed + num_copied + num_moved + num_constructed_with_id
                + num_constructed_with_id_and_name - num_destroyed;
        }

        static void ResetCounters() {
            default_construction_throw_countdown = 0;
            num_default_constructed = 0;
            num_copied = 0;
            num_moved = 0;
            num_destroyed = 0;
            num_constructed_with_id = 0;
            num_constructed_with_id_and_name = 0;
            num_assigned = 0;
            num_move_assigned = 0;
        }

        bool throw_on_copy = false;
        int id = 0;
        std::string name;

        static inline int default_construction_throw_countdown = 0;
        static inline int num_default_constructed = 0;
        static inline int num_constructed_with_id = 0;
        static inline int num_constructed_with_id_and_name = 0;
        static inline int num_copied = 0;
        static inline int num_moved = 0;
        static inline int num_destroyed = 0;
        static inline int num_assigned = 0;
        static inline int num_move_assigned = 0;
    };

}  // namespace

// TEST(Vector,BaseMethod){
//     Obj::ResetCounters();
//     const size_t SIZE = 100500;
//     const size_t INDEX = 10;
//     const int MAGIC = 42;

//     {
//         AV_vector<int> v;
//         EXPECT_EQ(v.capacity(), 0);
//         EXPECT_EQ(v.size() , 0);

//         v.reserve(SIZE);
//         EXPECT_EQ(v.capacity(),SIZE);
//         EXPECT_EQ(v.size(), 0);
//     }
//     {
//         AV_vector<int> v(SIZE);
//         const auto& cv(v);
//         EXPECT_EQ(v.capacity() , SIZE);
//         EXPECT_EQ(v.size() , SIZE);
//         EXPECT_EQ(v[0] , 0);
//         EXPECT_EQ(&v[0] , &cv[0]);
//         v[INDEX] = MAGIC;
//         EXPECT_EQ(v[INDEX] , MAGIC);
//         EXPECT_EQ(&v[100] - &v[0] , 100);

//         v.reserve(SIZE * 2);
//         EXPECT_EQ(v.size() , SIZE);
//         EXPECT_EQ(v.capacity() , SIZE * 2);
//         EXPECT_EQ(v[INDEX] , MAGIC);
//     }

//     {
//         AV_vector<int> v(SIZE);
//         v[INDEX] = MAGIC;
//         const auto v_copy(v);
//         EXPECT_NE(&v[INDEX] , &v_copy[INDEX]);
//         EXPECT_EQ(v[INDEX] , v_copy[INDEX]);
//     }
//     {
//         AV_vector<Obj> v;
//         v.reserve(SIZE);
//         EXPECT_EQ(Obj::GetAliveObjectCount() , 0);
//     }
//     {
//         AV_vector<Obj> v(SIZE);
//         EXPECT_EQ(Obj::GetAliveObjectCount() , SIZE);
//         const int old_copy_count = Obj::num_copied;
//         const int old_move_count = Obj::num_moved;
//         v.reserve(SIZE * 2);
//         EXPECT_EQ(Obj::GetAliveObjectCount() , SIZE);
//         EXPECT_EQ(Obj::num_copied , old_copy_count);
//         EXPECT_EQ(Obj::num_moved ,old_move_count + static_cast<int>(SIZE));
//     }
//     EXPECT_EQ(Obj::GetAliveObjectCount() , 0);
//     //std::cerr << "Test1() is succesful\n";

// }

// TEST(Vector,Exception) {
//     const size_t SIZE = 100;
//     Obj::ResetCounters();
//     {
//         Obj::default_construction_throw_countdown = SIZE / 2;
//         try {
//             AV_vector<Obj> v(SIZE);
//             ASSERT_EQ(false , "Exception is expected");
//         }
//         catch (const std::runtime_error&) {
//         }
//         catch (...) {
//             // Unexpected error
//             ASSERT_EQ(false ,"Unexpected exception");
//         }
//         EXPECT_EQ(Obj::num_default_constructed , SIZE / 2 - 1);
//         EXPECT_EQ(Obj::GetAliveObjectCount() , 0);
//     }
//     Obj::ResetCounters();
//     {
//         AV_vector<Obj> v(SIZE);
//         try {
//             v[SIZE / 2].throw_on_copy = true;
//             AV_vector<Obj> v_copy(v);
//             ASSERT_EQ(false , "Exception is expected");
//         }
//         catch (const std::runtime_error&) {
//             EXPECT_EQ(Obj::num_copied , SIZE / 2);
//         }
//         catch (...) {
//             // Unexpected error
//             ASSERT_EQ(false , "Unexpected exception");
//         }
//         EXPECT_EQ(Obj::GetAliveObjectCount() , SIZE);
//     }
//     Obj::ResetCounters();
//     {
//         AV_vector<Obj> v(SIZE);
//         try {
//             v[SIZE - 1].throw_on_copy = true;
//             v.reserve(SIZE * 2);
//         }
//         catch (...) {
//             // Unexpected error
//             ASSERT_EQ(false , "Unexpected exception");
//         }
//         EXPECT_EQ(v.capacity() , SIZE * 2);
//         EXPECT_EQ(v.size() , SIZE);
//         EXPECT_EQ(Obj::GetAliveObjectCount() , SIZE);
//     }
//     //std::cerr << "Test2() is succesful\n";
// }

TEST(Vector,ConstructSZ){
    //arrange
    Obj::ResetCounters();
    const int SIZE = 25;
    const int a = 5;
    {
        AV_vector<int> vc(SIZE);

        //act
        
        //assert
        EXPECT_EQ(vc.size(),SIZE);
        EXPECT_EQ(vc.capacity(),SIZE);
        //EXPECT_EQ(vc.begin(),&vc[0]);
    }
    {
        AV_vector<Obj> vc(SIZE);
        EXPECT_EQ(Obj::num_default_constructed,0);

        EXPECT_EQ(vc.size(),SIZE);
        EXPECT_EQ(vc.capacity(),SIZE);
        //EXPECT_EQ(vc.begin(),nullptr);
    }
}

TEST(Vector,ConstrFullParams){
    Obj::ResetCounters();
    const size_t SIZE = 10;
    
    std::allocator<int> alloc;
    {
        const int a = 5;
        //arrange   

        AV_vector<int> vc(SIZE,a,alloc);
        //act
        
        //assert
        EXPECT_EQ(vc.capacity(),SIZE);
        EXPECT_EQ(vc[4],a);
        EXPECT_EQ(vc.size(),SIZE);
    }
    {
        Obj obj;
        AV_vector<Obj> vc(SIZE,obj,alloc);

        EXPECT_EQ(vc.capacity(),SIZE);
        EXPECT_EQ(vc[4].id,obj.id);
        EXPECT_EQ(vc.size(),SIZE);
        EXPECT_EQ(Obj::num_default_constructed,0);
        
    }

}

TEST(Vector,CopyConstr){
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc(SIZE);

        for (size_t i = 0; i < vc.size(); i++)
        {
            vc.push_back(i);
        }

        AV_vector<int> right(vc);
        EXPECT_EQ(right.size(),vc.size());
        EXPECT_EQ(right.capacity(),vc.capacity());
        EXPECT_NE(right.begin(),vc.begin());
        EXPECT_EQ(right[rand()%SIZE],vc[rand()%SIZE]);
    }
    {
        AV_vector<Obj> vc(SIZE);

        for (size_t i = 0; i < vc.size(); i++)
        {
            
            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_constructed_with_id,SIZE);

        AV_vector<Obj> right(vc);
        EXPECT_EQ(Obj::num_constructed_with_id,SIZE*2);
        EXPECT_EQ(right.size(),vc.size());
        EXPECT_EQ(right.capacity(),vc.capacity());
        EXPECT_NE(right.begin(),vc.begin());
    }
        EXPECT_EQ(Obj::GetAliveObjectCount(),0);
}

TEST(Vector,MoveConstr){
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc(SIZE);

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }

        AV_vector<int> right(std::move(vc));
        EXPECT_NE(right.size(),vc.size());
        EXPECT_NE(right.capacity(),vc.capacity());
        EXPECT_NE(right[rand()%SIZE],vc[rand()%SIZE]);
    }
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_constructed_with_id,SIZE);

        AV_vector<Obj> right(std::move(vc));
        EXPECT_EQ(Obj::num_destroyed,0);
        EXPECT_EQ(Obj::num_constructed_with_id,SIZE);
        EXPECT_EQ(Obj::num_assigned,0);
        EXPECT_EQ(Obj::num_copied,0);
        EXPECT_NE(right.size(),vc.size());
        EXPECT_NE(right.capacity(),vc.capacity());
        EXPECT_NE(right[rand()%SIZE].id,vc[rand()%SIZE].id);
    }
        EXPECT_EQ(Obj::GetAliveObjectCount(),0);

}

TEST(Vector,CopyAssign){
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc(SIZE);

        for (size_t i = 0; i < vc.size(); i++)
        {
            vc.push_back(i);
        }

        AV_vector<int> right(SIZE);
        right=vc;
        EXPECT_EQ(right.size(),vc.size());
        EXPECT_EQ(right.capacity(),vc.capacity());
        EXPECT_NE(right.begin(),vc.begin());
        EXPECT_EQ(right[rand()%SIZE],vc[rand()%SIZE]);
    }
    {
        AV_vector<Obj> vc(SIZE);

        for (size_t i = 0; i < vc.size(); i++)
        {
            
            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_constructed_with_id,SIZE);

        AV_vector<Obj> right(SIZE);
        right=vc;
        EXPECT_EQ(Obj::num_assigned,SIZE);
        EXPECT_EQ(Obj::num_constructed_with_id,SIZE*2);
        EXPECT_EQ(right.size(),vc.size());
        EXPECT_EQ(right.capacity(),vc.capacity());
        EXPECT_NE(right.begin(),vc.begin());
    }
        EXPECT_EQ(Obj::GetAliveObjectCount(),0);
}

TEST(Vector,MoveAssign){
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc(SIZE);

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }

        AV_vector<int> right(SIZE);
        right=std::move(vc);
        EXPECT_EQ(vc.begin(),nullptr);
        EXPECT_NE(right.size(),vc.size());
        EXPECT_NE(right.capacity(),vc.capacity());
        EXPECT_NE(right[rand()%SIZE],vc[rand()%SIZE]);
    }
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_constructed_with_id,SIZE);
        AV_vector<Obj> right(SIZE);
        right=std::move(vc);
        EXPECT_EQ(Obj::num_assigned,0);
        EXPECT_EQ(Obj::num_copied,0);
        EXPECT_EQ(Obj::num_destroyed,0);
        EXPECT_EQ(Obj::num_constructed_with_id,SIZE);
        EXPECT_EQ(vc.begin(),nullptr);
        EXPECT_NE(right.size(),vc.size());
        EXPECT_NE(right.capacity(),vc.capacity());
        EXPECT_NE(right[rand()%SIZE].id,vc[rand()%SIZE].id);
    }
        EXPECT_EQ(Obj::GetAliveObjectCount(),0);
}

TEST(Vector,Reserve){
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    const size_t RSIZE = 60;
    {
        AV_vector<int> vc(SIZE);
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }
        AV_vector<int> right(vc);
        vc.reserve(RSIZE);
        EXPECT_EQ(vc.capacity(),RSIZE);
        EXPECT_EQ(vc.size(),SIZE);
        EXPECT_EQ(vc[rand()%SIZE],right[rand()%SIZE]);
        
    }
    {
        AV_vector<Obj> vc(SIZE);

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        vc.reserve(RSIZE);
        EXPECT_EQ(Obj::num_copied,0);
        EXPECT_EQ(vc.size(),SIZE);
        EXPECT_EQ(vc.capacity(),RSIZE);
        EXPECT_EQ(Obj::num_moved,SIZE);

    }
        EXPECT_EQ(Obj::GetAliveObjectCount(),0);
    {
        AV_vector<Obj> vc(SIZE);

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        AV_vector<Obj> right(vc);
        vc.reserve(RSIZE);
        EXPECT_EQ(right[rand()%SIZE].id,vc[rand()%SIZE].id);
    }
         EXPECT_EQ(Obj::GetAliveObjectCount(),0);

    //act
    
    //assert
}

TEST(Vector,Resize){
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    const size_t RSIZE = 40;
    {
        AV_vector<int> vc(SIZE);
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }
        AV_vector<int> right(vc);
        vc.reserve(RSIZE);
        EXPECT_EQ(vc.capacity(),RSIZE);
        EXPECT_EQ(vc.size(),RSIZE);
        EXPECT_EQ(vc[rand()%RSIZE],right[rand()%RSIZE]);
    }
    
    EXPECT_EQ(Obj::GetAliveObjectCount(),0);
}

TEST(Vector,pushBack){
    //arrange

    //act
    
    //assert
}

TEST(Vector,RVpushBack){
    //arrange

    //act
    
    //assert
}

TEST(Vector,popBack){
    //arrange

    //act
    
    //assert
}



struct C {
    C() noexcept {
        ++def_ctor;
    }
    C(const C& /*other*/) noexcept {
        ++copy_ctor;
    }
    C(C&& /*other*/) noexcept {
        ++move_ctor;
    }
    C& operator=(const C& other) noexcept {
        if (this != &other) {
            ++copy_assign;
        }
        return *this;
    }
    C& operator=(C&& /*other*/) noexcept {
        ++move_assign;
        return *this;
    }
    ~C() {
        ++dtor;
    }

    static void Reset() {
        def_ctor = 0;
        copy_ctor = 0;
        move_ctor = 0;
        copy_assign = 0;
        move_assign = 0;
        dtor = 0;
    }

    inline static size_t def_ctor = 0;
    inline static size_t copy_ctor = 0;
    inline static size_t move_ctor = 0;
    inline static size_t copy_assign = 0;
    inline static size_t move_assign = 0;
    inline static size_t dtor = 0;
};

void Dump() {
    using namespace std;
    cerr << "Def ctors: " << C::def_ctor              //
        << ", Copy ctors: " << C::copy_ctor          //
        << ", Move ctors: " << C::move_ctor          //
        << ", Copy assignments: " << C::copy_assign  //
        << ", Move assignments: " << C::move_assign  //
        << ", Dtors: " << C::dtor << endl;
}

void Benchmark() {
    using namespace std;
    try {
        const size_t NUM = 10;
        C c;
        {
            cerr << "std::vector:" << endl;
            C::Reset();
            vector<C> v(NUM);
            Dump();
            v.push_back(c);
        }
        Dump();
    }
    catch (...) {
    }
    try {
        const size_t NUM = 10;
        C c;
        {
            cerr << "Vector:" << endl;
            C::Reset();
            AV_vector<C> v(NUM);
            Dump();
            v.push_back(c);
        }
        Dump();
    }
    catch (...) {
    }
}



// int main() {

//     testing::InitGoogleTest();

//     return RUN_ALL_TESTS();
// }