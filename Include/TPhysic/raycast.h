#ifndef __RAYCAST_H__
# define __RAYCAST_H__

# include "world.h"
# include "vector.h"

typedef struct		s_world t_world;

typedef struct      s_raycast
{
    t_vector3       origin;
    t_vector3       direction;
	t_vector3		invdirection;
    double          range;
    char            layer;

    t_vector3       hitPosition;
    double          hitDistance;
    t_collider     *hitCollider;
}                   t_raycast;

t_raycast           Raycast(t_world *world, t_vector3 origin,
	t_vector3 direction, double range, char layer, t_raycast *result);

#endif
