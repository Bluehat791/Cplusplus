#pragma once
#include <memory>
#include <cstddef>
#include <utility>
#include <stdexcept>
#include <iterator>
#include <string>
#include <iostream>




template<typename T,typename Alloc = std::allocator<T>>
class List
{
	class List_Node
	{
	public:
		List_Node* pNext;
		T data;
		List_Node(T data = T(), List_Node* pNext = nullptr)
		{
			this->data = data;
			this->pNext = pNext;
		}

		List_Node& operator=(List_Node& tmp) {
			this->data = tmp.data;
			this->pNext = tmp.pNext;
			return *this;
		}
	};
	template<typename value_type>
	struct List_iterator
	{	
		friend class List;
		using difference_type  = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;
		using pointer = value_type*;
		using reference = value_type&;
		using List_iterator_const = List_iterator<const value_type>;

		List_iterator() noexcept : m_node() 
		{}

		explicit List_iterator(List_Node* ptr = nullptr) noexcept
		: m_node(ptr) 
		{}

		List_iterator(const List_iterator& iter);

		~List_iterator() = default;

		List_iterator& operator=(const List_iterator& lst) = default; 



		reference operator*() const noexcept
		{
			return this->m_node.data;
		}

		pointer operator->() const noexcept {
			return &(this->m_node.data);
		}
		
		List_iterator<value_type>& operator++() noexcept
		{
			m_node = m_node->pnext;
			return *this;
		}
		List_iterator<value_type>& operator++(int) noexcept
		{   
			List_iterator<value_type> tmp(this);
			m_node = m_node->pnext;
			return *this;
		}

		//??
		friend bool operator==(const List_iterator<value_type>& x,List_iterator<value_type>& y) noexcept
		{
			return x.m_node == y.m_node;
		}

		friend bool operator!=(const List_iterator<value_type>& x,List_iterator<value_type>& y) noexcept
		{
			return x.m_node != y.m_node;
		}

		private:
		List_Node* m_node;
	};
public:


    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = unsigned int;
	using AllocTraits = std::allocator_traits<Alloc>;
	using iterator = List_iterator<T>;
	using const_iterator = List_iterator<const T>;
	
	//List() = default;
	List(const List& lst);
	List();
	~List();

	void clear();
	void pop_front();
	void push_back(T data);
	int GetSize() { return size; }
	T& operator[](const int& index);
	void push_front(T data);
	void insert(const T& value, int index);
	void removeAt(int index);
	void pop_back();
	List_iterator<T> begin() noexcept
	{
		return List_iterator<T>(this->head);
	}

	List_iterator<T> end() noexcept
	{
		List_Node* current = this->head;
		while (current!=nullptr)
		{
			current = current->pNext;
		}
		return current;
	}

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	const_iterator cbegin() const noexcept {
		return const_iterator(head);
	}

	const_iterator cend() const noexcept {
		return end();
	}

	iterator before_begin() const noexcept {
		return iterator(&head);
	}

	const_iterator cbefore_begin() const noexcept {
		return const_iterator(const_cast<List_Node*>(&head));
	}

	/*const_iterator before_begin() const noexcept {
		return cbefore_begin();
	}*/

	

	void swap(List& lst);
	
	template<typename input_iterator>
	void MakeList(input_iterator first,input_iterator last);


	List& operator=(const List& lst);
	iterator InsertAfter(const_iterator pos,const T& value) {
		pos.m_node->pNext = new List_Node(value,pos.m_node->pNext);
		++size;
		return iterator(pos.m_node->pNext);
	}

	iterator EraseAfter(const_iterator pos) noexcept {
		auto tmp = (pos.m_node->pNext) ? pos.m_node->pNext->pnext: nullptr;
		delete pos.m_node->pNext;
		pos.m_node->pNext = tmp;
		--size;
		return iterator(pos.m_node->pNext);
	}

	bool operator==(const List<T>& other);
    

private:
	List_Node* head;
	int size;

};

template<typename T,typename Alloc>
List<T,Alloc>::List()
{
	size = 0;
	head = nullptr;
}

template <typename T, typename Alloc>
inline List<T, Alloc>::List(const  List<T, Alloc>& lst)
{
	MakeList(lst.begin(),lst.end());
}

template <typename T, typename Alloc>
List<T, Alloc>::~List()
{
	clear();
	delete head;
}

template<typename T,typename Alloc>
void List<T,Alloc>::clear()
{
	while (size)
	{
		pop_front();
	}
}

template<typename T,typename Alloc>
void List<T,Alloc>::pop_front()
{
	List_Node* temp = head;
	head = head->pNext;
	delete temp;
	--size;
}

template<typename T,typename Alloc>
void List<T,Alloc>::push_back(T data)
{
	if (head == nullptr)
		head = new List_Node(data);
	List_Node* current = this->head;
	while (current->pNext!=nullptr)
	{
		current = current->pNext;
	}
	current->pNext = new List_Node(data);

	++size;
}

template<typename T,typename Alloc>
T& List<T,Alloc>::operator[](const int& index)
{	
	int count = 0;
	List_Node* current = this->head;
	while (current!=nullptr)
	{
		if (count == index)
		{
			return current->data;
		}
		current = current->pNext;
		count++;
	}
	return current->data;
}

template<typename T,typename Alloc>
void List<T,Alloc>::push_front(T data)
{
	head = new List_Node(data,head);
	size++;
}

template<typename T,typename Alloc>
void List<T,Alloc>::insert(const T& value, int index)
{
	if (index==0)
	{
		push_front(value);
	}
	List_Node* previous = this->head;
	 
	for (int i = 0; i < index-1; i++)
	{
		previous = previous->pNext;
	}

	List_Node* newNode = new List_Node(value, previous->pNext);
	previous->pNext = newNode;
	size++;
}

template<typename T,typename Alloc>
void List<T,Alloc>::removeAt(int index)
{	
	if (index == 0)
	{
		pop_front();
	}
	else {
		List_Node previous = this->head;
		for (int i = 0; i < index-1; i++)
		{
			previous = previous->pNext;
		}

		List_Node* toDelete = previous->pNext;
		delete toDelete;
		size--; 
	}
}

template<typename T,typename Alloc>
void List<T,Alloc>::pop_back()
{
	removeAt(size - 1); 
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::swap(List &lst)
{
	std::swap(size,lst,size);
	std::swap(head,lst.head);
}

template <typename T, typename Alloc>
inline List<T, Alloc> &List<T, Alloc>::operator=(const List &lst)
{
	if(this!=&lst) {
		List<T> tmp(lst);
		swap(tmp);
	}
	return *this;
}

template <typename T, typename Alloc>
inline bool List<T, Alloc>::operator==(const List<T> &other)
{	
    return this->head == other->head;
}

template <typename T, typename Alloc>
template <typename input_iterator>
inline void List<T, Alloc>::MakeList(input_iterator first, input_iterator last)
{
	size=0;
	List<T> tmp;
	List_Node* temp = &tmp.head;
	for (; first!=last; ++first)
	{
		temp->pNext = new List_Node(*first,temp->pNext);
		temp = temp->pNext;
		++tmp.size;
	}

	swap(tmp);
	
}
