#pragma once

#include "generic/type_info.hpp"

#define NS_ENTITY_TYPE 1

enum EntityTypeIndex
{
	ETI_Entity,
	ETI_Actor,
	ETI_Widget
};

REGISTRY_TYPE(Entity, ETI_Entity, NS_ENTITY_TYPE);
REGISTRY_TYPE(Actor, ETI_Actor, NS_ENTITY_TYPE);
REGISTRY_TYPE(Widget, ETI_Widget, NS_ENTITY_TYPE);


#define NS_COMPONENT_TYPE 2

enum ComponentTypeIndex
{
	CTI_Transform,
	CTI_Render,
	CTI_Move,
	CTI_OctreeObject
};


REGISTRY_TYPE(RenderComponent, CTI_Render, NS_COMPONENT_TYPE);
REGISTRY_TYPE(Transform, CTI_Transform, NS_COMPONENT_TYPE);
REGISTRY_TYPE(MoveComponent, CTI_Move, NS_COMPONENT_TYPE);
REGISTRY_TYPE(OctreeObject, CTI_OctreeObject, NS_COMPONENT_TYPE);

#define NS_SYSTEM_TYPE 3

enum SystemTypeIndex
{
	STI_Render,
	STI_Move,
	STI_Octree
};

REGISTRY_TYPE(RenderSystem, STI_Render, NS_SYSTEM_TYPE);
REGISTRY_TYPE(MoveSystem, STI_Move, NS_SYSTEM_TYPE);
REGISTRY_TYPE(Octree, STI_Octree, NS_SYSTEM_TYPE);