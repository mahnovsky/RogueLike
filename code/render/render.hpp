#pragma once

#include "defines.hpp"
#include "render_common.hpp"


class ICamera;
class RenderComponent;

struct RenderObjectData;

struct RenderObjectDataDelete { // default deleter for unique_ptr
	constexpr RenderObjectDataDelete() noexcept = default;

	RenderObjectDataDelete(const RenderObjectDataDelete&) noexcept {}

	void operator()(RenderObjectData* _Ptr) const noexcept;
};

using RenderObjectDataPtr = std::unique_ptr<RenderObjectData, RenderObjectDataDelete>;

enum class DataPresentMode
{
	Point,
	Line,
	Triangle,
	TriangleFan,
	TriangleStrip
};

RenderObjectDataPtr create_render_data(DataPresentMode present_mode);

class IRenderObject
{
public:
	virtual ~IRenderObject() = default;

	virtual void on_resource_changed(RenderResourceType type, const basic::String& name) = 0;

	virtual void on_component_changed(RenderComponent& comp) = 0;

	virtual void update_mvp(const glm::mat4& mvp) = 0;

	virtual void update_color(basic::Color color) = 0;
};

class IRender
{
public:
    virtual ~IRender( ) = default;

    virtual bool init( ResourceStorage* rs, int width, int height ) = 0;

    virtual void clear( ) = 0;

    virtual void present() = 0;

	virtual void add_to_frame(IRenderObject* object) = 0;

	virtual void add_to_frame(const RenderObjectData& data) = 0;

	virtual IRenderObject* create_object(RenderComponent& comp) = 0;

	virtual void delete_object(IRenderObject* obj) = 0;

    static IRender* create( );
};
