#ifndef __VECTOR3_H__
# define __VECTOR3_H__

# include "Util.h"

typedef struct	s_vector3
{
	union
	{
		double data[3];
		struct
		{
			double		x;
			double		y;
			double		z;
		};
	};
}				t_vector3;

typedef struct 	s_vector2
{
	double		x;
	double		y;
}				t_vector2;

# define VEC3DISTSQR(a, b) ABS((SQR(a.x - b.x) + SQR(a.y - b.y) + SQR(a.z - b.z)))

t_vector3       *new_vector3(double x, double y, double z);
t_vector3       add_vector3_to_vector3(t_vector3 from, t_vector3 to);
t_vector3       add_vector3_to_vector3_mult(t_vector3 from, t_vector3 to, double multiplier);
t_vector3       sub_vector3_from_vector3(t_vector3 to, t_vector3 from);
t_vector3       div_vector3_from_vector3(t_vector3 to, t_vector3 from);
t_vector3		mul_vector3_vector3(t_vector3 a, t_vector3 b);
t_vector3       mul_vector3(t_vector3 to, double mul);
t_vector3       div_vector3(t_vector3 to, double div);
double			vector3_scalarProduct(t_vector3 v1, t_vector3 v2);
double          vector3_magnitude(t_vector3 vec3);
t_vector3		vector3_crossproduct(t_vector3 a, t_vector3 b);
float			vector3_squared_magnitude(t_vector3 vec3);
float			vector3_angle(t_vector3 v1, t_vector3 v2);

t_vector3		vector3_normalized(t_vector3 v);
void 			vector3_normalize(t_vector3 *v);
t_vector3		vector3_abs(t_vector3 a);

int				vector3_getmaxaxis(t_vector3 target);
int				vector3_getminaxis(t_vector3 target);

#endif
