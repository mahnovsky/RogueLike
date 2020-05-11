#pragma once

#include "type_info.hpp"

#define NS_ENTITY_TYPE 1

enum EntityTypeIndex
{
	ETI_Entity,
	ETI_Actor
};

REGISTRY_TYPE(Entity, ETI_Entity, NS_ENTITY_TYPE);
REGISTRY_TYPE(Actor, ETI_Actor, NS_ENTITY_TYPE);

#define NS_COMPONENT_TYPE 2

enum ComponentTypeIndex
{
	CTI_Transform,
	CTI_Render,
	CTI_Move,
	CTI_OctreeObject,
	CTI_Widget,
	CTI_Text
};

REGISTRY_TYPE(RenderComponent, CTI_Render, NS_COMPONENT_TYPE);
REGISTRY_TYPE(Transform, CTI_Transform, NS_COMPONENT_TYPE);
REGISTRY_TYPE(MoveComponent, CTI_Move, NS_COMPONENT_TYPE);
REGISTRY_TYPE(OctreeObject, CTI_OctreeObject, NS_COMPONENT_TYPE);
REGISTRY_TYPE(Widget, CTI_Widget, NS_COMPONENT_TYPE);
REGISTRY_TYPE(TextComponent, CTI_Text, NS_COMPONENT_TYPE);

#define NS_SYSTEM_TYPE 3

enum SystemTypeIndex
{
	STI_Render,
	STI_Move,
	STI_Octree,
	STI_ResourceStorage
};

REGISTRY_TYPE(RenderSystem, STI_Render, NS_SYSTEM_TYPE);
REGISTRY_TYPE(MoveSystem, STI_Move, NS_SYSTEM_TYPE);
REGISTRY_TYPE(Octree, STI_Octree, NS_SYSTEM_TYPE);
REGISTRY_TYPE(ResourceStorage, STI_ResourceStorage, NS_SYSTEM_TYPE);

#define NS_RESOURCE_TYPE 4

enum ResourceTypeIndex
{
	RTI_Config,
	RTI_Texture,
	RTI_Font,
	RTI_StaticMesh,
	RTI_ShaderProgram,
	RTI_BaseShader
};

REGISTRY_TYPE(Config, RTI_Config, NS_RESOURCE_TYPE);
REGISTRY_TYPE(Texture, RTI_Texture, NS_RESOURCE_TYPE);
REGISTRY_TYPE(Font, RTI_Font, NS_RESOURCE_TYPE);
REGISTRY_TYPE(StaticMesh, RTI_StaticMesh, NS_RESOURCE_TYPE);
REGISTRY_TYPE(ShaderProgram, RTI_ShaderProgram, NS_RESOURCE_TYPE);
REGISTRY_TYPE(BaseShader, RTI_BaseShader, NS_RESOURCE_TYPE);