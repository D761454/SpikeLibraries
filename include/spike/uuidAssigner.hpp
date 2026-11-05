/*
uuidAssigner.hpp - spike libraries (uuids / generate_id)
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

#ifndef __UUID_ASSIGNER_HPP
#define __UUID_ASSIGNER_HPP

#include "engine_aliases.h"

/*
----------------------------------------------------------------------

File Summary:
	This file holds the helper function for assigning uuids, ensuring they
	are not null (0) or invalid (255)

	NOTE: stduuid is not endorsed by or associated in any way with
	SpikeTooth or Cayden Andrews.

----------------------------------------------------------------------
*/

namespace uuids {
	/// <summary>
	/// generates a non null, non invalid uuid
	/// </summary>
	/// <returns>valid uuid</returns>
	inline uuid generate_id() noexcept {
		uuid id;
		do {
			id = uuids::uuid_system_generator{}();
		} while (id == kInvalidUuid || id.is_nil());

		return id;
	}
}

#endif