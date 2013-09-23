#include "collidergjk.h"
#include "transform.h"
#include "object.h"

#include "math.h"

typedef t_vector3 (*collider_support)(t_collider *a, t_vector3 d);

int     test_collider_collider_gjk(t_collider *collider1, t_collider *collider2, t_response *response)
{
	static collider_support staticsupport[6] = {
		0,
        support_aabb,
        support_sphere,
        support_capsule,
        support_cylinder,
        support_convex
    };
    collider_support support[2] = {staticsupport[collider1->type], staticsupport[collider2->type]};
    t_vector3 vector = (t_vector3){1, 0, 0};
    int n = 0;

    t_vector3 b, c, d;
    t_vector3 ao, ab, ac, abc, abp, acp;
    t_vector3 ad, acd, adb;

	if (response)
	{
		response->a = collider1;
		response->b = collider2;
		response->length = 0;
		response->normal = (t_vector3){ 0, 0, 1 };
	}

    for (;;)
    {
        t_vector3 a = sub_vector3_from_vector3(support[0](collider1, vector), support[1](collider2, mul_vector3(vector, -1)));

        if (vector3_scalarProduct(a, vector) < 0)
            return 0;

        switch (n) {
            case 0:
                b = a;
                vector = mul_vector3(a, -1);
                n++;
                break;
            case 1:
                vector = cross_aba(sub_vector3_from_vector3(b, a), mul_vector3(a, -1));
                c = b;
                b = a;
                n++;
                break;
            case 2: ;
                ao = mul_vector3(a, -1);
                ab = sub_vector3_from_vector3(b, a);
                ac = sub_vector3_from_vector3(c, a);

                abc = vector3_crossproduct(ab, ac);

                abp = vector3_crossproduct(ab, abc);

                if (vector3_scalarProduct(abp, ao) > 0)
                {
                    c = b;
                    b = a;

                    vector = cross_aba(ab, ao);
                    break;
                }

                acp = vector3_crossproduct(abc, ac);
                if (vector3_scalarProduct(acp, ao) > 0)
                {
                    b = a;
                    vector = cross_aba(ac, ao);
                    break;
                }

                if (vector3_scalarProduct(abc, ao) > 0)
                {
                    d = c;
                    c = b;
                    b = a;

                    vector = abc;
                }
                else
                {
                    d = b;
                    b = a;

                    vector = mul_vector3(abc, -1);
                }
                n++;
                break;
            case 3: ;
                ao = mul_vector3(a, -1);
                ab = sub_vector3_from_vector3(b, a);
                ac = sub_vector3_from_vector3(c, a);

                abc = vector3_crossproduct(ab, ac);

                if (vector3_scalarProduct(abc, ao) > 0)
                {
                    goto check_face;
                }

                ad = sub_vector3_from_vector3(d, a);
                acd = vector3_crossproduct(ac, ad);

                if (vector3_scalarProduct(acd, ao) > 0)
                {
                    b = c;
                    c = d;

                    ab = ac;
                    ac = ad;

                    abc = acd;

                    goto check_face;
                }

                adb = vector3_crossproduct(ad, ab);

                if(vector3_scalarProduct(adb, ao) > 0)
                {
                    c = b;
                    b = d;

                    ac = ab;
                    ab = ad;

                    abc = adb;

                    goto check_face;
                }

				if (response)
					epa_collider_collider(collider1, collider2, response, a, b, c, d);
                return true;
                check_face:
                abp = vector3_crossproduct(ab, abc);

                if(vector3_scalarProduct(abp, ao) > 0)
                {
                    c = b;
                    b = a;

                    vector = cross_aba(ab, ao);

                    n = 2;

                    break;
                }

                acp = vector3_crossproduct(abc, ac);
                if(vector3_scalarProduct(acp, ao) > 0)
                {
                    b = a;

                    vector = cross_aba(ac, ao);

                    n = 2;

                    break;
                }

                d = c;
                c = b;
                b = a;

                vector = abc;
                n = 3;
                break;
        }
    }
	return false;
}

typedef struct s_triangle {
	t_vector3 	points[3];
	t_vector3	normal;
	float 		disttocenter;
}				t_triangle;

typedef struct s_poly {
	int			trianglescount;
	t_triangle	triangles[600];
}				t_poly;

typedef struct s_edgelist {
	int			pointscount;
	t_vector3	points[3000];
}			   t_edgelist;

void inittriangle(t_triangle *result, t_vector3 a, t_vector3 b, t_vector3 c)
{
	result->points[0] = a;
	result->points[1] = b;
	result->points[2] = c;
	result->normal = vector3_normalized(vector3_crossproduct(sub_vector3_from_vector3(b, a), sub_vector3_from_vector3(c, a)));
	result->disttocenter = fabs(vector3_scalarProduct(result->normal, a));
}

void    epa_collider_collider(t_collider *collider1, t_collider *collider2, t_response *response,
			t_vector3 a, t_vector3 b, t_vector3 c, t_vector3 d)
{
	static collider_support staticsupport[6] = {
		0,
        support_aabb,
        support_sphere,
        support_capsule,
        support_cylinder,
        support_convex
    };
    collider_support support[2] = {staticsupport[collider1->type], staticsupport[collider2->type]};

	t_poly poly;
	t_edgelist edges;

	inittriangle(&poly.triangles[0], a, b, c);
	inittriangle(&poly.triangles[1], a, c, d);
	inittriangle(&poly.triangles[2], a, d, b);
	inittriangle(&poly.triangles[3], b, d, c);
	poly.trianglescount = 4;

	int iterations = 0;
	while (iterations++ < 11 && iterations >= 0)
	{
		float mindist = 10000;
		int mindistid = -1;
		int i, y;
		for (i = 0; i < poly.trianglescount; i++)
		{
			if (poly.triangles[i].disttocenter < mindist)
			{
				mindist = poly.triangles[i].disttocenter;
				mindistid = i;
			}
		}
		t_triangle closest = poly.triangles[mindistid];

		t_vector3 supported = sub_vector3_from_vector3(support[0](collider1, closest.normal), support[1](collider2, mul_vector3(closest.normal, -1)));
		if (vector3_scalarProduct(supported, closest.normal) - closest.disttocenter < 11 || iterations > 10 || poly.trianglescount > 550)
		{
			response->a = collider1;
			response->b = collider2;
			response->length = mindist;
			response->normal = closest.normal;
			return;
		}
		else
		{
			edges.pointscount = 0;
			for (i = 0; i < poly.trianglescount;)
			{
				if (vector3_scalarProduct(poly.triangles[i].normal, sub_vector3_from_vector3(supported, poly.triangles[i].points[0])) > 0)
				{
					if (edges.pointscount > 2980)
						break;
					edges.points[edges.pointscount + 0] = poly.triangles[i].points[0];
					edges.points[edges.pointscount + 1] = poly.triangles[i].points[1];
					edges.points[edges.pointscount + 2] = poly.triangles[i].points[1];
					edges.points[edges.pointscount + 3] = poly.triangles[i].points[2];
					edges.points[edges.pointscount + 4] = poly.triangles[i].points[2];
					edges.points[edges.pointscount + 5] = poly.triangles[i].points[0];
					edges.pointscount += 6;

					memcpy(&poly.triangles[i], &poly.triangles[i + 1], (poly.trianglescount - i) * sizeof(t_triangle));
					poly.trianglescount--;
					continue;
				}
				i++;
			}

			for (i = 0; i < edges.pointscount && poly.trianglescount < 600; i += 2)
			{
				// poly.triangles[poly.trianglescount++] = createtriangle(supported, edges.points[i], edges.points[i + 1]);
				inittriangle(&poly.triangles[poly.trianglescount++], supported, edges.points[i], edges.points[i + 1]);
			}
		}
	}
}

t_vector3 cross_aba(t_vector3 a, t_vector3 b)
{
    return vector3_crossproduct(vector3_crossproduct(a, b), a);
}

t_vector3 support_aabb(t_collider *aabb, t_vector3 d)
{
	return (d);
}

t_vector3 support_sphere(t_collider *sphere, t_vector3 d)
{
	d.x = (int)(d.x / 60) * 60.f;
	d.y = (int)(d.y / 60) * 60.f;
	d.z = (int)(d.z / 60) * 60.f;
	vector3_normalize(&d);
	d = mul_vector3(d, sphere->sphere.radius);
	d = add_vector3_to_vector3(sphere->position, d);
	return (d);
}

t_vector3 support_capsule(t_collider *capsule, t_vector3 d)
{
	return (d);
}

t_vector3 support_cylinder(t_collider *cylinder, t_vector3 d)
{
	t_vector3 result;
	result.x = d.x;
	result.y = d.y;
	result.z = 0;
	vector3_normalize(&result);
	result = mul_vector3(result, cylinder->cylinder.radius);
	result.z = d.z + cylinder->position.z > 0 ? cylinder->cylinder.height : -cylinder->cylinder.height;
	return (add_vector3_to_vector3(d, cylinder->position));
}

t_vector3 support_convex(t_collider *convex, t_vector3 d)
{
	float mindot = -100000;
	int i = 0;
	int closestpoint = -1;

	float actudot;
	while (i < convex->convex.pointcount)
	{
		actudot = vector3_scalarProduct(convex->convex.points[i], d);
		if (actudot > mindot)
		{
			mindot = actudot;
			closestpoint = i;
		}
		i++;
	}

	return add_vector3_to_vector3(convex->convex.points[closestpoint], convex->position);
}
