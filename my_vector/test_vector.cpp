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


TEST(Vector, ConstructSZ) {
    //arrange
    Obj::ResetCounters();
    const int SIZE = 25;
    const int a = 5;
    {
        AV_vector<int> vc(SIZE);

        //act

        //assert
        EXPECT_EQ(vc.size(), SIZE);
        EXPECT_EQ(vc.capacity(), SIZE);
        //EXPECT_EQ(vc.begin(),&vc[0]);
    }
    {
        AV_vector<Obj> vc(SIZE);
        EXPECT_EQ(Obj::num_default_constructed, SIZE);

        EXPECT_EQ(vc.size(), SIZE);
        EXPECT_EQ(vc.capacity(), SIZE);
        //EXPECT_EQ(vc.begin(),nullptr);
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}

TEST(Vector, ConstrFullParams) {
    Obj::ResetCounters();
    const size_t SIZE = 10;

    std::allocator<int> alloc;
    {
        const int a = 5;
        //arrange   

        AV_vector<int> vc(SIZE, a, alloc);
        //act

        //assert
        EXPECT_EQ(vc.capacity(), SIZE);
        EXPECT_EQ(vc[4], a);
        EXPECT_EQ(vc.size(), SIZE);
    }
    {
        Obj obj;
        AV_vector<Obj> vc(SIZE, obj, alloc);

        EXPECT_EQ(vc.capacity(), SIZE);
        EXPECT_EQ(vc[4].id, obj.id);
        EXPECT_EQ(vc.size(), SIZE);
        EXPECT_EQ(Obj::num_copied, SIZE);
        //default construcctor for obj
        EXPECT_EQ(Obj::num_default_constructed, 1);

    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);

}

TEST(Vector, CopyConstr) {
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }

        AV_vector<int> right(vc);
        EXPECT_EQ(right.size(), vc.size());
        EXPECT_EQ(right.capacity(), vc.capacity());
        EXPECT_NE(right.begin(), vc.begin());
        int random = rand() % SIZE;
        EXPECT_EQ(right[random], vc[random]);
    }
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {

            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_constructed_with_id, SIZE);

        AV_vector<Obj> right(vc);
        EXPECT_EQ(Obj::num_copied, SIZE);
        EXPECT_EQ(Obj::num_constructed_with_id, SIZE);
        EXPECT_EQ(right.size(), vc.size());
        EXPECT_EQ(right.capacity(), vc.capacity());
        EXPECT_NE(right.begin(), vc.begin());
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}

TEST(Vector, MoveConstr) {
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }

        AV_vector<int> right(std::move(vc));
        EXPECT_NE(right.size(), vc.size());
        EXPECT_NE(right.capacity(), vc.capacity());
        int rand_count = rand() % SIZE;
        try
        {
            EXPECT_NE(right[rand_count], vc[rand_count]);
            EXPECT_EQ("No exception", "Error");
        }
        catch (const std::exception&)
        {
        }
    }
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_constructed_with_id, SIZE);

        AV_vector<Obj> right(std::move(vc));
        EXPECT_EQ(Obj::num_constructed_with_id, SIZE);
        EXPECT_EQ(Obj::num_assigned, 0);
        EXPECT_EQ(Obj::num_copied, 0);
        EXPECT_NE(right.size(), vc.size());
        EXPECT_NE(right.capacity(), vc.capacity());
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);

}

TEST(Vector, CopyAssign) {
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }

        AV_vector<int> right(SIZE);
        right = vc;
        EXPECT_EQ(right.size(), vc.size());
        EXPECT_EQ(right.capacity(), vc.capacity());
        EXPECT_NE(right.begin(), vc.begin());
        int rand_count = rand() % SIZE;
        EXPECT_EQ(right[rand_count], vc[rand_count]);
    }
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {

            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_constructed_with_id, SIZE);

        AV_vector<Obj> right(SIZE);
        right = vc;
        EXPECT_EQ(Obj::num_copied, SIZE);
        EXPECT_EQ(Obj::num_constructed_with_id, SIZE);
        EXPECT_EQ(right.size(), vc.size());
        EXPECT_EQ(right.capacity(), vc.capacity());
        EXPECT_NE(right.begin(), vc.begin());
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);

}

TEST(Vector, MoveAssign) {
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }

        AV_vector<int> right(SIZE);
        right = std::move(vc);
        EXPECT_EQ(vc.begin(), nullptr);
        EXPECT_NE(right.size(), vc.size());
        EXPECT_NE(right.capacity(), vc.capacity());
        try
        {
            int rand_count = rand() % SIZE;
            EXPECT_NE(right[rand_count], vc[rand_count]);
            EXPECT_EQ("No exception", "Error");
        }
        catch (const std::exception&)
        {
        }
    }
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_constructed_with_id, SIZE);
        AV_vector<Obj> right(SIZE);
        right = std::move(vc);
        EXPECT_EQ(Obj::num_assigned, 0);
        EXPECT_EQ(Obj::num_copied, 0);
        EXPECT_EQ(Obj::num_constructed_with_id, SIZE);
        EXPECT_EQ(vc.begin(), nullptr);
        EXPECT_NE(right.size(), vc.size());
        EXPECT_NE(right.capacity(), vc.capacity());
        try
        {
            int rand_count = rand() % SIZE;
            EXPECT_NE(right[rand_count].id, vc[rand_count].id);
            EXPECT_EQ("No exception", "Error");
        }
        catch (const std::exception&)
        {
        }
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}

TEST(Vector, Reserve) {
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    const size_t RSIZE = 60;
    {
        AV_vector<int> vc;
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }
        AV_vector<int> right(vc);
        vc.reserve(RSIZE);
        EXPECT_GE(vc.capacity(), RSIZE);
        EXPECT_EQ(vc.size(), SIZE);
        int rand_count = rand() % SIZE;
        EXPECT_EQ(vc[rand_count], right[rand_count]);

    }
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        vc.reserve(RSIZE);
        EXPECT_EQ(Obj::num_copied, 0);
        EXPECT_EQ(vc.size(), SIZE);
        EXPECT_GE(vc.capacity(), RSIZE);
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        AV_vector<Obj> right(vc);
        vc.reserve(RSIZE);
        int rand_count = rand() % SIZE;
        EXPECT_EQ(right[rand_count].id, vc[rand_count].id);
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);

    //act

    //assert
}

TEST(Vector, Resize) {
    //arrange
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    const size_t RSIZE = 40;
    const size_t GRSIZE = 60;
    {
        AV_vector<int> vc;
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }
        AV_vector<int> right(vc);
        vc.resize(RSIZE);
        EXPECT_EQ(vc.capacity(), RSIZE);
        EXPECT_EQ(vc.size(), RSIZE);
        int rand_count = rand() % RSIZE;
        EXPECT_EQ(vc[rand_count], right[rand_count]);
    }
    {
        AV_vector<Obj> vc;

        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        AV_vector<Obj> right(vc);
        vc.resize(RSIZE);
        EXPECT_EQ(Obj::GetAliveObjectCount(), SIZE + RSIZE);
        vc.resize(GRSIZE);
        EXPECT_EQ(Obj::GetAliveObjectCount(), SIZE + GRSIZE);
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}

TEST(Vector, pushBack) {
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc;
        int x = 177;
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(x);
        }
        EXPECT_EQ(vc.size(), SIZE);
        EXPECT_GE(vc.capacity(), SIZE);
        int rand_count = rand() % SIZE;
        EXPECT_EQ(vc[rand_count], 177);
    }
    {
        AV_vector<Obj> vc;
        Obj obj(177);
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(obj);
        }
        EXPECT_EQ(Obj::num_copied, SIZE);
        int rand_count = rand() % SIZE;
        EXPECT_EQ(vc[rand_count].id, obj.id);
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}

TEST(Vector, RVpushBack) {
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc;
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }
        EXPECT_EQ(vc.size(), SIZE);
        EXPECT_GE(vc.capacity(), SIZE);
        int rand_count = rand() % SIZE;
        EXPECT_EQ(vc[rand_count], rand_count);
    }
    {
        AV_vector<Obj> vc;
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        EXPECT_EQ(Obj::num_copied, 0);
        int rand_count = rand() % SIZE;
        EXPECT_EQ(vc[rand_count].id, rand_count);
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}

TEST(Vector, popBack) {
    Obj::ResetCounters();
    srand(time(NULL));
    const size_t SIZE = 50;
    {
        AV_vector<int> vc;
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(i);
        }
        vc.pop_back();
        vc.pop_back();
        EXPECT_EQ(vc.size(), SIZE - 2);
        EXPECT_EQ(vc[SIZE - 2], SIZE - 2);
        for (size_t i = 0; i < vc.size(); i++)
        {
            EXPECT_EQ(vc[i], i);
        }
        
    }
    {
        AV_vector<Obj> vc;
        for (size_t i = 0; i < SIZE; i++)
        {
            vc.push_back(Obj(i));
        }
        vc.pop_back();
        vc.pop_back();
        for (size_t i = 0; i < vc.size(); i++)
        {
            EXPECT_EQ(vc[i].id, i);
        }
    }
    EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}
