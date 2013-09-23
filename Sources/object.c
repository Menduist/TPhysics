
#include "world.h"
#include "object.h"
#include "collider.h"
#include "quadtree.h"
#include <stdlib.h>

t_object *TObjectCreateStatic(void *userdata, char layer)
{
	t_object  *object;

	object = (t_object*)calloc(1, sizeof(t_staticobject));
	vector_init(&object->colliders, 1);
	object->objecttype = 0;
	object->inversemass = 0;
	object->friction = 0.3;
	object->userdata = userdata;
	object->layer = layer;
	return (object);
}

t_object *TObjectCreateDynamic(void *userdata, char layer)
{
	t_object  *object;

	object = (t_object*)calloc(1, sizeof(t_dobject));
	object->dynamic.current_triggers = vector_create_sized(1);
	object->objecttype = 1;
	object->inversemass = 1 / 1;
	object->friction = 0.3;
	object->userdata = userdata;
	object->layer = layer;
	return (object);
}

t_object *TObjectCreateSubworld(void *userdata, char layer, int subworldsize)
{
	t_object *object;

	object = (t_object *)calloc(1, sizeof(t_subworldobject));
	object->dynamic.current_triggers = vector_create_sized(1);
	object->objecttype = 2;
	object->inversemass =  1 / 1;
	object->friction = 0.3;
	object->userdata = userdata;
	object->layer = layer;
	object->subworld.subworld.broadtree = BuildQuadtree(subworldsize, 3);
	object->subworld.subworld.object_list = newSmartArray(MAXOBJECT);
	return (object);
}

void		TSetObjectPosition(t_object *obj, double x, double y, double z)
{
	obj->transform.position.x = x;
	obj->transform.position.y = y;
	obj->transform.position.z = z;
}

void        UpdateObject(t_object *object, float deltatime)
{
	object->dynamic.groundedangle = 0;
	object->velocity = add_vector3_to_vector3_mult(object->dynamic.force,
					object->velocity, deltatime/* * object->inversemass*/);
	object->dynamic.force = (t_vector3){0, 0, 0};
	object->dynamic.groundnormal = (t_vector3){0, 0, 0};
    object->transform.position = add_vector3_to_vector3_mult(object->velocity, object->transform.position, deltatime);

	int i;
	for (i = 0; i < object->colliders.current; i++)
	{
		t_collider *coll = object->colliders.data[i];
		coll->position = add_vector3_to_vector3(object->transform.position, coll->offset);
	}
}

void            MoveObjectToSubworld(t_object *subworld, t_object *object)
{
	if (object->world == subworld->world)
	{
		t_vector3 tempvel;
		RemoveObjectFromWorld(object);
		AddObjectToWorld(&subworld->subworld.subworld, object);

		tempvel = add_vector3_to_vector3(object->transform.position, sub_vector3_from_vector3(object->velocity, subworld->velocity));
		object->transform.position = transform_position(object->transform.position, subworld->transform);
		tempvel = transform_position(tempvel, subworld->transform);
		object->velocity = sub_vector3_from_vector3(tempvel, object->transform.position);
	}
}

void            MoveObjectToWorld(t_object *subworld, t_object *object)
{
	if (object->world == &subworld->subworld.subworld)
	{
		t_vector3 tempvel;
		RemoveObjectFromWorld(object);
		AddObjectToWorld(subworld->world, object);

		tempvel = add_vector3_to_vector3(object->transform.position, add_vector3_to_vector3(object->velocity, subworld->velocity));
		object->transform.position = transform_position_inv(object->transform.position, subworld->transform);
		tempvel = transform_position_inv(tempvel, subworld->transform);
		object->velocity = sub_vector3_from_vector3(tempvel, object->transform.position);
	}
}
