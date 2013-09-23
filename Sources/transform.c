#include <math.h>
#include "transform.h"
#include "matrix.h"

// t_vector3		transform_position(t_vector3 pos, t_transform from, t_transform to)
// {
// 	t_vector3	result;
// 	t_transform diff;
//
// 	diff.position = sub_vector3_from_vector3(from.position, to.position);
// 	diff.rotation = sub_vector3_from_vector3(from.rotation, to.rotation);
//
// 	result.x = diff.position.x + (cos(diff.rotation.z) * pos.x + sin(diff.rotation.z) * -pos.y);
// 	result.y = diff.position.y + (sin(diff.rotation.z) * pos.x + cos(diff.rotation.z) * pos.y);
//
// 	return (result);
// }

#define DEG2RAD 0.0174532925

t_vector3		transform_position(t_vector3 pos, t_transform to)
{
	t_vector3	result;
	t_matrix	matrix;

	matrix = init_matrix_translate(-to.position.x, -to.position.y, -to.position.z);
	matrix = mult_matrix(init_matrix_rotate(to.rotation.x * DEG2RAD, -to.rotation.y * DEG2RAD, -to.rotation.z * DEG2RAD), matrix);

	result = mult_matrix_vector3(matrix, pos);

	return (result);
}

t_vector3		transform_position_inv(t_vector3 pos, t_transform from)
{
	t_matrix	matrix;

	matrix = init_matrix_translate(from.position.x, from.position.y, from.position.z);
	matrix = mult_matrix(matrix, init_matrix_rotate_inv(-from.rotation.x * DEG2RAD, from.rotation.y * DEG2RAD, from.rotation.z * DEG2RAD));

	return mult_matrix_vector3(matrix, pos);
}
