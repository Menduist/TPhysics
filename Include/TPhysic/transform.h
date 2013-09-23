#ifndef __TRANSFORM_H__
# define __TRANSFORM_H__

#include "vector.h"

typedef struct	s_transform
{
	t_vector3	position;
	t_vector3	rotation;
}				t_transform;

t_vector3		transform_position(t_vector3 position, t_transform to);
t_vector3		transform_position_inv(t_vector3 position, t_transform from);

#endif
