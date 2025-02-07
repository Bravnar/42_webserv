#ifndef HASHTABLE_HPP
# define HASHTABLE_HPP

# include <iostream>
# include <cstdlib>
# include <stdio.h>

template <typename KeyType, typename ValueType>
class HashTable {
	private:
		typedef struct s_node {
			KeyType 		key ;
			ValueType 		value ;
			struct s_node*	next ;
			s_node( const KeyType& k, const ValueType& v)
				: key(k), value(v), next(NULL) { }
		} 	t_node ;

		t_node**	_lists ;
		size_t		_total ;
		size_t		_size ;

		size_t	hash( const KeyType& key ) const ;
		void	rehash() ;

	public:
} ;

#endif 