#ifndef __SPIKE_STL_ALIASES_H
#define __SPIKE_STL_ALIASES_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstring>
#include <string_view>

/*
----------------------------------------------------------------------

File Summary:
	This file holds default aliases for STL data types for use in
	projects.

----------------------------------------------------------------------
*/

/*
STL Types also include:
	-	map
	-	set
	-	list
	-	vector
*/

using string = std::string;
using string_view = std::string_view;

template<typename T>
using uset = std::unordered_set<T>;

template<typename T, typename Y, typename U = std::hash<T>>
using umap = std::unordered_map<T, Y, U>;

#endif