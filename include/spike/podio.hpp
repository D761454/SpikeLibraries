/*
----------------------------------------------------------------------

BSD 3-Clause License

Copyright (c) 2026, Cayden Andrews

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#ifndef __SPIKE_PODIO_HPP
#define __SPIKE_PODIO_HPP

#include <cstring>
#include <type_traits>
#include <fstream>

/*
----------------------------------------------------------------------

File Summary:
	This file holds helper methods for writing / reading PODs to file.

----------------------------------------------------------------------
*/

/*

to satisfy POD rules, a class or struct must satisfy the following:
STD Layout:
	-	no mixed access specifiers
	-	no distributed members across inheritance levels
	-	no reference data mambers
	-	no virtual functions
	-	first member var should not be base class type or derived from it
Trivially Copyable:
	-	no custom destructor
	-	no custom copy or move
	-	non TC data members (STL types or non TC struct / class)

*/

namespace podio {
	template <typename T>
	constexpr bool is_pod_writable_v =
		std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;

	/// <summary>
	/// Writes a POD class or struct to file in one command
	/// </summary>
	/// <typeparam name="T">Type of object to write from</typeparam>
	/// <param name="os">File Stream to Write to</param>
	/// <param name="obj">Object to write</param>
	/// <returns>if the write / os is good</returns>
	template <typename T>
	inline bool write_pod(std::ostream& os, T const& obj) {
		static_assert(is_pod_writable_v<T>, "write_pod requires trivially copyable, standard-layout type");
		os.write(reinterpret_cast<char const*>(&obj), sizeof(T));
		return os.good();
	}

	/// <summary>
	/// Reads in a POD class or struct from file in one command
	/// </summary>
	/// <typeparam name="T">Type of object to read into</typeparam>
	/// <param name="is">File Stream to Read from</param>
	/// <param name="out">Object to read data into</param>
	/// <returns>if the read was good or it is end of file</returns>
	template <typename T>
	inline bool read_pod(std::istream& is, T& out) {
		static_assert(is_pod_writable_v<T>, "read_pod requires trivially copyable, standard-layout type");
		is.read(reinterpret_cast<char*>(&out), sizeof(T));
		return is.good();
	}

	/// <summary>
	/// Zero Initializes a PODs memory to 0
	/// </summary>
	/// <typeparam name="T">Type of object to zero initialize</typeparam>
	/// <param name="obj">Object to Zero Initialize</param>
	template <typename T>
	inline void zero_init(T& obj) noexcept {
		static_assert(is_pod_writable_v<T>, "zero_init targets POD-like types");
		std::memset(&obj, 0, sizeof(T));
	}
}

#endif