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

#ifndef __ASSET_IMP_IMPORTER_HPP
#define __ASSET_IMP_IMPORTER_HPP

/*
----------------------------------------------------------------------

File Summary:
	Primary API to interface with assetimp, allowing a user to pass a 
	file to import, to then receive a collection of assets imported 
	from said file.

----------------------------------------------------------------------
*/

#include <vector>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/error.hpp"

namespace assimp {
	class BaseImporter;

	/*return type for an imported asset file*/
	struct Collection;

	/// <summary>
	/// Importer object used for importing assets, claiming ownership of an imported scene
	/// </summary>
	class Importer {
	private:

	public:
		Importer();
		Importer(const Importer& other) = delete;
		Importer& operator=(const Importer&) = delete;

		~Importer();

		inline void SetFlags(u32 flags) { pp = flags; }

		/// <summary>Reads an asset file using the currently configured flags.</summary>
		const Collection* ReadFile(const string& path);

		/// <summary>Reads an asset file using the provided post-processing flags.</summary>
		const Collection* ReadFile(const string& path, u32 flags);

		/// <summary>Returns the error from the most recent read attempt.</summary>
		const errlib::Error& GetLastError() const noexcept { return lastError; }

		/// <summary>Retrieves the last imported asset collection.</summary>
		const Collection* GetCollection();
		const Collection* GetCollection() const noexcept;

		/// <summary>
		/// deletes the last imported asset collection
		/// </summary>
		void FreeCollection();

		/// <summary>
		/// Used to check if a file with the provided extension can be parsed by any of the available importers
		/// </summary>
		/// <param name="extn">extension to check</param>
		/// <returns>if an importer is registered that can import files with the extension</returns>
		bool IsExtensionSupported(const string& extn) const;

		inline const usize GetImporterCount() const { return importers.size(); }

	protected:
		std::vector<BaseImporter*> importers;
		Collection* scene;
		errlib::Error lastError;
		u32 pp = 0;
	};
}

#endif