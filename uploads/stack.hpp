#pragma once

namespace ft {

template < class T, class Container = vector<T> >
class stack {
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
	typedef Container			container_type;
	typedef std::size_t			size_type;

	/*
	___  ___               _                  __                  _   _                 
	|  \/  |              | |                / _|                | | (_)                
	| .  . | ___ _ __ ___ | |__   ___ _ __  | |_ _   _ _ __   ___| |_ _  ___  _ __  ___ 
	| |\/| |/ _ \ '_ ` _ \| '_ \ / _ \ '__| |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
	| |  | |  __/ | | | | | |_) |  __/ |    | | | |_| | | | | (__| |_| | (_) | | | \__ \
	\_|  |_/\___|_| |_| |_|_.__/ \___|_|    |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
	*/

	// Constructs a stack with a copy of the underlying container
	explicit	stack (const container_type & ctnr = container_type()) {
		c = ctnr;
	}

	// Returns whether the stack is empty: i.e. whether its size is zero
	bool		empty () const {
		return c.empty();
	}

	// Returns the number of elements in the stack
	size_type	size () const {
		return c.size();
	}

	// Returns a reference to the top element in the stack
	value_type &		top () {
		return c.back();
	}
	const value_type &	top () const {
		return c.back();
	}

	// Inserts a new element at the top of the stack, above its current top element
	// The content of this new element is initialized to a copy of val
	void	push (const value_type & val) {
		c.push_back(val);
	}

	// Removes the element on top of the stack, effectively reducing its size by one
	void	pop () {
		c.pop_back();
	}

// friend bool	operator== (const stack<T, Container> & lhs, const stack<T, Container> & rhs);
// friend bool	operator!= (const stack & lhs, const stack & rhs);
// friend bool	operator< (const stack & lhs, const stack & rhs);
// friend bool	operator<= (const stack & lhs, const stack & rhs);
// friend bool	operator> (const stack & lhs, const stack & rhs);
// friend bool	operator>= (const stack & lhs, const stack & rhs);

protected:
	container_type	c;
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

template < class T, class Container >
bool	operator== (const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
	return (lhs.c == rhs.c);
}
template < class T, class Container >
bool	operator!= (const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
	return (lhs.c != rhs.c);
}
template < class T, class Container >
bool	operator<  (const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
	return (lhs.c < rhs.c);
}
template < class T, class Container >
bool	operator<= (const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
	return (lhs.c <= rhs.c);
}
template < class T, class Container >
bool	operator>  (const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
	return (lhs.c > rhs.c);
}
template < class T, class Container >
bool	operator>= (const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
	return (lhs.c >= rhs.c);
}

}
