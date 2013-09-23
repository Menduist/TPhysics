#include "vector.h"
#include "collider.h"
#include <stdlib.h>
#include <math.h>
#include "world.h"
#include "collidergjk.h"
#include "object.h"

#include "assert.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define CLAMP(min, max, x) MAX(MIN(x, max), min)

t_collider		*TCreateBoxCollider(double x, double y, double z)
{
	t_collider	*collider;
	
	collider = (t_collider *) calloc(1, sizeof(t_aabb_collider));

	collider->aabb.half_size.x = x;
	collider->aabb.half_size.y = y;
	collider->aabb.half_size.z = z;
	collider->type = 1;

	collider->aabbsize = (t_vector3){x, y, z};

	return (collider);
}

t_collider		*TCreateOrientedBoxCollider(double x, double y, double z, double yaw)
{
	double mcos = cos(yaw * 0.0174533);
	if (1.f - fabs(mcos) < 0.01)
		return TCreateBoxCollider(x, y, z);
	if (fabs(mcos) < 0.01)
		return TCreateBoxCollider(y, x, z);

	t_vector3 *points = malloc(sizeof(t_vector3) * 8);
	double msin = sin(yaw * 0.0174533);

	points[0].x = x * mcos - y * msin;
	points[0].y = x * msin + y * mcos;

	points[1].x = x * mcos - -y * msin;
	points[1].y = x * msin + -y * mcos;

	points[2].x = -x * mcos - y * msin;
	points[2].y = -x * msin + y * mcos;

	points[3].x = -x * mcos - -y * msin;
	points[3].y = -x * msin + -y * mcos;

	int i;
	for (i = 0; i < 4; i++)
	{
		points[i].z = -z;

		points[i + 4].z = z;
		points[i + 4].x = points[i].x;
		points[i + 4].y = points[i].y;
	}
	return TCreateConvexCollider(points, 8);
}

t_collider		*TCreateSphereCollider(double radius)
{
	t_collider	*collider;

	collider = (t_collider *)calloc(1, sizeof(t_sphere_collider));
	collider->sphere.radius = radius;
	collider->type = 2;

	collider->aabbsize = (t_vector3){radius, radius, radius};

	return (collider);
}

t_collider		*TCreateCapsuleCollider(double height, double radius)
{
	t_collider	*collider;
	collider = (t_collider *)calloc(1, sizeof(t_capsule_collider));
	collider->capsule.radius = radius;
	collider->capsule.extends = (t_vector3){0, 0, height};
	collider->type = 3;

	collider->aabbsize = (t_vector3){radius, radius, radius + height};
	return collider;
}

t_collider		*TCreateCylinderCollider(double height, double radius)
{
	t_collider	*collider;
	collider = (t_collider *)calloc(1, sizeof(t_cylinder_collider));
	collider->cylinder.radius = radius;
	collider->cylinder.height = height;
	collider->type = 4;

	collider->aabbsize = (t_vector3){radius, radius, height};
	return collider;
}

t_collider		*TCreateConvexCollider(t_vector3 *points, int pointscount)
{
	t_collider	*collider;
	collider = (t_collider *)calloc(1, sizeof(t_convex_collider));
	collider->convex.points = points;
	collider->convex.pointcount = pointscount;
	collider->type = 5;

	t_vector3 max;
	int i;
	max.x = max.y = max.z = -100000;

	for (i = 0; i < pointscount; i++)
	{
		if (fabs(points[i].x) > max.x)
			max.x = fabs(points[i].x);

		if (fabs(points[i].y) > max.y)
			max.y = fabs(points[i].y);

		if (fabs(points[i].z) > max.z)
			max.z = fabs(points[i].z);
	}

	collider->aabbsize = max;
	return collider;
}

void			TObjectAddCollider(t_collider *collider, t_object *object)
{
	vector_append(&object->colliders, collider);
	collider->ref = object;
}

/*
Logic Section
 */

typedef int (*collidertest)(t_collider *a, t_collider *, t_response *response);

int		test_collider_collider(t_collider *collider1, t_collider *collider2, t_response *response)
{
	static collidertest testcollision[3][4] =
	{
		//				AABB				SPHERE			   CAPSULE		  CYLINDER
		{	&test_aabb_aabb,	&test_aabb_sphere,  &test_aabb_capsule, 	&test_aabb_cylinder},
		{				0, 	  &test_sphere_sphere,&test_sphere_capsule, 	&test_sphere_cylinder},
		{				0, 	  					0,&test_capsule_capsule,   &test_capsule_cylinder}
	};
	t_collider *c;
	int 		collision;

	collision = 0;
	if (collider1->type > collider2->type)
	{
		c = collider1;
		collider1 = collider2;
		collider2 = c;
	}
	if (collider2->type == 5)
		return test_collider_collider_gjk(collider1, collider2, response);

	collision = testcollision[collider1->type - 1][collider2->type - 1]
											(collider1, collider2, response);

	return collision;
}



/*
response Section
 */
void	response_collider(t_response *r)
{
	t_object *a = r->a->ref;
	t_object *b = r->b->ref;
	t_vector3 relvelocity = sub_vector3_from_vector3(b->velocity,
														a->velocity);

	float dot = vector3_scalarProduct(relvelocity, r->normal);

	int groundangle = vector3_scalarProduct(r->normal, (t_vector3){0, 0, 1}) * 100 + 101;
	if (a->objecttype != 0 && (a->dynamic.groundedangle == 0 || a->dynamic.groundedangle > groundangle))
	{
		a->dynamic.groundedangle = groundangle;
		a->dynamic.groundnormal = r->normal;
	}
	if (b->objecttype != 0 && (b->dynamic.groundedangle == 0 || b->dynamic.groundedangle > 202 - groundangle))
	{
		b->dynamic.groundedangle = 202 - groundangle;
		b->dynamic.groundnormal = mul_vector3(r->normal, -1);
	}

	if (dot > 0)
		return;

	float j = -(1 + MAX(a->bounciness, b->bounciness)) * dot;
	j /= a->inversemass + b->inversemass;

	t_vector3 impulse = mul_vector3(r->normal, j);
	a->velocity = add_vector3_to_vector3(a->velocity, mul_vector3(impulse, -a->inversemass));
	b->velocity = add_vector3_to_vector3(b->velocity, mul_vector3(impulse, b->inversemass));

	float correctionlength = r->length - 0.01;
	if (correctionlength < 0)
		correctionlength = 0;
	t_vector3 correction = mul_vector3(r->normal, correctionlength /
		(a->inversemass + b->inversemass));
	a->transform.position = add_vector3_to_vector3(a->transform.position, mul_vector3(correction, -a->inversemass));
	b->transform.position = add_vector3_to_vector3(b->transform.position, mul_vector3(correction, b->inversemass));


	relvelocity = sub_vector3_from_vector3(b->velocity,
														a->velocity);
	t_vector3 dotnorm = mul_vector3(r->normal, vector3_scalarProduct(relvelocity, r->normal));
	t_vector3 tangeant = sub_vector3_from_vector3(relvelocity, dotnorm);
	vector3_normalize(&tangeant);

	float jt = -vector3_scalarProduct(relvelocity, tangeant);
	jt /= a->inversemass + b->inversemass;

	if (fabs(jt) < 0.0001 || a->friction == 0 || b->friction == 0)
		return;

	float mu = (a->friction + b->friction) / 2.f;

	t_vector3 frictionImpulse;
	if (fabs(jt) < j * mu)
		frictionImpulse = mul_vector3(tangeant, jt);
	else
	{
		frictionImpulse = mul_vector3(tangeant, -j * (a->friction + b->friction) / 3.f);
	}


	a->velocity = add_vector3_to_vector3(a->velocity, mul_vector3(frictionImpulse, -a->inversemass));
	b->velocity = add_vector3_to_vector3(b->velocity, mul_vector3(frictionImpulse, b->inversemass));
}

/*
Test Section
 */

int     test_aabb_aabb(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 1);
	assert(bb->type == 1);
	t_aabb_collider a = aa->aabb;
	t_aabb_collider b = bb->aabb;
	t_vector3 relpos = sub_vector3_from_vector3(bb->position,
													aa->position);

	t_vector3 overlap = (t_vector3){0, 0, 0};
	overlap.x = a.half_size.x + b.half_size.x - fabs(relpos.x);
	if (overlap.x > 0)
	{
		overlap.y = a.half_size.y + b.half_size.y - fabs(relpos.y);

		if (overlap.y > 0)
		{
			overlap.z = a.half_size.z + b.half_size.z - fabs(relpos.z);

			if (overlap.z > 0)
			{
				if (resp)
				{
					int maxaxis = vector3_getminaxis(overlap);

					resp->normal = (t_vector3){0.0, 0.0, 0.0};
					if (relpos.data[maxaxis] < 0)
					{
						resp->normal.data[maxaxis] = -1;
					}
					else
					{
						resp->normal.data[maxaxis] = 1;
					}
					resp->length = overlap.data[maxaxis];
					resp->a = aa;
					resp->b = bb;
				}
				return 1;
			}
		}
	}
    return 0;
}

int     test_aabb_sphere(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 1);
	assert(bb->type == 2);
	t_sphere_collider a = bb->sphere;
	t_aabb_collider b = aa->aabb;

	t_vector3   pBox;
	t_vector3   delta;
	t_vector3	relpos;
	t_vector3	pDistBox;
	float		distance;

	int minaxis;

	relpos = sub_vector3_from_vector3(bb->position, aa->position);
	pBox = (t_vector3){clamp(relpos.x, -b.half_size.x, b.half_size.x),
						clamp(relpos.y, -b.half_size.y, b.half_size.y),
						clamp(relpos.z, -b.half_size.z, b.half_size.z)};

	delta = sub_vector3_from_vector3(relpos, pBox);
	distance = vector3_squared_magnitude(delta);

	if (distance > a.radius * a.radius)
		return 0;
	if (resp)
	{
		if (distance < 0.000001f)
		{
			pDistBox = sub_vector3_from_vector3(b.half_size, vector3_abs(pBox));
			minaxis = vector3_getminaxis(pDistBox);
			if (pBox.data[minaxis] > 0)
				pBox.data[minaxis] = b.half_size.data[minaxis];
			else
				pBox.data[minaxis] = -b.half_size.data[minaxis];
			resp->normal = sub_vector3_from_vector3(relpos, pBox);
			resp->length = vector3_magnitude(resp->normal);
			resp->normal = mul_vector3(resp->normal, -(1 / resp->length));
			resp->length += a.radius;
		}
		else
		{
			resp->normal = sub_vector3_from_vector3(relpos, pBox);
			resp->length = vector3_magnitude(resp->normal);
			resp->normal = mul_vector3(resp->normal, 1 / resp->length);
			resp->length = a.radius - resp->length;
		}

		resp->a = aa;
		resp->b = bb;
	}

	return 1;
}

int    			test_aabb_capsule(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 1);
	assert(bb->type == 3);
	t_capsule_collider a = bb->capsule;
	t_aabb_collider b = aa->aabb;

	t_vector3 closest = closest_pt_point_segment(aa->position, sub_vector3_from_vector3(bb->position, a.extends), add_vector3_to_vector3(bb->position, a.extends));


	t_vector3   pBox;
	t_vector3   delta;
	t_vector3	relpos;
	t_vector3	pDistBox;
	float		distance;

	int minaxis;

	relpos = sub_vector3_from_vector3(closest, aa->position);
	pBox = (t_vector3){clamp(relpos.x, -b.half_size.x, b.half_size.x),
						clamp(relpos.y, -b.half_size.y, b.half_size.y),
						clamp(relpos.z, -b.half_size.z, b.half_size.z)};

	delta = sub_vector3_from_vector3(relpos, pBox);
	distance = vector3_squared_magnitude(delta);

	if (distance > a.radius * a.radius)
		return 0;
	if (resp)
	{
		if (distance < 0.000001f)
		{
			pDistBox = sub_vector3_from_vector3(b.half_size, vector3_abs(pBox));
			minaxis = vector3_getminaxis(pDistBox);
			if (pBox.data[minaxis] > 0)
				pBox.data[minaxis] = b.half_size.data[minaxis];
			else
				pBox.data[minaxis] = -b.half_size.data[minaxis];
			resp->normal = sub_vector3_from_vector3(relpos, pBox);
			resp->length = vector3_magnitude(resp->normal);
			resp->normal = mul_vector3(resp->normal, -(1 / resp->length));
			resp->length += a.radius;
		}
		else
		{
			resp->normal = sub_vector3_from_vector3(relpos, pBox);
			resp->length = vector3_magnitude(resp->normal);
			resp->normal = mul_vector3(resp->normal, 1 / resp->length);
			resp->length = a.radius - resp->length;
		}

		resp->a = aa;
		resp->b = bb;
	}

	return 1;
}

int    			test_aabb_cylinder(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 1);
	assert(bb->type == 4);
	t_cylinder_collider a = bb->cylinder;
	t_aabb_collider b = aa->aabb;

	t_vector3 extents = (t_vector3){0, 0, a.height};
	t_vector3 closest = closest_pt_point_segment(aa->position, sub_vector3_from_vector3(bb->position, extents), add_vector3_to_vector3(bb->position, extents));


	t_vector3   pBox;
	t_vector3   delta;
	t_vector3	relpos;
	t_vector3	reelpos;
	t_vector3	pDistBox;
	float		distance;

	int minaxis;

	relpos = sub_vector3_from_vector3(bb->position, aa->position);
	reelpos = sub_vector3_from_vector3(closest, aa->position);
	pBox = (t_vector3){clamp(relpos.x, -b.half_size.x, b.half_size.x),
						clamp(relpos.y, -b.half_size.y, b.half_size.y),
						clamp(relpos.z, -b.half_size.z, b.half_size.z)};

	delta = sub_vector3_from_vector3(relpos, pBox);
	distance = vector3_squared_magnitude(delta);

	if (distance > a.radius * a.radius)
		return 0;
	if (distance < 0.000001f)
	{
		if (resp)
		{
			pDistBox = sub_vector3_from_vector3(b.half_size, vector3_abs(pBox));
			minaxis = vector3_getminaxis(pDistBox);
			if (pBox.data[minaxis] > 0)
				pBox.data[minaxis] = b.half_size.data[minaxis];
			else
				pBox.data[minaxis] = -b.half_size.data[minaxis];
			resp->normal = sub_vector3_from_vector3(relpos, pBox);
			resp->normal.x += resp->normal.x > 0 ? a.radius : -a.radius;
			resp->normal.y += resp->normal.y > 0 ? a.radius : -a.radius;
			resp->length = vector3_magnitude(resp->normal);
			resp->normal = mul_vector3(resp->normal, -(1 / resp->length));
		}
	}
	else
	{
		t_vector3 abspos = add_vector3_to_vector3(pBox, aa->position);
		if (fabs(abspos.z - bb->position.z) > a.height)
			return 0;
		if (resp)
		{
			t_vector3 capsulecorrection = sub_vector3_from_vector3(relpos, pBox);
			float magnitude = vector3_magnitude(capsulecorrection);
			magnitude = a.radius - magnitude;

			float aabbcorrection = b.half_size.z + a.height - fabs(relpos.z);

			if (magnitude < aabbcorrection)
			{
				resp->normal = capsulecorrection;
				resp->length = vector3_magnitude(resp->normal);
				resp->normal = mul_vector3(resp->normal, 1 / resp->length);
				resp->length = a.radius - resp->length;
			}
			else
			{
				resp->normal.x = 0;
				resp->normal.y = 0;
				resp->normal.z = reelpos.z > 0 ? 1 : -1;
				resp->length = aabbcorrection;
			}
		}
	}

	if (resp)
	{
		resp->a = aa;
		resp->b = bb;
	}

	return 1;
}

int				test_sphere_sphere(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 2);
	assert(bb->type == 2);
	t_sphere_collider a = aa->sphere;
	t_sphere_collider b = bb->sphere;

	t_vector3 relpos = sub_vector3_from_vector3(bb->position, aa->position);

	if (vector3_squared_magnitude(relpos) > (a.radius + b.radius) * (a.radius + b.radius))
		return 0;
	if (resp)
	{
		resp->length = a.radius + b.radius - vector3_magnitude(relpos);
		vector3_normalize(&relpos);
		resp->normal = relpos;
		resp->a = aa;
		resp->b = bb;
	}
	return 1;
}

int				test_sphere_capsule(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 2);
	assert(bb->type == 3);
	t_sphere_collider a = aa->sphere;
	t_capsule_collider b = bb->capsule;

	t_vector3 closest = closest_pt_point_segment(aa->position, sub_vector3_from_vector3(bb->position, b.extends), add_vector3_to_vector3(bb->position, b.extends));

	t_vector3 relpos = sub_vector3_from_vector3(aa->position, closest);

	if (vector3_squared_magnitude(relpos) > (a.radius + b.radius) * (a.radius + b.radius))
		return 0;
	if (resp)
	{
		resp->length = a.radius + b.radius - vector3_magnitude(relpos);
		vector3_normalize(&relpos);
		resp->normal = relpos;
		resp->a = aa;
		resp->b = bb;
	}
	return 1;
}

int				test_sphere_cylinder(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 2);
	assert(bb->type == 4);
	t_sphere_collider a = aa->sphere;
	t_cylinder_collider b = bb->cylinder;

	t_vector3 relpos = sub_vector3_from_vector3(aa->position, bb->position);
	t_vector3 closestrel = closest_pt_point_cylinder(relpos, b.height, b.radius);
	t_vector3 closestabs = add_vector3_to_vector3(bb->position, closestrel);

	t_vector3 closest_relpos = sub_vector3_from_vector3(closestabs, aa->position);
	if (vector3_squared_magnitude(closest_relpos) > a.radius * a.radius)
		return 0;
	if (resp)
	{
		resp->length = a.radius - vector3_magnitude(closest_relpos);
		vector3_normalize(&closest_relpos);
		resp->normal = closest_relpos;
		resp->a = aa;
		resp->b = bb;
	}
	return 1;
}

int				test_capsule_cylinder(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 3);
	assert(bb->type == 4);
	t_capsule_collider a = aa->capsule;
	t_cylinder_collider b = bb->cylinder;

	t_vector3 capsclosest = closest_pt_point_segment(bb->position, sub_vector3_from_vector3(aa->position, a.extends), add_vector3_to_vector3(aa->position, a.extends));

	t_vector3 relpos = sub_vector3_from_vector3(capsclosest, bb->position);
	t_vector3 closestrel = closest_pt_point_cylinder(relpos, b.height, b.radius);
	t_vector3 closestabs = add_vector3_to_vector3(bb->position, closestrel);

	t_vector3 closest_relpos = sub_vector3_from_vector3(closestabs, capsclosest);
	if (vector3_squared_magnitude(closest_relpos) > a.radius * a.radius)
		return 0;
	if (resp)
	{
		resp->length = a.radius - vector3_magnitude(closest_relpos);
		vector3_normalize(&closest_relpos);
		resp->normal = closest_relpos;
		resp->a = aa;
		resp->b = bb;
	}
	return 1;
}

int				test_capsule_capsule(t_collider *aa, t_collider *bb, t_response *resp)
{
	assert(aa->type == 3);
	assert(bb->type == 3);
	t_capsule_collider a = aa->capsule;
	t_capsule_collider b = bb->capsule;

	t_vector3 aclosest = closest_pt_point_segment(bb->position, sub_vector3_from_vector3(aa->position, a.extends), add_vector3_to_vector3(aa->position, a.extends));
	t_vector3 bclosest = closest_pt_point_segment(aa->position, sub_vector3_from_vector3(bb->position, b.extends), add_vector3_to_vector3(bb->position, b.extends));

	t_vector3 relpos = sub_vector3_from_vector3(bclosest, aclosest);

	if (vector3_squared_magnitude(relpos) > (a.radius + b.radius) * (a.radius + b.radius))
		return 0;
	if (resp)
	{
		resp->length = a.radius + b.radius - vector3_magnitude(relpos);
		vector3_normalize(&relpos);
		resp->normal = relpos;
		resp->a = aa;
		resp->b = bb;
	}
	return 1;
}

int		test_point_aabb(t_vector3 pt, t_aabb_collider b, t_response *resp)
{
	if (pt.x < b.collider.position.x - b.half_size.x || pt.x > b.half_size.x + b.collider.position.x)
		return 0;
	if (pt.y < b.collider.position.y - b.half_size.y || pt.y > b.half_size.y + b.collider.position.y)
		return 0;
	if (pt.z < b.collider.position.z - b.half_size.z || pt.z > b.half_size.z + b.collider.position.z)
		return 0;

	resp->push.x = 0;
	resp->push.y = 0;
	resp->push.z = 0;
	resp->length = 0;
	return 1;
}

/*
Math Secton
 */

void           closest_pt_point_aabb(t_vector3 point, t_aabb_collider b, t_vector3 *pointout)
{
	double       v;
	double       min;
	double       max;

	v = 0;
	min = b.collider.position.x - b.half_size.x;
	max = b.collider.position.x + b.half_size.x;
	if (point.x < min)
		v = min;
	else if (point.x > max)
		v = max;
	else
		v = point.x;
	pointout->x = v;

	v = 0;
	min = b.collider.position.y - b.half_size.y;
	max = b.collider.position.y + b.half_size.y;
	if (point.y < min)
		v = min;
	else if (point.y > max)
		v = max;
	else
		v = point.y;
	pointout->y = v;

	v = 0;
	min = b.collider.position.z - b.half_size.z;
	max = b.collider.position.z + b.half_size.z;
	if (point.z < min)
		v = min;
	else if (point.z > max)
		v = max;
	else
		v = point.z;
	pointout->z = v;
}


t_vector3		closest_pt_point_segment(t_vector3 point, t_vector3 sega, t_vector3 segb)
{
	t_vector3	ab = sub_vector3_from_vector3(segb, sega);

	float t = vector3_scalarProduct(sub_vector3_from_vector3(point, sega), ab) / vector3_squared_magnitude(ab);
	t = clamp(t, 0, 1);

	return add_vector3_to_vector3_mult(ab, sega, t);
}

t_vector3		closest_pt_point_cylinder(t_vector3 point, float height, float radius)
{
	t_vector3 tmp = point;
	tmp.z = 0;
	if (vector3_squared_magnitude(tmp) > radius * radius)
	{
		vector3_normalize(&tmp);
		tmp = mul_vector3(tmp, radius);
	}
	tmp.z = clamp(point.z, -height, height);
	return tmp;
}

float   clamp(float value, float min, float max)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}
