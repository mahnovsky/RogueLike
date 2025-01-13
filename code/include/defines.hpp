#pragma once

class Config;
class TimerManager;
class Texture;
class FileResource;
class Text;
class Sprite;
class SharedObject;
class ObjectManager;

namespace core {
	class ResourceStorage;
	class WidgetSystem;
	class Engine;
}
class IRender;
class ICamera;
class IWindow;
class Transform;
class Material;
class Widget;
class WidgetList;
class WidgetText;
class WidgetButton;
class EcsManager;
class World;
class Entity;
class StaticMesh;
class IRenderObject;

class TransformComponent;

namespace input
{
class Input;
}

struct RenderNode;

namespace se
{
class Font;
}

#include "stdext.hpp"

#include "common.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
