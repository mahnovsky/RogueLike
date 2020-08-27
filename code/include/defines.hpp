#pragma once

class Config;
class TimerManager;
class Texture;
class FileResource;
class Text;
class Sprite;
class SharedObject;
class ObjectManager;
class Engine;
class ResourceStorage;
class IRender;
class ICamera;
class IWindow;
class Transform;
class Material;
class Widget;
class RootWidget;
class WidgetList;
class WidgetText;
class EcsManager;
class World;
class Entity;
class StaticMesh;

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

#include <cstdint>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

using std::int8_t;
using std::int16_t;
using std::int32_t;

#include "basic/common.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
