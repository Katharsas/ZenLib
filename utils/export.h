#pragma once
#include <zenload/zTypes.h>

namespace Utils
{
	/**
	 * Should be built with largeAddressAware or x64 when exporting very large meshes.
	 * VS Debug: Use "_NO_DEBUG_HEAP=1" in Properties -> Debugging -> Environment to reproduce 
	 * bad_alloc exception caused by not enough memory.
	 *
	 * Exports the given packed-mesh structure into a wavefront-OBJ-file
	 * @param mesh Mesh to export
	 * @param file Target OBJ-File
	 * @param number of fractional digits of numbers in output, last digit is rounded
	 * @param name Name of the object to export
	 * @return Whether the export succeeded
	 */
	bool exportPackedMeshToObj(ZenLoad::PackedMesh& mesh, const std::string& file,
		const int decimals = 6, const std::string& name = "Object");
}
