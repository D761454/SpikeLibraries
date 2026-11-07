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

#include <set>
#include <vector>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

namespace errlib {
	struct Error;
}

namespace assimp {
	/* common interface for all importers */
	class BaseImporter;

	/*return type for an imported asset file*/
	struct Collection;

	/*
	interface for importing assets
	Note: Imported assets are not parsed for the engine specifically at current,
	merely returning an assimp scene / mesh / textures to then be parsed into the engines current mesh implementation
	*/
	class Importer {
	private:

	public:
		Importer();
		Importer(const Importer& other) = delete;
		Importer& operator=(const Importer&) = delete;

		~Importer();

		errlib::Error RegisterLoader(BaseImporter* l);
		errlib::Error UnregisterLoader(BaseImporter* l);

		inline void SetFlags(u32 flags) { pp = flags; }

		const Collection* ReadFile(const string& path, u32 flags = 0);

		const Collection* GetScene();
		void FreeScene();

		bool IsExtensionSupported(const string& extn) const;

		inline const usize GetImporterCount() const { return importers.size(); }

		BaseImporter* GetImporter(usize idx) const { return importers.at(idx); }
		BaseImporter* GetImporter(const string& extn) const;

		const usize GetImporterIndex(const string& extn) const;

	protected:
		std::vector<BaseImporter*> importers;
		Collection* scene;
		u32 pp = 0;
	};
}

#endif