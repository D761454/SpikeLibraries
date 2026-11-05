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

#ifndef __SPIKE_NIVECTOR_H
#define __SPIKE_NIVECTOR_H

#include <vector>

/*
----------------------------------------------------------------------

File Summary:
	This file holds the nivector container type. The nivector<T> is
	equivalent to a std::vector, differing in its resize method.
	Resizing does not default allocate its extra memory.

----------------------------------------------------------------------
*/

namespace {
	template <typename T>
	struct NoInitAllocator : std::allocator<T> {
		template <typename U>
		struct rebind { using other = NoInitAllocator<U>; };

		template <typename U, typename... Args>
		void construct(U* p, Args&&...) {
			// Leave uninitialized for default construction
		}
	};
}

/// <summary>
/// A vector where resizing does not initialize memory, allowing for speedy resizing with a subsequent memcpy from a position within a buffer
/// </summary>
/// <typeparam name="T">Type of object held by the nivector</typeparam>
template<typename T>
struct niVector : public std::vector<T, NoInitAllocator<T>> {
	using Base = std::vector<T, NoInitAllocator<T>>;
	using Base::Base; // inherit ctors

	niVector() noexcept(std::is_nothrow_default_constructible_v<Base>) = default;
	niVector(const niVector&) = default;
	niVector(niVector&&) noexcept = default;
	niVector& operator=(const niVector&) = default;
	niVector& operator=(niVector&&) noexcept = default;
	~niVector() = default;

	// construct/assign from any std::vector<T, OtherAlloc>
	// optimized: use memcpy when T is trivially copyable
	template <class OtherAlloc>
	niVector(const std::vector<T, OtherAlloc>& other) {
		if (other.empty()) { this->clear(); return; }
		if constexpr (std::is_trivially_copyable_v<T>) {
			this->resize(other.size());
			std::memcpy(this->data(), other.data(), other.size() * sizeof(T));
		}
		else {
			this->reserve(other.size());
			this->insert(this->end(), other.begin(), other.end());
		}
	}

	template <class OtherAlloc>
	niVector& operator=(const std::vector<T, OtherAlloc>& other) {
		if (other.empty()) { this->clear(); return *this; }
		if constexpr (std::is_trivially_copyable_v<T>) {
			this->resize(other.size());
			std::memcpy(this->data(), other.data(), other.size() * sizeof(T));
		}
		else {
			this->clear();
			this->reserve(other.size());
			this->insert(this->end(), other.begin(), other.end());
		}
		return *this;
	}

	// convert to any std::vector<T, OtherAlloc>
	template <class OtherAlloc>
	explicit operator std::vector<T, OtherAlloc>() const {
		return std::vector<T, OtherAlloc>(this->begin(), this->end());
	}
};

#endif