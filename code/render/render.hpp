#pragma once

#include "defines.hpp"
#include "render_common.hpp"
#include "resource_storage.hpp"

class ICamera;

class IRenderObject
{
public:
	virtual ~IRenderObject() = default;

	virtual void set_resource(RenderResourceType type, const std::string& resource_name) = 0;

	virtual void update_mvp(const glm::mat4& mvp) = 0;

	virtual void update_color(basic::Color color) = 0;

	virtual MeshData& get_mesh_data() = 0;

	virtual void update_mesh_data() = 0;

	virtual void set_vertex_draw_mode(::VertexDrawMode mode) = 0;

	virtual void set_vertex_buffer_usage(VertexBufferUsage usage) = 0;

	virtual void set_render_state(uint32_t flag) = 0;

	virtual void set_camera_index(uint32_t index) = 0;
};

class IRender
{
public:
    virtual ~IRender( ) = default;

    virtual bool init( core::ResourceStorage* rs, int width, int height ) = 0;

    virtual void clear( ) = 0;

    virtual void present() = 0;

	virtual void add_to_frame(IRenderObject* object) = 0;

	virtual int32_t add_camera(ICamera* camera) = 0;

	virtual IRenderObject* create_object() = 0;

	virtual void delete_object(IRenderObject* obj) = 0;

    static IRender* create( );
};
