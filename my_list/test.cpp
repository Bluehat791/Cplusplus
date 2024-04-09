#include "AV_List.h"
#include <gtest/gtest.h>

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
		bool IsAlive() const noexcept {
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

		bool operator==(const Obj& rhs) {
			if (id == rhs.id)
				return true;
			return false;
		}

		bool operator!=(const Obj& rhs) {
			if (id == rhs.id)
				return false;
			return true;
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

TEST(List, empty) {
	const int SIZE = 20;
	{
		List<int> lst;
		EXPECT_EQ(lst.GetSize(), 0);
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(i);
		}

		EXPECT_EQ(lst.GetSize(), 20);
		EXPECT_EQ(lst.empty(), false);

	}
}
TEST(List, clear) {
	const int SIZE = 20;
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(i);
		}

		EXPECT_EQ(lst.GetSize(), 20);
		lst.clear();
		EXPECT_EQ(lst.GetSize(), 0);
		EXPECT_EQ(lst.empty(), true);
	}
	//extra copying due to anon object
	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(Obj(i));
		}
		EXPECT_EQ(Obj::num_constructed_with_id, SIZE);
		lst.clear();
		EXPECT_EQ(Obj::num_moved, 20);
	}

	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}
TEST(List, insert_after) {
	const int SIZE = 20;
	//srand(time(NULL));
	Obj::ResetCounters();
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(i);
		}
		List<int> other = lst;
		other.InsertAfter(other.find_element(13), 77);
		//EXPECT_NE(lst, other);
		//EXPECT_NE(other.GetSize(), SIZE);
		EXPECT_EQ(other.GetSize(), SIZE + 1);
		EXPECT_EQ(other[14], 77);
		EXPECT_NE(other, lst);
	}

	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(Obj(i));
		}
		List<Obj> other = lst;
		other.InsertAfter(other.find_element(13), Obj(77));
		EXPECT_EQ(other.GetSize(), SIZE + 1);
		EXPECT_EQ(other[14].id, 77);
		EXPECT_EQ(other.GetSize(), SIZE + 1);
		EXPECT_NE(other, lst);
	}
	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}
TEST(List, emplace_after) {
	const int SIZE = 20;
	Obj::ResetCounters();
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(i);
		}
		lst.emplace_after(lst.find_element(5), 77);
		lst.emplace_after(lst.find_element(7), 88);
		EXPECT_EQ(lst.GetSize(), SIZE + 2);
		EXPECT_EQ(lst[6], 77);
		EXPECT_EQ(lst[8], 88);
	}
	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(Obj(i));
		}
		EXPECT_EQ(Obj::num_moved, SIZE);
		EXPECT_EQ(Obj::num_copied, 0);
		lst.emplace_after(lst.find_element(5), Obj(77));
		lst.emplace_after(lst.find_element(7), Obj(88));
		EXPECT_EQ(lst[6].id, 77);
		EXPECT_EQ(lst[8].id, 88);
		EXPECT_EQ(lst.GetSize(), SIZE + 2);
	}


	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}
TEST(List, erase_after) {
	const int SIZE = 20;
	Obj::ResetCounters();
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(i);
		}
		lst.emplace_after(lst.find_element(5), 77);
		EXPECT_EQ(lst[6], 77);
		lst.EraseAfter(lst.find_element(5));
		EXPECT_EQ(lst.GetSize(), SIZE);
		EXPECT_NE(lst[6], 77);
	}

	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(Obj(i));
		}
		lst.emplace_after(lst.find_element(5), Obj(77));
		EXPECT_EQ(lst[6].id, 77);
		lst.EraseAfter(lst.find_element(5));
		EXPECT_EQ(lst.GetSize(), SIZE);
		EXPECT_NE(lst[6].id, 77);
		//EXPECT_EQ(Obj::num_destroyed, 1);
	}
	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}
TEST(List, push_front) {
	const int SIZE = 20;
	Obj::ResetCounters();
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(i);
		}
		EXPECT_EQ(lst.GetSize(), SIZE);
		for (size_t i = 0; i < SIZE; i++)
		{
			EXPECT_EQ(lst[i], SIZE - i - 1);
		}
	}
	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(Obj(i));
		}
		EXPECT_EQ(SIZE, Obj::num_moved);
		EXPECT_EQ(Obj::num_copied, 0);
	}
	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}
TEST(List, emplace_front) {
	const int SIZE = 20;
	Obj::ResetCounters();
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.emplace_front(i);
		}

		for (size_t i = 0; i < SIZE; i++)
		{
			EXPECT_EQ(lst[i], SIZE - i - 1);
		}
	}
	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.emplace_front(Obj(i));
		}
		//with rvalue-ref there is movement
		EXPECT_EQ(Obj::num_moved, SIZE);
		Obj obj(13);
		lst.emplace_front(obj);

		//there should be no movement and copied
		EXPECT_EQ(Obj::num_moved, SIZE);
		EXPECT_EQ(Obj::num_copied, 0);
		EXPECT_EQ(Obj::num_assigned, 1);

	}
	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);

}
TEST(List, pop_front) {
	const int SIZE = 20;
	Obj::ResetCounters();
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.emplace_front(i);
		}

		lst.pop_front();
		lst.pop_front();
		EXPECT_NE(lst.GetSize(), SIZE);
		EXPECT_EQ(lst.GetSize(), SIZE - 2);
		for (size_t i = 0; i < SIZE - 2; i++)
		{
			//size fw-list now 18
			EXPECT_EQ(lst[i], SIZE - i - 3);
		}
	}
	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.emplace_front(Obj(i));
		}
		lst.pop_front();
		lst.pop_front();
		EXPECT_EQ(Obj::num_destroyed, SIZE+2);
		for (size_t i = 0; i < SIZE - 2; i++)
		{
			//size fw-list now 18
			EXPECT_EQ(lst[i].id, SIZE - i - 3);
		}
		EXPECT_EQ(Obj::num_copied, 0);
	}
	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}
TEST(List, resize) {
	const int SIZE = 20;
	const int newSize = 15;
	const int newUpsize = 25;
	Obj::ResetCounters();
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(i);
		}
		lst.resize(15);
		EXPECT_EQ(lst.GetSize(), newSize);
		for (size_t i = 0; i < newSize; i++)
		{
			EXPECT_EQ(lst[i], newSize-i-1);
		}
			try
			{
				lst.find_element(newSize + 3);
			}
			catch (std::out_of_range) {
			} 
			catch (...) {
				EXPECT_EQ("", "not out of range");
			}

		lst.resize(newUpsize, 4);
		for (size_t i = 0; i < newUpsize-newSize; i++)
		{
			EXPECT_EQ(lst[i], 4);
		}
	}
	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.emplace_front(Obj(i));
		}
		lst.resize(newSize);
		EXPECT_NE(Obj::GetAliveObjectCount(), SIZE);
		EXPECT_EQ(Obj::num_destroyed, SIZE + (SIZE - newSize));
		lst.resize(newUpsize, Obj(4));
		EXPECT_NE(Obj::GetAliveObjectCount(), newSize);

	}
	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}
TEST(List, swap) {
	const int SIZE = 20;
	Obj::ResetCounters();
	{
		List<int> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.push_front(i);
		}
		List<int> lst2;
		lst2.resize(SIZE, 4);
		lst2.swap(lst);
		for (size_t i = 1; i < SIZE; i++)
		{
			EXPECT_EQ(lst2[i], SIZE-i-1);
		}
		for (size_t i = 0; i < SIZE; i++)
		{
			EXPECT_EQ(lst[i], 4);
		}
	}
	{
		List<Obj> lst;
		for (size_t i = 0; i < SIZE; i++)
		{
			lst.emplace_front(Obj(i));
		}
		EXPECT_EQ(Obj::num_constructed_with_id, SIZE);
		List<Obj> lst2;
		lst2.resize(SIZE, Obj(4));
		lst.swap(lst2);
		EXPECT_EQ(Obj::GetAliveObjectCount(), (SIZE*2)+2);
	}
	EXPECT_EQ(Obj::GetAliveObjectCount(), 0);
}



