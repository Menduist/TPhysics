#ifndef __QUADTREE_H__
# define __QUADTREE_H__

# include "vector.h"
# include "object.h"
# include "mem.h"

typedef struct s_treenode {
    t_vector2 center;

    float width;

    struct s_treenode *childs[4];
    t_vector *colliderlist;
    t_vector *dyncolliderlist;
}           t_treenode;

t_treenode *BuildQuadtree(float width, float mindepth);
void DestroyQuadTree(t_treenode **tree);

void AddColliderToNode(t_treenode *target, t_collider *collider);
void RemoveColliderFromNode(t_treenode *target, t_collider *toremove);

void GetCollidingObjectsNode(t_treenode *target, t_collider *collider, t_vector *list, int layer);

void TestColliderWithNodeInsert(t_treenode *target, t_collider *collider, float delta);
void ClearAllDynsOfNode(t_treenode *target);

void NodeRaycast(t_treenode *target, t_raycast *ray);

#endif
