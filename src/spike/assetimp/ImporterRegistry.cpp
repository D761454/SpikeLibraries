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

#include "spike/assetimp/ImporterRegistry.h"

#include <vector>

#include "spike/assetimp/BaseImporter.h"

// Include Importers [...]
#if defined(_USING_FBX)
#include "spike/assetimp/FbxImporter.h"
#endif
#include "spike/assetimp/GltfImporter.h"
#include "spike/assetimp/ObjImporter.h"
#include "spike/assetimp/BTexImporter.h"
#include "spike/assetimp/ShaderImporter.h"

void assimp::get_importer_instances(std::vector<BaseImporter*>& out) {
	out.reserve(5);
	out.push_back(new ObjImporter());
	out.push_back(new GltfImporter());
#if defined(_USING_FBX)
	out.push_back(new FbxImporter());
#endif
	out.push_back(new BTexImporter());
	out.push_back(new ShaderImporter());
}

void assimp::delete_importer_instances(std::vector<BaseImporter*>& del) {
	for (size_t i = 0; i < del.size(); ++i) {
		delete del[i];
		del[i] = nullptr;
	}
	del.clear();
}