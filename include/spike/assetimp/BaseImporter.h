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

#ifndef __ASSET_IMP_BASE_IMPORTER_HPP
#define __ASSET_IMP_BASE_IMPORTER_HPP

#include <set>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

namespace errlib {
	struct Error;
}

namespace assimp {
	struct Collection;

	/// <summary>
	/// Base class to be used by all specific importers, containing the virtual methods invoked by the 
	/// primary importer on file parsing
	/// </summary>
	class BaseImporter {
	public:
		BaseImporter() noexcept;
		virtual ~BaseImporter() = default;

		inline void SetFlags(u32 flags) { ppFlags = flags; }

		/// <summary>
		/// checks the file extension against the extensions list within the importer, determining if the importer is designed to parse the file
		/// </summary>
		/// <param name="file">file to check against</param>
		/// <returns>if the file can be parsed by this importer</returns>
		bool CanParse(const string& file) const;

		/// <summary>
		/// Called to import assets from a file, returnign the result as an asset collection
		/// </summary>
		/// <param name="file">path to file to read</param>
		/// <returns>asset collection holding all imported assets</returns>
		Collection* ReadFile(const string& file);

		/// <summary>
		/// checks if this importer can parse a file with the provided extension
		/// </summary>
		/// <param name="extn"></param>
		/// <returns></returns>
		inline bool SupportsExtension(const string& extn) const {
			return extensions.contains(extn);
		}

	protected:
		virtual errlib::Error InternReadFile(const string& file, Collection* scene) = 0;

		std::set<string> extensions;

		u32 ppFlags;
	};
}

#endif