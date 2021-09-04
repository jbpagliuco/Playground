#pragma once


#include "Core/Core.h"
#include "Renderer/Mesh.h"

#define MESH_ASSET_VERSION 2
#define MESH_ASSET_MIN_VERSION 2
#define MESH_ASSET_MAX_VERSION 2

namespace playground
{
	bool MeshSystemInit();
	void MeshSystemShutdown();

#if CORE_BUILD_TYPE(TOOLS)
	bool ConvertMeshObjToMeshx(const std::string &objFilename, bool calculateTangentSpaceBasicVectors);
#endif
}