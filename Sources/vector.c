#include "vector.h"
#include <stdlib.h>
#include <math.h>

t_vector3       *new_vector3(double x, double y, double z)
{
	t_vector3   *vec3;

	vec3 = (t_vector3 *)malloc(sizeof(t_vector3));
	vec3->x = x;
	vec3->y = y;
	vec3->z = z;
	return (vec3);
}

t_vector3       add_vector3_to_vector3(t_vector3 from, t_vector3 to)
{
	to.x += from.x;
	to.y += from.y;
	to.z += from.z;
	return (to);
}

t_vector3       add_vector3_to_vector3_mult(t_vector3 from, t_vector3 to, double multiplier)
{
	to.x += from.x * multiplier;
	to.y += from.y * multiplier;
	to.z += from.z * multiplier;
	return (to);
}

t_vector3       sub_vector3_from_vector3(t_vector3 to, t_vector3 from)
{
	to.x -= from.x;
	to.y -= from.y;
	to.z -= from.z;
	return (to);
}

t_vector3       div_vector3_from_vector3(t_vector3 to, t_vector3 from)
{
	to.x /= from.x;
	to.y /= from.y;
	to.z /= from.z;
	return (to);
}

t_vector3		mul_vector3_vector3(t_vector3 a, t_vector3 b)
{
	return (t_vector3){a.x * b.x, a.y * b.y, a.z * b.z};
}

t_vector3       mul_vector3(t_vector3 to, double mul)
{
	to.x *= mul;
	to.y *= mul;
	to.z *= mul;
	return (to);
}

t_vector3       div_vector3(t_vector3 to, double div)
{
	to.x /= div;
	to.y /= div;
	to.z /= div;
	return (to);
}

float           Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//      y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

double			vector3_scalarProduct(t_vector3 v1, t_vector3 v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

t_vector3		vector3_crossproduct(t_vector3 a, t_vector3 b)
{
	return (t_vector3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

double          vector3_magnitude(t_vector3 vec3)
{
	return (sqrt((vec3.x * vec3.x) + (vec3.y * vec3.y)+ (vec3.z * vec3.z)));
	//return (Q_rsqrt((vec3.x * vec3.x) + (vec3.y * vec3.y)+ (vec3.z * vec3.z)));
}

float			vector3_squared_magnitude(t_vector3 vec3)
{
	return (float)((vec3.x * vec3.x) + (vec3.y * vec3.y) + (vec3.z * vec3.z));
}

float			vector3_angle(t_vector3 v1, t_vector3 v2)
{
	double scalar;
	double Magnitude1;
	double Magnitude2;
	double cos;


	scalar = vector3_scalarProduct(v1, v2);
	Magnitude1 = vector3_magnitude(v1);
	Magnitude2 = vector3_magnitude(v2);

	cos = scalar / sqrt(Magnitude1 * Magnitude2);
	return cos;
}

void 			vector3_normalize(t_vector3 *v)
{
	double magnitude = vector3_magnitude(*v);
	if (magnitude != 0)
	{
		*v = mul_vector3(*v, 1 / magnitude);
	}
}

t_vector3		vector3_normalized(t_vector3 v)
{
	double magnitude = vector3_magnitude(v);
	if (magnitude != 0)
	{
		v = mul_vector3(v, 1 / magnitude);
	}
	return v;
}


t_vector3		vector3_abs(t_vector3 a)
{
	a.x = fabs(a.x);
	a.y = fabs(a.y);
	a.z = fabs(a.z);
	return a;
}

int				vector3_getmaxaxis(t_vector3 target)
{
	target.x = fabs(target.x);
	target.y = fabs(target.y);
	target.z = fabs(target.z);
	if (target.x >= target.y && target.x >= target.z)
		return 0;
	if (target.y >= target.x && target.y >= target.z)
		return 1;
	return 2;
}

int				vector3_getminaxis(t_vector3 target)
{
	target.x = fabs(target.x);
	target.y = fabs(target.y);
	target.z = fabs(target.z);
	if (target.x <= target.y && target.x <= target.z)
		return 0;
	if (target.y <= target.x && target.y <= target.z)
		return 1;
	return 2;
}
