#ifndef __SPIKE_UUID_ASSIGNER_HPP
#define __SPIKE_UUID_ASSIGNER_HPP

#ifndef UUID_SYSTEM_GENERATOR
#define UUID_SYSTEM_GENERATOR
#endif
#include "external/stduuid/uuid.h"

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
	[[nodiscard]] inline uuid generate_id() noexcept {
		uuid id;
		do {
			id = uuid_system_generator{}();
		} while (id == uuid_namespace_max || id.is_nil());

		return id;
	}
}

#endif