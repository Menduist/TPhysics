#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "vector.h"

typedef struct s_vector3 t_vector3;
typedef struct u_object t_object;

typedef struct	s_basecollider
{
	int			type;
	t_object	*ref;
	t_vector3	aabbsize;

	t_vector3 	offset;
	t_vector3 	position;
}				t_basecollider;

//Type 1
typedef struct	s_aabb_collider
{
	t_basecollider  collider;
	t_vector3	half_size;
}				t_aabb_collider;

//Type 2
typedef struct	s_sphere_collider
{
    t_basecollider	collider;
	double   	radius;
}				t_sphere_collider;

//Type 3
typedef struct	s_capsule_collider
{
	t_basecollider	collider;
	double		radius;
	t_vector3	extends;
}				t_capsule_collider;

//Type 4
typedef struct	s_cylinder_collider
{
	t_basecollider	collider;
	double		radius;
	double		height;
}				t_cylinder_collider;

//Type 5
typedef struct	s_convex_collider
{
	t_basecollider	collider;
	t_vector3	*points;
	int			pointcount;
}				t_convex_collider;

typedef union u_collider
{
	struct	{
		int			type;
		t_object	*ref;
		t_vector3	aabbsize;

		t_vector3 	offset;
		t_vector3 	position;
	};
	t_aabb_collider aabb;
	t_sphere_collider sphere;
	t_capsule_collider capsule;
	t_cylinder_collider cylinder;
	t_convex_collider convex;
}		      t_collider;

typedef struct	s_response
{
	t_vector3	push;
	t_vector3   normal;
	float 		length;

	t_collider	*a;
	t_collider	*b;
}				t_response;

/*
Init section
 */
t_collider		*TCreateSphereCollider(double radius);
t_collider		*TCreateBoxCollider(double x, double y, double z);
t_collider		*TCreateOrientedBoxCollider(double x, double y, double z, double yaw);
t_collider		*TCreateCapsuleCollider(double height, double radius);
t_collider		*TCreateCylinderCollider(double height, double radius);
t_collider		*TCreateConvexCollider(t_vector3 *points, int pointscount);

void			TObjectAddCollider(t_collider *collider, t_object *object);

/*
Test Section
 */
int				test_collider_collider(t_collider *collider1, t_collider *collider2, t_response *response);
int             test_aabb_aabb(t_collider *a, t_collider *b, t_response *resp);
int    			test_aabb_sphere(t_collider *a,	t_collider *b, t_response *resp);
int    			test_aabb_capsule(t_collider *a, t_collider *b, t_response *resp);
int    			test_aabb_cylinder(t_collider *aa, t_collider *bb, t_response *resp);

int				test_sphere_sphere(t_collider *a, t_collider *b, t_response *resp);
int				test_sphere_capsule(t_collider *aa, t_collider *bb, t_response *resp);
int				test_sphere_cylinder(t_collider *aa, t_collider *bb, t_response *resp);

int				test_capsule_cylinder(t_collider *aa, t_collider *bb, t_response *resp);
int				test_capsule_capsule(t_collider *aa, t_collider *bb, t_response *resp);


int				test_point_aabb(t_vector3 pt, t_aabb_collider b, t_response *resp);
/*
Response Section
 */
void			response_collider(t_response *r);

/*
Math Section
 */
void           	closest_pt_point_aabb(t_vector3 point, t_aabb_collider b, t_vector3 *pointout);
t_vector3		closest_pt_point_segment(t_vector3 point, t_vector3 sega, t_vector3 segb);
t_vector3		closest_pt_point_cylinder(t_vector3 point, float height, float radius);
float   clamp(float value, float min, float max);
#endif
