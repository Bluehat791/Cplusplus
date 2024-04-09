#pragma once
#include <memory>
#include <cstddef>
#include <utility>
#include <stdexcept>
#include <iterator>
#include <string>
#include <iostream>
#include <utility>
#include<forward_list>

template<typename T, typename Alloc = std::allocator<T>>
class List
{
	class List_Node
	{
	public:
		List_Node* pNext;
		T data;
		List_Node() : data(T()),pNext(nullptr) {}
		List_Node(const T& data, List_Node* pNext = nullptr)
		{
			this->data = data;
			this->pNext = pNext;
		}
		List_Node(T&& data, List_Node* pNext = nullptr): data(std::move(data)),pNext(pNext)
		{
		}

		List_Node& operator=(const List_Node& tmp) {
			this->data = tmp.data;
			this->pNext = tmp.pNext;
			return *this;
		}

		List_Node& operator=(List_Node&& tmp) {
			this->data = std::move(tmp.data);
			this->pNext = tmp.pNext;
			return *this;
		}

		void destroy_data() {
			data.~T();
		}

		~List_Node() {
			delete pNext;
		}
	};
	template<typename ValueType>
	struct List_iterator
	{
	private:
		List_Node* node_ = nullptr;
		friend class List;
		explicit List_iterator(List_Node* node) noexcept : node_(node) {}

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = ValueType*;
		using reference = ValueType&;

		List_iterator() = default;
		List_iterator(const List_iterator<T>& other) noexcept : node_(other.node_) {}
		List_iterator& operator=(const List_iterator& rhs) noexcept;

		bool operator== (const List_iterator<const T>& rhs) const noexcept;
		bool operator!= (const List_iterator<const T>& rhs) const noexcept;
		bool operator== (const List_iterator<T>& rhs) const noexcept;
		bool operator!= (const List_iterator<T>& rhs) const noexcept;

		List_iterator& operator++();
		List_iterator operator++(int);
		reference operator* () const;
		pointer operator-> () const;

	};
public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using size_type = unsigned int;
	using AllocTraits = std::allocator_traits<Alloc>;
	using iterator = List_iterator<T>;
	using const_iterator = List_iterator<const T>;

	List();
	List(const List& lst);
	List(List&& lst);

	~List();

	iterator before_begin() noexcept;
	const_iterator cbefore_begin() const noexcept;
	const_iterator before_begin() const noexcept;
	iterator begin() noexcept;
	iterator end() noexcept;
	const_iterator begin() const noexcept;
	const_iterator end() const noexcept;
	const_iterator cbegin() const noexcept;
	const_iterator cend() const noexcept;
	const_iterator find_element(int index);

	void clear();
	void pop_front();
	T& front();
	T& back();
	template<typename... Args>
	int GetSize() { return size; }
	T& operator[](const int& index);
	void push_front(const T& data);
	void push_front(T&& data);

	template<typename... Args>
	void emplace_front(Args&&... args);

	template<typename... Args>
	iterator emplace_after(const_iterator pos, Args&&... args);


	void resize(size_t count);
	void resize(size_t count, const T& value);
	bool empty();
	void swap(List& other);

	template<typename input_iterator>


	List<T>& operator=(const List<T>& lst);
	List& operator=(List&& lst);

	iterator InsertAfter(const_iterator pos, const T& value) {
		try
		{
			pos.node_->pNext = new List_Node(value, pos.node_->pNext);
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
			throw;
		}
		++size;
		return iterator(pos.node_->pNext);
	}

	iterator InsertAfter(const_iterator pos, T&& value) {
		try
		{
			pos.node_->pNext = new List_Node(std::move(value), pos.node_->pNext);
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
			throw;
		}
		++size;
		return iterator(pos.node_->pNext);
	}



	iterator EraseAfter(const_iterator pos) noexcept {

		List_Node* current = pos.node_->pNext;
		pos.node_->pNext = current->pNext;
		current->data.~T();
		size--;
		return iterator(pos.node_->pNext);
	}

	bool operator==(const List<T,Alloc>& other) const;
	bool operator!=(const List<T, Alloc>& other) const;


private:

	List_Node* head;
	int size;
	Alloc alloc;
	//using AllocTraits = std::allocator_traits<Alloc>;
};
//-- begin / and
template <typename T, typename Alloc>
typename List<T, Alloc>::iterator
List<T, Alloc>::before_begin() noexcept {
	return iterator(&head);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator
List<T, Alloc>::cbefore_begin() const noexcept {
	return const_iterator(const_cast<List_Node*>(&head));
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator
List<T, Alloc>::before_begin() const noexcept {
	return const_iterator(&head);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator
List<T, Alloc>::begin() noexcept {
	return iterator(head);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator
List<T, Alloc>::end() noexcept {
	if(head->pNext==nullptr)
		return iterator(head);
	List_Node* current(head);
	while (current->pNext != nullptr)
	{
		current = current->pNext;

	}


	return iterator(current);
}

template <typename T, typename Alloc>
[[nodiscard]] typename List<T, Alloc>::const_iterator
List<T, Alloc>::begin() const noexcept {
	return const_iterator(head->pNext);
}

template <typename T, typename Alloc>
[[nodiscard]] typename List<T, Alloc>::const_iterator
List<T, Alloc>::end() const noexcept {
	return const_iterator();
}

template <typename T, typename Alloc>
[[nodiscard]] typename List<T, Alloc>::const_iterator
List<T, Alloc>::cbegin() const noexcept {
	return const_iterator(head->pNext);
}

template <typename T, typename Alloc>
[[nodiscard]] typename List<T, Alloc>::const_iterator
List<T, Alloc>::cend() const noexcept {
	return const_iterator();
}

template<typename T, typename Alloc>
inline typename List<T, Alloc>::const_iterator List<T, Alloc>::find_element(int index)
{
	if (index > size)
		throw std::out_of_range("oops");
	List_Node* current(head);
	int count = 0;
	while (count != index)
	{
		current = current->pNext;
		count++;
	}
	return const_iterator(current);
}

//------------------ Iterators method implementation
template <typename T, typename Alloc>
template <typename ValueType>
typename List<T, Alloc>::template List_iterator<ValueType>&
List<T, Alloc>::List_iterator<ValueType>::
operator=(const List_iterator& rhs) noexcept {
	node_ = rhs.node_;
	return *this;
}

template <typename T, typename Alloc>
template <typename ValueType>
[[nodiscard]] bool List<T, Alloc>::List_iterator<ValueType>::
operator==(const List_iterator<const T>& rhs) const noexcept {
	return node_ == rhs.node_;
}

template <typename T, typename Alloc>
template <typename ValueType>
[[nodiscard]] bool List<T, Alloc>::List_iterator<ValueType>::
operator!=(const List_iterator<const T>& rhs) const noexcept {
	return node_ != rhs.node_;
}

template <typename T, typename Alloc>
template <typename ValueType>
[[nodiscard]] bool List<T, Alloc>::List_iterator<ValueType>::
operator==(const List_iterator<T>& rhs) const noexcept {
	return node_ == rhs.node_;
}

template <typename T, typename Alloc>
template <typename ValueType>
[[nodiscard]] bool List<T, Alloc>::List_iterator<ValueType>::
operator!=(const List_iterator<T>& rhs) const noexcept {
	return node_ != rhs.node_;
}

template <typename T, typename Alloc>
template <typename ValueType>
typename List<T, Alloc>::template List_iterator<ValueType>&
List<T, Alloc>::List_iterator<ValueType>::operator++() {
	if (node_ == nullptr) {
		throw std::out_of_range("Invalid iterator");
	}
	node_ = node_->pNext;
	return *this;
}

template <typename T, typename Alloc>
template <typename ValueType>
typename List<T, Alloc>::template List_iterator<ValueType>
List<T, Alloc>::List_iterator<ValueType>::operator++(int) {
	if (node_ == nullptr) {
		throw std::out_of_range("Invalid iterator");
	}
	auto old_value(*this);
	++(*this);
	return old_value;
}

template <typename T, typename Alloc>
template <typename ValueType>
[[nodiscard]] typename List<T, Alloc>::template List_iterator<ValueType>::reference
List<T, Alloc>::List_iterator<ValueType>::operator*() const {
	if (node_ == nullptr) {
		throw std::out_of_range("Invalid iterator");
	}
	return node_->data;
}

template <typename T, typename Alloc>
template <typename ValueType>
[[nodiscard]] typename List<T, Alloc>::template List_iterator<ValueType>::pointer
List<T, Alloc>::List_iterator<ValueType>::operator->() const {
	if (node_ == nullptr) {
		throw std::out_of_range("Invalid iterator");
	}
	return &(node_->value);
}


//------------------


template<typename T, typename Alloc>
List<T, Alloc>::List()
{
	head = new List_Node();
	size = 0;

}

template <typename T, typename Alloc>
inline List<T, Alloc>::List(const  List<T, Alloc>& lst)
{	
	size = 0;
	head = nullptr;
	List_Node* current = lst.head;
	List<T> tmp;
	for (size_t i = lst.size; i > 0; i--)
	{
		tmp.push_front(current->data);
		current = current->pNext;
	}
	swap(tmp);
}

template<typename T, typename Alloc>
inline List<T, Alloc>::List(List&& lst)
{
	size = 0;
	head = nullptr;
	List_Node* current = lst.head;
	List<T> tmp;
	for (size_t i = lst.size; i > 0; i++)
	{
		tmp.push_front(std::move(current->data));
		current = current->pNext;
	}
	swap(tmp);
}

template <typename T, typename Alloc>
List<T, Alloc>::~List()
{
	clear();
	delete head;
}

template<typename T, typename Alloc>
void List<T, Alloc>::clear()
{
	while (this->size)
	{
		pop_front();
	}
}

template<typename T, typename Alloc>
void List<T, Alloc>::pop_front()
{	
	List_Node* current(head);
	head = head->pNext;
	current->destroy_data();
	--size;
}

template<typename T, typename Alloc>
inline T& List<T, Alloc>::front()
{	
	return head->data;
}

template<typename T, typename Alloc>
inline T& List<T, Alloc>::back()
{
	List_Node* current = this->head;
	while (current->pNext != nullptr)
	{
		current = current->pNext;
	}
	return current->data;

}

template<typename T, typename Alloc>
T& List<T, Alloc>::operator[](const int& index)
{
	if (0 == index)
	{
		return front();
	}
	if (index > size)
		return back();
	int count = 0;
	List_Node* current = this->head;
	while (count != index )
	{

		current = current->pNext;
		count++;
	}
	return current->data;
}

template<typename T, typename Alloc>
template<typename ...Args>
inline void List<T, Alloc>::emplace_front(Args && ...args)
{
	try {
		head = new List_Node(std::forward<Args>(args)..., head);
	} 
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		throw;
	}
	++size;
}

template<typename T, typename Alloc>
template<typename ...Args>
inline typename List<T, Alloc>::iterator List<T, Alloc>::emplace_after(const_iterator pos, Args&& ...args)
{
	try
	{
		pos.node_->pNext = new List_Node(std::forward<Args>(args)..., pos.node_->pNext);
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		throw;
	}
	++size;
	return iterator(pos.node_->pNext);
}



template<typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& data)
{	
	List_Node* current(head);
	try
	{
		head = new List_Node(data, current);
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		delete current;
		throw;
	};
	++size;
}

template<typename T, typename Alloc>
void List<T, Alloc>::push_front(T&& data) {

	List_Node* current(head);
	try
	{
		head = new List_Node(std::move(data), current);
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		delete current;
		throw;
	};
	++size;
}

template<typename T, typename Alloc>
inline void List<T, Alloc>::resize(size_t count)
{
	if (count < size) {
		while (size != count)
			pop_front();
	}
	else {
		for (size_t i = count; i < size; ++i)
		{
			this->push_front(T());
		}
	}
}

template<typename T, typename Alloc>
inline void List<T, Alloc>::resize(size_t count, const T& value)
{
	if (count < size) {
		while (size != count)
			pop_front();
	}
	else {
		int tmpSize = size;
		for (size_t i = 0; i < count - tmpSize; ++i)
		{
			this->push_front(value);
		}
	}
}

template<typename T, typename Alloc>
inline bool List<T, Alloc>::empty()
{
	return begin() == end();
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::swap(List& other)
{
	std::swap(size, other.size);
	std::swap(head, other.head);
}

template<typename T, typename Alloc>
template<typename input_iterator>
inline List<T>& List<T, Alloc>::operator=(const List<T>& lst)
{
	if (this != &lst) {
		List<T> tmp(lst);
		swap(tmp);
	}
	return *this;
}

template<typename T, typename Alloc>
inline List<T, Alloc>& List<T, Alloc>::operator=(List&& lst)
{
	head = lst.head;
	size = lst.size;
	lst.head = nullptr;
	lst.size = 0;
	return *this;
}

template<typename T, typename Alloc>
inline bool List<T, Alloc>::operator==(const List<T, Alloc>& other) const
{	
	if (size != other.size)
		return false;
	List_Node* current = head;
	List_Node* rhs = other.head;
	for (size_t i = 0; i < size; i++)
	{
		if (current->data != rhs->data)
			return false;
		current = current->pNext;
		rhs = rhs->pNext;
	}
	return true;
}

template<typename T, typename Alloc>
inline bool List<T, Alloc>::operator!=(const List<T, Alloc>& other) const
{
	if (size != other.size)
		return true;

	List_Node* current = head;
	List_Node* rhs = other.head;
	for (size_t i = 0; i < size; i++)
	{
		if (current->data != rhs->data)
			return true;
		current = current->pNext;
		rhs = rhs->pNext;
	}
	return false;
}



