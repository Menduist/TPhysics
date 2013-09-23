#ifndef __WORLD_H__
# define __WORLD_H__

# include "transform.h"
# include "smartArray.h"
# include "mem.h"
# include "collider.h"
# include "raycast.h"

typedef struct s_treenode t_treenode;

# define MAXOBJECT 1000

typedef struct u_object t_object;

typedef struct		s_world
{
    t_smart_array	*object_list;
    t_treenode      *broadtree;
    double          timeaccumulator;
}					t_world;

t_world				*TWorldCreate(float size);
void				AddObjectToWorld(t_world *world, t_object *obj);
void                RemoveObjectFromWorld(t_object *obj);
void                UpdateTPhysic(t_world *toupdate, float delta);

t_vector			*GetCloseColliders(t_world *world, t_vector3 position, double range, int layer);

void				UpdateWorld(t_world *world, float deltatime);

void                TestsColliders(t_collider *a, t_collider *b, float delta);

extern void         OnCollision(t_object *a, t_object *b, float deltatime);
extern void         OnObjectEnteredTrigger(t_object *trigger, t_object *entered);
extern void         OnObjectExitedTrigger(t_object *trigger, t_object *exited);
extern void         OnLogMessage(char *str);

#endif
