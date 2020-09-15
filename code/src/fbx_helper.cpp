#include "fbx_helper.hpp"

#include "OpenFBX/ofbx.h"

bool load_fbx_mesh(ofbx::IScene* scene, int index, MeshData& mesh_data)
{
    if (scene->getMeshCount() > index)
    {
        const ofbx::Mesh* mesh = scene->getMesh(index);
		if (!mesh)
		{
			return false;
		}

        const ofbx::Geometry* geometry = mesh->getGeometry();
		if (!geometry)
		{
			return false;
		}

        const ofbx::Vec3* vertices = geometry->getVertices();
		if (!vertices)
		{
			return false;
		}

		const ofbx::Vec3* normals = geometry->getNormals();
		const ofbx::Vec2* uvs0 = geometry->getUVs();
		VertexBuffer buffer;
        for (int i = 0; i < geometry->getVertexCount(); ++i)
        {
            ofbx::Vec3 vertex(*(vertices + i));

            Vertex v;
            v.pos = { vertex.x, vertex.y, vertex.z };

			if (uvs0)
			{
				ofbx::Vec2 uv = *(uvs0 + index);
				v.uv = { uv.x, uv.y };
			}

			if (normals)
			{
				ofbx::Vec3 normal = *(normals + index);
				v.normal = { normal.x, normal.y, normal.z };
			}

			buffer.push_back(v);
        }

		setup_vertices(mesh_data.vertex_data, buffer);

        const int* indices = geometry->getFaceIndices();
        for (int i = 0; i < geometry->getIndexCount(); ++i)
        {
            int index = *(indices + i);
            if (index < 0) index = ~index;
            mesh_data.indices.push_back(index);
        }

        return true;
    }

    return false;
}