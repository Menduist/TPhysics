#include "world.h"
#include "collider.h"
#include "object.h"
#include "transform.h"
#include "assert.h"
#include "collisionlayer.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    t_collider *shapes[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    shapes[0] = TCreateBoxCollider(1, 1, 1);
    shapes[1] = TCreateBoxCollider(1, 1, 1);
    shapes[2] = TCreateSphereCollider(1);
    shapes[3] = TCreateSphereCollider(1);
    shapes[4] = TCreateCapsuleCollider(1, 1);
    shapes[5] = TCreateCapsuleCollider(1, 1);
    shapes[6] = TCreateCylinderCollider(1, 1);
    shapes[7] = TCreateCylinderCollider(1, 1);

    printf("AABB-AABB Tests\n");
    shapes[1]->position.z = 1.9;
    assert(test_collider_collider(shapes[0], shapes[1], 0) == true);
    shapes[1]->position.z = 2.1;
    assert(test_collider_collider(shapes[0], shapes[1], 0) == false);
    shapes[1]->position.z = 0;

    printf("AABB-SPHERE Tests\n");
    shapes[2]->position.x = 1.9;
    assert(test_collider_collider(shapes[0], shapes[2], 0) == true);
    shapes[2]->position.y = 1.9;
    assert(test_collider_collider(shapes[0], shapes[2], 0) == false);
    shapes[2]->position.x = shapes[2]->position.y = 0;

    printf("AABB-CAPSULE Tests\n");
    shapes[4]->position.x = 1.9;
    assert(test_collider_collider(shapes[0], shapes[4], 0) == true);
    shapes[4]->position.z = 2.9;
    assert(test_collider_collider(shapes[0], shapes[4], 0) == false);
    shapes[4]->position.x = shapes[4]->position.z = 0;

    printf("AABB-CYLINDER Tests\n");
    shapes[6]->position.z = 1.9;
    assert(test_collider_collider(shapes[0], shapes[6], 0) == true);
    shapes[6]->position.z = 2.1;
    assert(test_collider_collider(shapes[0], shapes[6], 0) == false);
    shapes[6]->position.z = 0;

    printf("\nSPHERE-SPHERE Tests\n");
    shapes[3]->position.x = 1.9;
    assert(test_collider_collider(shapes[2], shapes[3], 0) == true);
    shapes[3]->position.y = 1.9;
    assert(test_collider_collider(shapes[2], shapes[3], 0) == false);
    shapes[3]->position.x = shapes[3]->position.y = 0;

    printf("SPHERE-CAPSULE Tests\n");
    shapes[4]->position.x = 1.9;
    assert(test_collider_collider(shapes[2], shapes[4], 0) == true);
    shapes[4]->position.z = 1.9;
    assert(test_collider_collider(shapes[2], shapes[4], 0) == false);
    shapes[4]->position.x = shapes[4]->position.z = 0;

    printf("SPHERE-CYLINDER Tests\n");
    shapes[6]->position.x = 1.9;
    assert(test_collider_collider(shapes[2], shapes[6], 0) == true);
    shapes[6]->position.y = 1.9;
    assert(test_collider_collider(shapes[2], shapes[6], 0) == false);
    shapes[6]->position.x = shapes[6]->position.y = 0;
    shapes[6]->position.z = 1.9;
    assert(test_collider_collider(shapes[2], shapes[6], 0) == true);
    shapes[6]->position.z = 2.1;
    assert(test_collider_collider(shapes[2], shapes[6], 0) == false);
    shapes[6]->position.z = 0;

    printf("\nCAPSULE-CAPSULE Tests\n");
    shapes[5]->position.x = 1.9;
    assert(test_collider_collider(shapes[4], shapes[5], 0) == true);
    shapes[5]->position.y = 1.9;
    assert(test_collider_collider(shapes[4], shapes[5], 0) == false);
    shapes[5]->position.y = 0;
    shapes[5]->position.z = 2.9;
    assert(test_collider_collider(shapes[4], shapes[5], 0) == false);
    shapes[5]->position.x = shapes[5]->position.z = 0;

    printf("CAPSULE-CYLINDER Tests\n");
    shapes[6]->position.x = 1.9;
    assert(test_collider_collider(shapes[4], shapes[6], 0) == true);
    shapes[6]->position.y = 1.9;
    assert(test_collider_collider(shapes[4], shapes[6], 0) == false);
    shapes[6]->position.x = shapes[6]->position.y = 0;
    shapes[6]->position.z = 1.9;
    assert(test_collider_collider(shapes[4], shapes[6], 0) == true);
    shapes[6]->position.z = 3.1;
    assert(test_collider_collider(shapes[4], shapes[6], 0) == false);
    shapes[6]->position.z = 0;

    printf("\nAll ok !\n");
    return (0);
}


void         OnCollision(t_object *a, t_object *b, float deltatime)
{

}
void         OnObjectEnteredTrigger(t_object *trigger, t_object *entered)
{

}
void         OnObjectExitedTrigger(t_object *trigger, t_object *exited)
{

}
