#pragma once

#include <memory>
#include "iterator/iterator.hpp"

namespace ft {

template < class T, class Alloc = std::allocator<T> >
class vector {
public:
	
	/*
	___  ___               _                 _____                     
	|  \/  |              | |               |_   _|                    
	| .  . | ___ _ __ ___ | |__   ___ _ __    | |_   _ _ __   ___  ___ 
	| |\/| |/ _ \ '_ ` _ \| '_ \ / _ \ '__|   | | | | | '_ \ / _ \/ __|
	| |  | |  __/ | | | | | |_) |  __/ |      | | |_| | |_) |  __/\__ \ 
	\_|  |_/\___|_| |_| |_|_.__/ \___|_|      \_/\__, | .__/ \___||___/
	                                              __/ | |              
	                                             |___/|_|         
	*/

	typedef T					value_type;
	typedef Alloc				allocator_type;

	// for default allocator : value_type &
	typedef typename allocator_type::reference				reference;
	// for default allocator : const value_type &
	typedef typename allocator_type::const_reference		const_reference;
	// for default allocator : value_type *
	typedef typename allocator_type::pointer				pointer;
	// for default allocator : const value_type *
	typedef typename allocator_type::const_pointer			const_pointer;

	// Iterators point to an element in a vector and point to the next one when incremented
	typedef ft::random_access_iterator<pointer>					iterator;
	// A const iterator is an iteraror that points to const content
	typedef ft::random_access_iterator<const_pointer>			const_iterator;
	// Reverse iterators point to an element in a vector and point to the previous one when incremented
	typedef ft::random_access_reverse_iterator<iterator>		reverse_iterator;
	// A const reverse iterator is a reverse iterator that points to const content
	typedef ft::random_access_reverse_iterator<const_iterator>	const_reverse_iterator;

	// size_type is size of type
	typedef std::size_t			size_type;
	typedef std::ptrdiff_t		difference_type;

	/*
	 _____                         _           _ 
	/  __ \                       (_)         | |
	| /  \/ __ _ _ __   ___  _ __  _  ___ __ _| |
	| |    / _` | '_ \ / _ \| '_ \| |/ __/ _` | |
	| \__/\ (_| | | | | (_) | | | | | (_| (_| | |
	 \____/\__,_|_| |_|\___/|_| |_|_|\___\__,_|_| 
	*/

	// Constucts an empty vector, with no elements
	explicit vector (void) : _array(NULL), _size(0), _capacity(0) {} // default

	// Constructs a vector with n elements
	// Each element is a copy of val
	explicit vector (size_type n, const value_type & val = value_type()) : _size(n), _capacity(n){ // fill
		this->_array = this->_alloc.allocate(this->_capacity);
		for (size_type i = 0; i < this->_size; i++) {
			this->_alloc.construct(this->_array + i, val);
		}
	}

	// Constructs a vector with as many elements as the range [first, last],
	// with each element constructed from its corresponding element
	// in that range, in the same order
	template < class InputIterator >
	// using enable_if here forbids any integral type to get into this function
	// so that only iterators may use this function
	vector (InputIterator first, InputIterator last, class enable_if<!is_integral<InputIterator>::value, InputIterator>::type* = NULL) { // range
		this->_size = ft::distance(first, last);
		this->_capacity = this->_size;
		this->_array = this->_alloc.allocate(this->_capacity);
		for (size_type i = 0; i < this->_size; i++) {
			this->_alloc.construct(this->_array + i, *first);
			first++;
		}
	}
	
	// Constructs a vector with a copy of each of the elements in src, in the same order
	vector (const vector & src) { // copy
		const_iterator index = src.begin();
		this->_size = src._size;
		this->_capacity = src._capacity;
		this->_alloc = src._alloc;
		this->_array = this->_alloc.allocate(this->_capacity);
		for (size_type i = 0; i < this->_size; i++) {
			this->_alloc.construct(this->_array + i, *index);
			index++;
		}
	}

	// Destructor
	~vector (void) {
		this->_reduce_size(0);
		this->_alloc.deallocate(this->_array, this->_capacity);
	}
	
	// Assignment operator
	vector & operator= (vector const & src) {
		const_iterator index = src.begin();
		this->_size = src._size;
		this->_capacity = src._capacity;
		this->_alloc = src._alloc;
		this->_array = this->_alloc.allocate(this->_capacity);
		for (size_type i = 0; i < this->_size; i++) {
			this->_alloc.construct(this->_array + i, *index);
			index++;
		}
		return *this;
	}
	
	/*
	 _____ _                 _                 
	|_   _| |               | |                
	  | | | |_ ___ _ __ __ _| |_ ___  _ __ ___ 
	  | | | __/ _ \ '__/ _` | __/ _ \| '__/ __|
	 _| |_| ||  __/ | | (_| | || (_) | |  \__ \ 
	 \___/ \__\___|_|  \__,_|\__\___/|_|  |___/
	*/

	// Out of bounds iterators shall not be dereferenced
	// iterator, reverse_iterator, const_iterator and const_reverse_iterator 
	// are random access iterators (RAI)

	// Returns an iterator pointing to the first element in the vector
	iterator				begin () {
		return iterator(this->_array);
	}
	// If the vector is const-qualified
	const_iterator			begin () const {
		return const_iterator(this->_array);
	}

	// Returns an iterator pointing to the past-the-end element in the vector
	iterator				end () {
		return iterator(this->_array + this->_size);
	}
	// If the vector is const-qualified
	const_iterator			end () const {
		return const_iterator(this->_array + this->_size);
	}

	// Returns a reverse iterator pointing to the last element in the vector
	reverse_iterator		rbegin () {
		return reverse_iterator(this->_array + this->_size);
	}
	// If the vector is const qualifed
	const_reverse_iterator	rbegin () const {
		return const_reverse_iterator(this->_array + this->_size);
	}

	// Returns a reverse iterator pointing to the theroritocal element preceding the 
	// first element in the vector (which is considered its reverse end)
	reverse_iterator		rend () {
		return reverse_iterator(this->_array);
	}
	// If the vector is const qualifed
	const_reverse_iterator	rend () const {
		return const_reverse_iterator(this->_array);
	}

	/*
	 _____                        _ _         
	/  __ \                      (_) |        
	| /  \/ __ _ _ __   __ _  ___ _| |_ _   _ 
	| |    / _` | '_ \ / _` |/ __| | __| | | |
	| \__/\ (_| | |_) | (_| | (__| | |_| |_| |
	 \____/\__,_| .__/ \__,_|\___|_|\__|\__, |
	            | |                      __/ |
	            |_|                     |___/   
	*/

	// Capacity and size won't always be the same
	// Capacity may be equal or greater than size to acommodate for future growth of the vector 
	// without having to reallocate the whole array

	// Returns the number of elements in the vector
	// This is the number of actual objects held in the vector, 
	// which is not necessarily equal to its storage capacity
	size_type	size () const {
		return this->_size;
	}

	// Returns the maximum number of elements that the vector can hold
	// This is the maximum potential size the container can reach due to known system
	// or library implementation limitations, but the container is by no means
	// guaranteed to be able to reach that size: it can still fail to allocate storage
	// at any point before that size is reached
	size_type	max_size () const {
		return this->_alloc.max_size();
	}

	// Resizes the container so that it contains n elements
	void		resize (size_type n, value_type val = value_type()) {
		// If n is equal to current container size, does nothing
		if (n == this->_size)
			return ;

		// If n is smaller than the current container size, the content is reduced 
		// to its first n elements, removing those beyong and destroying them
		if (n < this->_size)
			this->_reduce_size(n);

		// If n is greater than the current container capacity, an automatic reallocation 
		// of the allocated storage space takes place
		if (n > this->_capacity)
			this->reserve(n);

		// If n is greater than current container size, the content is expanded by 
		// inserting at the as many elements as needed to reach a size of n
		// If val is specififed, the new elements are initialized as copies of val, 
		// otherwise, they are value-initialized
		if (n > this->_size)
			this->_increase_size(n, val);
	}

	// Returns the size of the storage space currently allocated for the vector, 
	// expressed in terms of elements
	size_type	capacity () const {
		return this->_capacity;
	}

	// Returns whether the vector is empty (i.e. whether its size is 0)
	bool		empty () const {
		return (this->_size == 0);
	}

	// Requests that the vector capacity be at least enough to contain n elements
	void		reserve (size_type n) {
		// If n is greater than max_size, throw an exception
		if (n > this->max_size())
			throw std::length_error("Cannot allocate size\n");
		
		// If n is greater than the current vector capacity, the function causes to 
		// reallocate its storage increasing its capacity to n (or greater)
		if (n > this->_capacity) {
			pointer tmp = this->_array;
			iterator until = tmp + this->_size;
			this->_array = this->_alloc.allocate(n);
			size_type i = 0;
			for (iterator from = tmp; from != until; from++) {
				this->_alloc.construct(this->_array + i, *from);
				i++;
			}
			for (size_type i = 0; i < this->_size; i++) {
				this->_alloc.destroy(tmp + i);
			}
			this->_alloc.deallocate(tmp, this->_capacity);
			this->_capacity = n;
		}

		// In all other cases, the function call does not cause a reallocation 
		// and the vector capacity is not affected
	}

	/*
	 _____ _                           _      ___                        
	|  ___| |                         | |    / _ \                       
	| |__ | | ___ _ __ ___   ___ _ __ | |_  / /_\ \ ___ ___ ___  ___ ___ 
	|  __|| |/ _ \ '_ ` _ \ / _ \ '_ \| __| |  _  |/ __/ __/ _ \/ __/ __|
	| |___| |  __/ | | | | |  __/ | | | |_  | | | | (_| (_|  __/\__ \__ \ 
	\____/|_|\___|_| |_| |_|\___|_| |_|\__| \_| |_/\___\___\___||___/___/ 
	*/

	// Difference between at() and operator[] is that at() is safer and can throw an exception
	// Difference between front() and begin() is that begin() returns a reference instead of an iterator

	// Returns a reference to the element at position n in the vector container
	reference			operator[] (size_type n) {
		return this->_array[n];
	}
	// If the vector is const-qualifed
	const_reference		operator[] (size_type n) const {
		return this->_array[n];
	}

	// Returns a reference to the element at position n in the vector
	reference			at (size_type n) {
		if (n > (this->_size - 1))
			throw std::out_of_range("Out of bounds\n");
		return this->_array[n];
	}
	// If the vector is const-qualifed
	const_reference		at (size_type n) const {
		if (n > (this->_size - 1))
			throw std::out_of_range("Out of bounds\n");
		return this->_array[n];
	}

	// Returns a reference to the first element in the vector
	reference			front () {
		return *(this->_array);
	}
	// If the vector is const-qualified
	const_reference		front () const {
		return *(this->_array);
	}

	// Returns a reference to the last element in the vector
	reference			back () {
		return this->_array[this->_size - 1];
	}
	// If the vector is const-qualified
	const_reference		back () const {
		return this->_array[this->_size - 1];
	}

	/*
	___  ___          _ _  __ _               
	|  \/  |         | (_)/ _(_)              
	| .  . | ___   __| |_| |_ _  ___ _ __ ___ 
	| |\/| |/ _ \ / _` | |  _| |/ _ \ '__/ __|
	| |  | | (_) | (_| | | | | |  __/ |  \__ \ 
	\_|  |_/\___/ \__,_|_|_| |_|\___|_|  |___/ 
	*/

	// Asign new contents to the vector, replacing its current contents, and modifying its size accordingly
	// Any elements held in the container before the call are destroyed and replaced by newly constructed
	// elements (no assignments of elements take place)
	template	< class InputIterator >
	void		assign (InputIterator first, InputIterator last, class enable_if<!is_integral<InputIterator>::value, InputIterator>::type* = NULL) { // range
		this->_reduce_size(0);
		this->reserve(_next_pow_two(ft::distance(first, last)));
		// adding each element of the first to last iterator range
		// each time increasing size by one and checking if we ever need a reallocation
		while (first != last) {
			this->_increase_size(this->_size + 1, *first);
			first++;
		}
	}
	void		assign (size_type n, const value_type & val) { // fill
		this->_reduce_size(0);
		this->reserve(_next_pow_two(n));
		this->_increase_size(n, val);
	} 
	
	// Adds a new element at the end of the vector, after its current last element
	// The content of val is copied (or moved) to the new element
	void		push_back (const value_type & val) {
		if (this->_size + 1 > this->_capacity)
			this->reserve(_next_pow_two(this->_size + 1));
		this->_increase_size(this->_size + 1, val);
	}

	// Removes the last element in the vector, effectively reducing the container size by one
	// This destroys the removed element
	void		pop_back () {
		this->_reduce_size(this->_size - 1);
	}

	// The vector is extended by inserting new elements before the element at the specified position,
	// effectively increasing the container size by the number of elements inserted
	// Inserting elements in positions other than the vector end causes the vector to relocate all
	// the elements that were after position to their new positions
	iterator	insert (iterator position, const value_type & val) { // single element
		// diff is distance from position to the end
		difference_type diff = distance(position, this->end());
		if (this->_size + 1 > this->_capacity)
			this->reserve(_next_pow_two(this->_size + 1));
		// tmp is size before any changes to it
		size_type	tmp = this->_size;
		// adding the new element at the end of the vector
		this->_increase_size(this->_size + 1, val);
		// updating position in case of reallocation of array if size increase surpasses capacity
		position = this->begin() + tmp - diff;
		// cur_position is the current position of the newly inserted elements
		iterator	cur_position = this->_array + tmp;
		while (cur_position > position) {
			// moving the new element backwards until we arrive at desired position
			this->_move_back(cur_position - this->begin());
			cur_position--;
		}
		return cur_position;
	}
	void		insert (iterator position, size_type n, const value_type & val) { // fill
		// diff is distance from position to the end
		difference_type diff = distance(position, this->end());
		if (this->_size + n > this->_capacity)
			this->reserve(_next_pow_two(this->_size + n));
		// tmp is size before any changes to it
		size_type	tmp = this->_size;
		this->_increase_size(this->_size + n, val);
		// updating position in case of reallocation of array if size increase surpasses capacity
		position = this->begin() + tmp - diff;
		// cur_position is the current position of the newly inserted elements
		iterator	cur_position = this->_array + tmp;
		while (cur_position > position) {
			// moving each new element backwards until we arrive at desired position
			for (size_type i = 0; i < n; i++)
				this->_move_back(distance(this->begin(), cur_position + i));
			cur_position--;
		}
	} 

	template	< class InputIterator >
	// using enable_if here forbids any integral type to get into this function
	// so that only iterators may use this function
	void		insert (iterator position, InputIterator first, InputIterator last, class enable_if<!is_integral<InputIterator>::value, InputIterator>::type* = NULL) { // range
		// diff is distance from position to the end
		difference_type diff = ft::distance(position, this->end());
		// tmp is size before any changes to it
		size_type	tmp = this->_size;
		// n is the number of elements added to the vector
		size_type	n = ft::distance(first, last);
		if (this->_size + n > this->_capacity)
			this->reserve(_next_pow_two(this->_size + n));
		while (first != last) {
			this->_increase_size(this->_size + 1, *first);
			first++;
		}
		// updating position in case of reallocation of array if size increase surpasses capacity
		position = this->begin() + tmp - diff;
		// cur_position is the current position of the newly inserted elements
		iterator cur_position = this->_array + tmp;
		while (cur_position > position) {
			// moving each new element backwards until we arrive at desired position
			for (size_type i = 0; i < n; i++)
				this->_move_back(ft::distance(this->begin(), cur_position + i));
			cur_position--;
		}
	}

	// Removes from the vector either a single elemnt (position) or a range of elements [first, last)
	// This effectively reduces the container size by the number of elements removed, which are destroyed
	// Erasing elements in positions other than the vector end causes the container to relocate all the
	// elements after the segment erased to their new positions
	iterator	erase (iterator position) { // single element
		// this is the distance from the begin to the point of erasure
		difference_type distance = ft::distance(this->begin(), position);
		// destroy element pointed to by position
		this->_alloc.destroy(this->_array + distance);
		// move backwards every element after position
		position++;
		while (position != this->end()) {
			this->_move_back(ft::distance(this->begin(), position));
			position++;
		}
		// reduce size by one
		this->_reduce_size(this->_size - 1);

		return this->begin() + distance;
	}
	iterator	erase (iterator first, iterator last) { // range
		// this is the distance from the begin to the point of erasure
		difference_type distance = ft::distance(this->begin(), first);
		// n is the number of elements erased
		size_type n = ft::distance(first, last);
		// erasing elements pointed to by first one by one
		for (size_type i = 0; i < n; i++)
			erase(this->begin() + distance);

		return this->begin() + distance;
	}

	// Exchanges the content of the vector by the content of x, which is another vector object of the same type
	// Sizes may differ
	void		swap (vector & x) {
		size_type		tmp_capacity = this->_capacity;
		size_type		tmp_size = this->_size;
		allocator_type	tmp_alloc = this->_alloc;
		pointer			tmp_array = this->_array;

		this->_capacity = x._capacity;
		this->_size = x._size;
		this->_alloc = x._alloc;
		this->_array = x._array;

		x._capacity = tmp_capacity;
		x._size = tmp_size;
		x._alloc = tmp_alloc;
		x._array = tmp_array;
	}

	// Removes all elements from the vector (which are destroyed), leaving the container with a size of 0
	void		clear () {
		this->_reduce_size(0);
	}

	/*
	  ___  _ _                 _             
	 / _ \| | |               | |            
	/ /_\ \ | | ___   ___ __ _| |_ ___  _ __ 
	|  _  | | |/ _ \ / __/ _` | __/ _ \| '__|
	| | | | | | (_) | (_| (_| | || (_) | |   
	\_| |_/_|_|\___/ \___\__,_|\__\___/|_|         
	*/

	// Returns a copy of the allocator object associated with the vector
	allocator_type	get_allocator () const {
		return this->_alloc;
	}                 

protected:

	/*
	 _   _           _             _   _            _                     _ 
	| | | |         | |           | | | |          | |                   | |
	| | | |_ __   __| | ___ _ __  | |_| |__   ___  | |__   ___   ___   __| |
	| | | | '_ \ / _` |/ _ \ '__| | __| '_ \ / _ \ | '_ \ / _ \ / _ \ / _` |
	| |_| | | | | (_| |  __/ |    | |_| | | |  __/ | | | | (_) | (_) | (_| |
	\___/|_| |_|\__,_|\___|_|     \__|_| |_|\___| |_| |_|\___/ \___/ \__,_|                            
	*/

	allocator_type	_alloc;
	pointer			_array;
	size_type		_size;
	size_type		_capacity;

	// Effectively reduces the size of the container while retaining its capacity
	// This means that any elements removed from the vector are destroyed
	// but the space is not deallocated
	// n is the effectively the new size
	void	_reduce_size (size_type n) {
		// If n is equal to current container size, does nothing
		if (n == this->_size)
			return ;
		
		for (size_type index = n; index < this->_size; index++) {
			this->_array[index] = value_type();
			this->_alloc.destroy(this->_array + index);
		}
		this->_size = n;
	}
	// Increases the size of the vector while reallocating if new size is over capacity
	// n is the effectively the new size
	void	_increase_size (size_type n, value_type val = value_type()) {
		// If n is equal to current container size, does nothing
		if (n == this->_size)
			return ;
		
		// If n is greater than the current container capacity, an automatic reallocation 
		// of the allocated storage space takes place
		if (n > this->_capacity)
			this->reserve(n);
		
		while (this->_size < n) {
			this->_alloc.construct(this->_array + this->_size, val);
			this->_size++;
		}
	}

	void	_move_back(difference_type position) {
		if (position < 1)
			return ;
		value_type tmp = this->_array[position - 1];
		this->_array[position - 1] = this->_array[position];
		this->_array[position] = tmp;
	}

	size_type	_next_pow_two(size_type n) {
		size_type 	pow = 2;

		while (n > pow)
			pow *= 2;
		return (pow);
	}
};

/*
 _   _                                            _                 
| \ | |                                          | |                
|  \| | ___  _ __ ______ _ __ ___   ___ _ __ ___ | |__   ___ _ __  
| . ` |/ _ \| '_ \______| '_ ` _ \ / _ \ '_ ` _ \| '_ \ / _ \ '__| 
| |\  | (_) | | | |     | | | | | |  __/ | | | | | |_) |  __/ |    
\_| \_/\___/|_| |_|     |_| |_| |_|\___|_| |_| |_|_.__/ \___|_|    
  __                  _   _                                   _                 _     
/ _|                | | (_)                                 | |               | |    
| |_ _   _ _ __   ___| |_ _  ___  _ __     _____   _____ _ __| | ___   __ _  __| |___ 
|  _| | | | '_ \ / __| __| |/ _ \| '_ \   / _ \ \ / / _ \ '__| |/ _ \ / _` |/ _` / __|
| | | |_| | | | | (__| |_| | (_) | | | | | (_) \ V /  __/ |  | | (_) | (_| | (_| \__ \ 	
|_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|  \___/ \_/ \___|_|  |_|\___/ \__,_|\__,_|___/                                                                                                                                 
*/

template	< class T >
bool		operator== (const vector<T> & lhs, const vector<T> & rhs) {
	if (lhs.size() != rhs.size())
		return false;
	return ft::equal(lhs.begin(), lhs.end(), rhs.begin());
}
template	< class T >
bool		operator!= (const vector<T> & lhs, const vector<T> & rhs) {
	return !(lhs == rhs);
}
template	< class T >
bool		operator<  (const vector<T> & lhs, const vector<T> & rhs) {
	return ft::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template	< class T >
bool		operator<= (const vector<T> & lhs, const vector<T> & rhs) {
	return !(rhs < lhs);
}
template	< class T >
bool		operator>  (const vector<T> & lhs, const vector<T> & rhs) {
	return (rhs < lhs);
}
template	< class T >
bool		operator>= (const vector<T> & lhs, const vector<T> & rhs) {
	return !(lhs < rhs);
}

template	< class T >
void		swap (vector<T> & x, vector<T> & y) {
	x.swap(y);
}

}
