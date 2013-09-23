#ifndef __COLLIDERGJK_H__
#define __COLLIDERGJK_H__

#include "collider.h"

int     test_collider_collider_gjk(t_collider *collider1, t_collider *collider2, t_response *response);
void    epa_collider_collider(t_collider *collider1, t_collider *collider2, t_response *response,
            t_vector3 a, t_vector3 b, t_vector3 c, t_vector3 d);

t_vector3 support_aabb(t_collider *aabb, t_vector3 d);
t_vector3 support_sphere(t_collider *sphere, t_vector3 d);
t_vector3 support_capsule(t_collider *capsule, t_vector3 d);
t_vector3 support_cylinder(t_collider *cylinder, t_vector3 d);
t_vector3 support_convex(t_collider *convex, t_vector3 d);

/*
** MATH
*/
t_vector3 cross_aba(t_vector3 a, t_vector3 b);
#endif
