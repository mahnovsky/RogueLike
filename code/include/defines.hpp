#pragma once

class TimerManager;
class Texture;
class ShaderBase;
class ShaderProgram;
class RenderObject;
class FileResource;
class Text;
class Sprite;
class Cpnfig;
class World;
class Object;
class ObjectManager;
class Actor;
class Engine;
class ResourceStorage;
class IRender;
class ICamera;
class IWindow;
class Transform;
class Material;
class Widget;

struct RenderNode;

namespace se {
class Font;
}

#include "basic/common.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
