#ifndef __OBJECT_H__
# define __OBJECT_H__

# include "Util.h"
# include "transform.h"
# include "mem.h"
# include "world.h"


typedef struct s_world t_world;
typedef union u_collider t_collider;

typedef struct	s_staticobject
{
	int			objecttype;
	t_transform	transform;
	t_vector  	colliders;

	t_world		*world;

	void		*userdata;
	double 		inversemass;
	double		friction;
	double		bounciness;
	t_vector3	velocity;
	char		layer;
	int			tag;
}				t_staticobject;

typedef struct s_dobject
{
	t_staticobject object;

	t_vector3   previousPos;

	t_vector3   force;

	t_vector	*current_triggers;

	int			groundedangle;
	t_vector3	groundnormal;
}				t_dobject;

typedef struct s_subworldobject
{
	t_dobject  dobject;

	struct s_world	subworld;
}				t_subworldobject;

typedef struct u_object {
	union
	{
		struct
		{
			int			objecttype;
			t_transform	transform;
			t_vector  	colliders;

			t_world		*world;

			void		*userdata;
			double 		inversemass;
			double		friction;
			double		bounciness;
			t_vector3	velocity;
			char		layer;
			int			tag;
		};
		t_dobject dynamic;
		t_subworldobject subworld;
	};
} t_object;


t_object		*TObjectCreateStatic(void *userdata, char layer);
t_object		*TObjectCreateDynamic(void *userdata, char layer);
t_object		*TObjectCreateSubworld(void *userdata, char layer, int subworldsize);

void			TSetObjectPosition(t_object *obj, double x, double y, double z);

void            MoveObjectToSubworld(t_object *subworld, t_object *object);
void            MoveObjectToWorld(t_object *subworld, t_object *object);
void            UpdateObject(t_object *object, float deltatime);
#endif
