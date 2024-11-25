#pragma once

#include "type_info.hpp"

enum EntityType
{
	ETI_Entity,
	ETI_Actor
};

REGISTRY_TYPE(Entity, ETI_Entity, EntityType);
REGISTRY_TYPE(Actor, ETI_Actor, EntityType);

enum ComponentType
{
	CTI_Transform,
	CTI_Render,
	CTI_Move,
	CTI_OctreeObject,
	CTI_Widget,
	CTI_Text
};

REGISTRY_TYPE(RenderComponent, CTI_Render, ComponentType);
REGISTRY_TYPE(Transform, CTI_Transform, ComponentType);
REGISTRY_TYPE(MoveComponent, CTI_Move, ComponentType);
REGISTRY_TYPE(OctreeObject, CTI_OctreeObject, ComponentType);
REGISTRY_TYPE(Widget, CTI_Widget, ComponentType);
REGISTRY_TYPE(TextComponent, CTI_Text, ComponentType);

enum SystemType
{
	STI_Render,
	STI_Move,
	STI_Octree,
	STI_ResourceStorage
};

REGISTRY_TYPE(RenderSystem, STI_Render, SystemType);
REGISTRY_TYPE(MoveSystem, STI_Move, SystemType);
REGISTRY_TYPE(Octree, STI_Octree, SystemType);
REGISTRY_TYPE(ResourceStorage, STI_ResourceStorage, SystemType);
