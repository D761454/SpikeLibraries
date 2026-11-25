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