#pragma once

#include "render_common.hpp"

namespace ofbx
{
	class IScene;
}

bool load_fbx_mesh(ofbx::IScene* scene, int index, MeshData& mesh_data);