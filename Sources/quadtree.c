#include "quadtree.h"
#include "vector.h"
#include "mem.h"
#include "collider.h"
#include <math.h>
#include "assert.h"
#include "world.h"
#include "collisionlayer.h"

void SplitNode(t_treenode *tosplit);
void SplitNodeRecur(t_treenode *tosplit, int times);

t_treenode *BuildQuadtree(float width, float mindepth)
{
    t_treenode *root = (t_treenode *)calloc(1, sizeof(t_treenode));
    root->width = width / 2;
    root->colliderlist = vector_create();
    root->dyncolliderlist = vector_create();
    SplitNodeRecur(root, mindepth);

    return root;
}

void DestroyQuadTree(t_treenode **tree)
{
    if (!*tree)
        return;
    DestroyQuadTree(&(*tree)->childs[0]);
    DestroyQuadTree(&(*tree)->childs[1]);
    DestroyQuadTree(&(*tree)->childs[2]);
    DestroyQuadTree(&(*tree)->childs[3]);
    free(*tree);
    *tree = 0;
}

void SplitNodeRecur(t_treenode *tosplit, int times)
{
    if (times == 1)
        return;
    times--;
    SplitNode(tosplit);

    SplitNodeRecur(tosplit->childs[0], times);
    SplitNodeRecur(tosplit->childs[1], times);
    SplitNodeRecur(tosplit->childs[2], times);
    SplitNodeRecur(tosplit->childs[3], times);
}

void CreateChildNode(t_treenode *tosplit, int index)
{
    float newwidth = tosplit->width / 2;
    tosplit->childs[index] = calloc(1, sizeof(t_treenode));

    tosplit->childs[index]->center.x = tosplit->center.x + ((index & 1) ? newwidth : -newwidth);
    tosplit->childs[index]->center.y = tosplit->center.y + ((index & 2) ? newwidth : -newwidth);
    tosplit->childs[index]->width = newwidth;
    tosplit->childs[index]->colliderlist = vector_create();
    tosplit->childs[index]->dyncolliderlist = vector_create();
}

void SplitNode(t_treenode *tosplit) {
    if (!tosplit->childs[0])
    {
        float newwidth = tosplit->width / 2;
        float offset = newwidth;

        t_treenode *childs = calloc(4, sizeof(t_treenode));
        int i;
        for (i = 0; i < 4; i++)
        {
            childs[i].center.x = tosplit->center.x + ((i & 1) ? offset : -offset);
            childs[i].center.y = tosplit->center.y + ((i & 2) ? offset : -offset);
            childs[i].width = newwidth;
            childs[i].colliderlist = vector_create();
            childs[i].dyncolliderlist = vector_create();

            tosplit->childs[i] = &childs[i];
        }
    }
}

void AddColliderToNode(t_treenode *target, t_collider *collider)
{
    int childindex = 0;
    int insidechild = 1;

    assert(fabs(target->center.x - collider->position.x) < (target->width + collider->aabbsize.x));
    assert(fabs(target->center.y - collider->position.y) < (target->width + collider->aabbsize.y));

    do {
        float delta = target->center.x - collider->position.x;
        if (fabs(delta) < collider->aabbsize.x)
        {
            insidechild = 0;
            break;
        }
        if (-delta > 0.f) childindex = 1;

        delta = target->center.y - collider->position.y;
        if (fabs(delta) < collider->aabbsize.y)
        {
            insidechild = 0;
            break;
        }
        if (-delta > 0.f) childindex |= 1 << 1;
    } while(0);

    if (insidechild && target->width > 0.1)
    {
        if (!target->childs[childindex])
            CreateChildNode(target, childindex);
        AddColliderToNode(target->childs[childindex], collider);
    }
    else
    {
        vector_append(target->colliderlist, collider);
    }
}

void RemoveColliderFromNode(t_treenode *target, t_collider *collider)
{
    int childindex = 0;
    int insidechild = 1;

    assert(fabs(target->center.x - collider->position.x) < (target->width + collider->aabbsize.x));
    assert(fabs(target->center.y - collider->position.y) < (target->width + collider->aabbsize.y));

    do {
        float delta = target->center.x - collider->position.x;
        if (fabs(delta) < collider->aabbsize.x)
        {
            insidechild = 0;
            break;
        }
        if (-delta > 0.f) childindex = 1;

        delta = target->center.y - collider->position.y;
        if (fabs(delta) < collider->aabbsize.y)
        {
            insidechild = 0;
            break;
        }
        if (-delta > 0.f) childindex |= 1 << 1;
    } while(0);

    if (insidechild && target->childs[childindex])
    {
        RemoveColliderFromNode(target->childs[childindex], collider);
    }
    else
    {
        vector_remove(target->colliderlist, collider);
    }
}

void TestColliderWithNode(t_treenode *target, t_collider *collider, float delta)
{
    int i;

    if (!target)
        return;
    for (i = 0; i < target->colliderlist->current; ++i)
    {
        TestsColliders(collider, target->colliderlist->data[i], delta);
    }
    for (i = 0; i < target->dyncolliderlist->current; ++i)
    {
        TestsColliders(collider, target->dyncolliderlist->data[i], delta);
    }
    TestColliderWithNode(target->childs[0], collider, delta);
    TestColliderWithNode(target->childs[1], collider, delta);
    TestColliderWithNode(target->childs[2], collider, delta);
    TestColliderWithNode(target->childs[3], collider, delta);
}

void TestColliderWithNodeInsert(t_treenode *target, t_collider *collider, float delta)
{
    int i;

    int childindex = 0;
    int insidechild = 1;

    do {
        float delta = target->center.x - collider->position.x;
        if (fabs(delta) < collider->aabbsize.x)
        {
            insidechild = 0;
            break;
        }
        if (-delta > 0.f) childindex = 1;

        delta = target->center.y - collider->position.y;
        if (fabs(delta) < collider->aabbsize.y)
        {
            insidechild = 0;
            break;
        }
        if (-delta > 0.f) childindex |= 1 << 1;
    } while(0);

    if (insidechild && target->childs[childindex])
    {
        TestColliderWithNodeInsert(target->childs[childindex], collider, delta);

        for (i = 0; i < target->colliderlist->current; ++i)
        {
            TestsColliders(collider, target->colliderlist->data[i], delta);
        }
        for (i = 0; i < target->dyncolliderlist->current; ++i)
        {
            TestsColliders(collider, target->dyncolliderlist->data[i], delta);
        }
    }
    else
    {
        TestColliderWithNode(target, collider, delta);
        vector_append(target->dyncolliderlist, collider);
    }
}

void GetCollidingObjectsNode(t_treenode *target, t_collider *collider, t_vector *list, int layer)
{
	int i;

	if (!target
		|| fabs(target->center.x - collider->position.x) > (target->width + collider->aabbsize.x)
		|| fabs(target->center.y - collider->position.y) > (target->width + collider->aabbsize.y))
		return;

	if (layer < 0 || layer == STATIC)
	{
		for (i = 0; i < target->colliderlist->current; ++i)
		{
			if (test_collider_collider(collider, target->colliderlist->data[i], 0))
				vector_append(list, target->colliderlist->data[i]);
		}
	}
	if (layer != STATIC)
	{
		for (i = 0; i < target->dyncolliderlist->current; ++i)
		{
			if (layer < 0 || layer == ((t_collider *)(target->dyncolliderlist->data[i]))->ref->layer)
			{
				if (test_collider_collider(collider, target->dyncolliderlist->data[i], 0))
					vector_append(list, target->dyncolliderlist->data[i]);
			}
		}
	}
	GetCollidingObjectsNode(target->childs[0], collider, list, layer);
	GetCollidingObjectsNode(target->childs[1], collider, list, layer);
	GetCollidingObjectsNode(target->childs[2], collider, list, layer);
	GetCollidingObjectsNode(target->childs[3], collider, list, layer);
}

void ClearAllDynsOfNode(t_treenode *target)
{
    if (!target)
        return;
    vector_clear(target->dyncolliderlist);
    ClearAllDynsOfNode(target->childs[0]);
    ClearAllDynsOfNode(target->childs[1]);
    ClearAllDynsOfNode(target->childs[2]);
    ClearAllDynsOfNode(target->childs[3]);
}

void NodeRaycast(t_treenode *target, t_raycast *ray)
{
    int i;
    double t;

    if (!target)
        return;
    for (i = 0; i < target->dyncolliderlist->current; ++i)
    {
        t_collider *current = target->dyncolliderlist->data[i];

        double t1 = ((current->position.x - current->aabbsize.x) - ray->origin.x) * ray->invdirection.x;
        double t2 = ((current->position.x + current->aabbsize.x) - ray->origin.x) * ray->invdirection.x;
        double t3 = ((current->position.y - current->aabbsize.y) - ray->origin.y) * ray->invdirection.y;
        double t4 = ((current->position.y + current->aabbsize.y) - ray->origin.y) * ray->invdirection.y;
        double t5 = ((current->position.z - current->aabbsize.z) - ray->origin.z) * ray->invdirection.z;
        double t6 = ((current->position.z + current->aabbsize.z) - ray->origin.z) * ray->invdirection.z;

        float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
        float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

        if (tmax < 0)
            continue;
        if (tmax > tmin)
            continue;
        // Collision
        if (tmin < ray->hitDistance) {
            ray->hitDistance = tmin;
            ray->hitCollider = current;
            ray->hitPosition = add_vector3_to_vector3(ray->origin,
                mul_vector3(ray->direction, tmin));
        }
    }
    NodeRaycast(target->childs[0], ray);
    NodeRaycast(target->childs[1], ray);
    NodeRaycast(target->childs[2], ray);
    NodeRaycast(target->childs[3], ray);
}
