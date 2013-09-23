#include "world.h"
#include "object.h"
#include <math.h>
#include <stdlib.h>
#include "quadtree.h"
#include "collider.h"
#include "collisionlayer.h"

void        UpdateSubworld(t_object *object, float deltatime);

t_world         *TWorldCreate(float size)
{
    t_world     *world;

    world = (t_world *)calloc(1, sizeof(t_world));
	world->object_list = newSmartArray(MAXOBJECT);
    world->broadtree = BuildQuadtree(size, 3);
    return (world);
}

void            TestsColliders(t_collider *obja, t_collider *objb, float delta)
{
    char collisiontype = layers[obja->ref->layer][objb->ref->layer];
    if (collisiontype && obja->ref->world == objb->ref->world && obja->ref->world)
    {
        if (fabs(obja->position.x - objb->position.x) > (obja->aabbsize.x + objb->aabbsize.x)) return;
        if (fabs(obja->position.y - objb->position.y) > (obja->aabbsize.y + objb->aabbsize.y)) return;
        if (fabs(obja->position.z - objb->position.z) > (obja->aabbsize.z + objb->aabbsize.z)) return;

        t_response response;
        int 		collision;

        if (collisiontype & BLOK)
            collision = test_collider_collider(obja, objb, &response);
        else
            collision = test_collider_collider(obja, objb, 0);

    	if (collision && collisiontype & TRIG)
    	{
    		t_object *nonstatic = obja->ref->objecttype ? obja->ref : objb->ref;
    		t_object *other = nonstatic == obja->ref ? objb->ref : obja->ref;

    		if (!vector_contains(nonstatic->dynamic.current_triggers, other))
    		{
    			OnObjectEnteredTrigger(nonstatic, other);
    			vector_append(nonstatic->dynamic.current_triggers, other);
    			if (other->objecttype != 0)
    			{
    				vector_append(other->dynamic.current_triggers, nonstatic);
    			}
    		}
    	}
    	if (collision)
    	{
    		if (collisiontype & BLOK)
    			response_collider(&response);
    		OnCollision(obja->ref, objb->ref, delta);
    	}
    }
}

int             ObjectsIntersecting(t_object *a, t_object *b)
{
    int i, y;

    for (i = 0; i < a->colliders.current; i++)
    {
        for (y = 0; y < b->colliders.current; y++)
        {
            if (test_collider_collider(a->colliders.data[i], b->colliders.data[y], 0))
                return true;
        }
    }
    return false;
}

int             ObjectInterectingCollider(t_object *a, t_collider *b)
{
    int i;

    for (i = 0; i < a->colliders.current; i++)
    {
        if (test_collider_collider(a->colliders.data[i], b, 0))
            return true;
    }
    return false;
}

void            UpdateWorld(t_world *world, float deltatime)
{
	int i, y, z;
    t_object *cur;

    ClearAllDynsOfNode(world->broadtree);
	for (i = 0; i < world->object_list->last_free_slot; i++) {
		if (world->object_list->object_array[i] != NULL) {
            cur = world->object_list->object_array[i];
			UpdateObject(cur, deltatime);

            if (cur->objecttype == 2)
                UpdateSubworld(cur, deltatime);

            for (z = 0; z < cur->colliders.current; z++)
                TestColliderWithNodeInsert(world->broadtree,
                                cur->colliders.data[z], deltatime);
		}
	}

	for (i = 0; i < world->object_list->last_free_slot; i++) {
		if (world->object_list->object_array[i] != NULL) {
            t_object *obj = world->object_list->object_array[i];

            if (obj->objecttype == 2)
                continue;

            for (y = 0; y < obj->dynamic.current_triggers->current; y++)
            {
				t_object *test = obj->dynamic.current_triggers->data[y];
				if (test->objecttype == 2)
					continue;
                if (!ObjectsIntersecting(obj, test))
                {
                    OnObjectExitedTrigger(obj, test);
                    vector_remove_index(obj->dynamic.current_triggers, y);
                    y--;
                    if (test->objecttype != 0)
                    {
                        vector_remove(test->dynamic.current_triggers, obj);
                    }
                }
            }
        }
    }
}

void        UpdateSubworld(t_object *object, float deltatime)
{
    t_world *world = &object->subworld.subworld;
    UpdateWorld(world, deltatime);

    int i, y;

    for (y = 0; y < object->colliders.current; y++)
        ((t_collider *)(object->colliders.data[y]))->position = ((t_collider *)(object->colliders.data[y]))->offset;
    for (i = 0; i < world->object_list->last_free_slot; i++) {
        if (world->object_list->object_array[i] != NULL) {
            t_object *obj = world->object_list->object_array[i];
            if (!obj->objecttype)
                continue;
            if (!ObjectsIntersecting(object, obj))
            {
                OnObjectExitedTrigger(object, obj);
                for (y = 0; y < object->colliders.current; y++)
                    ((t_collider *)(object->colliders.data[y]))->position = ((t_collider *)(object->colliders.data[y]))->offset;
                vector_remove(object->dynamic.current_triggers, obj);
                vector_remove(obj->dynamic.current_triggers, object);
            }
        }
    }

    for (y = 0; y < object->colliders.current; y++)
        ((t_collider *)(object->colliders.data[y]))->position = add_vector3_to_vector3(((t_collider *)(object->colliders.data[y]))->offset, object->transform.position);
}

void        UpdateTPhysic(t_world *toupdate, float delta)
{
    toupdate->timeaccumulator += delta;

    if (toupdate->timeaccumulator > 0.3)
        toupdate->timeaccumulator = 0.3;

    while (toupdate->timeaccumulator > 0.01)
    {
        UpdateWorld(toupdate, 0.01);
        toupdate->timeaccumulator -= 0.01;
    }
}

t_vector *GetCloseColliders(t_world *world, t_vector3 position, double range, int layer)
{
	t_vector *result = vector_create_sized(10);
	t_sphere_collider collider;

	collider.collider.type = 2;
	collider.radius = range;
	collider.collider.aabbsize = (t_vector3) { range, range, range };
	collider.collider.position = position;

	GetCollidingObjectsNode(world->broadtree, &collider, result, layer);
	return result;
}

void		AddObjectToWorld(t_world *world, t_object *obj)
{
    int i;

    if (obj->objecttype == 0)
    {
        for (i = 0; i < obj->colliders.current; i++)
        {
            ((t_collider *)obj->colliders.data[i])->position = add_vector3_to_vector3(obj->transform.position, ((t_collider *)obj->colliders.data[i])->offset);
            AddColliderToNode(world->broadtree, obj->colliders.data[i]);
        }
    }
    else
    {
        putInSmartArray(world->object_list, obj);
    }
    obj->world = world;
}

void        RemoveObjectFromWorld(t_object *obj)
{
    int i;

    if (obj->world)
    {
        if (obj->objecttype == 0)
        {
            for (i = 0; i < obj->colliders.current; i++)
				RemoveColliderFromNode(obj->world->broadtree, obj->colliders.data[i]);
        }
        else
            removeValueFromSmartArray(obj->world->object_list, obj);
        obj->world = 0;
    }
}

t_raycast           Raycast(t_world *world, t_vector3 origin,
	t_vector3 direction, double range, char layer, t_raycast *ray)
{

    ray->origin = origin;
    ray->layer = layer;
    ray->direction = direction;
    ray->range = range;

    ray->invdirection.x = 1.0 / (ray->direction.x + 0.000001);
    ray->invdirection.y = 1.0 / (ray->direction.y + 0.000001);
    ray->invdirection.z = 1.0 / (ray->direction.z + 0.000001);
    ray->hitDistance = 10000000000000000000000.0;
    ray->hitCollider = NULL;

    NodeRaycast(world->broadtree, ray);
    return *ray;
}
