# TPhysics
TPhysics is the physic engine of a multiplayer game with a C++ (UE4) client and a Golang server. C was a natural choice to create a physic library which could be used by both the server and the client.

Features :
- Rigid-Bodies or Triggers objects
- AABB, Sphere, Capsule, Cylinder & Convex colliders
- Quadtree-objects for better performances
- Raycasting
- Subworlds

## Subworlds
Subworlds allowed to create, for instance, space ships. In the main world, space ships are simply a moving capsule trigger. As soon as something collided with the trigger, it would enter the subworld, containing all the colliders of the spaceship (eg the spaceship structure, other players, etc).

This way, moving space ships is a lot more stable (since the child colliders are not actually moving along), and AABB can be used for the spaceship structure (as the structure always stay aligned with the subworld)

The downsides of this technique is that it's a bit complex to switch objects between the main world and the subworlds, and that static object in the main world can't have precise collisions with the colliders of a subworld.
