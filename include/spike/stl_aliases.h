/*
stl_aliases.h - spike libraries
----------------------------------------------------------------------

Copyright (c) 2026-2026, SpikeTooth, Cayden Andrews

All rights reserved.

* Re-distributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Re-distributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of SpikeTooth, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the SpikeTooth team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#ifndef __STL_ALIASES_H
#define __STL_ALIASES_H

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