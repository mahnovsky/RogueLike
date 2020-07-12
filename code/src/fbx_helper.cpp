#include "fbx_helper.hpp"

#include "OpenFBX/ofbx.h"

bool load_fbx_mesh(ofbx::IScene* scene, int index, MeshData& mesh_data)
{
    if (scene->getMeshCount() > index)
    {
        const ofbx::Mesh* mesh = scene->getMesh(index);

        const ofbx::Geometry* geometry = mesh->getGeometry();

        const ofbx::Vec3* vertices = geometry->getVertices();

        for (int i = 0; i < geometry->getVertexCount(); ++i)
        {
            ofbx::Vec3 vertex(*(vertices + i));

            Vertex v;
            v.pos = { vertex.x, vertex.y, vertex.z };
            mesh_data.vb.push_back(v);
        }

        const int* indices = geometry->getFaceIndices();
        for (int i = 0; i < geometry->getIndexCount(); ++i)
        {
            int index = *(indices + i);
            if (index < 0) index = -1 * index;
            mesh_data.ib.push_back(index);
        }

        return true;
    }

    return false;
}