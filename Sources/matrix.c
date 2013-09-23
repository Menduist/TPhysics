#include "Matrix.h"
#include "math.h"
#include "vector.h"

t_matrix init_matrix_identity()
{
	t_matrix m;

	m.m[0][0] = 1.f; m.m[0][1] = 0.f; m.m[0][2] = 0.f; m.m[0][3] = 0.f;
	m.m[1][0] = 0.f; m.m[1][1] = 1.f; m.m[1][2] = 0.f; m.m[1][3] = 0.f;
	m.m[2][0] = 0.f; m.m[2][1] = 0.f; m.m[2][2] = 1.f; m.m[2][3] = 0.f;
	m.m[3][0] = 0.f; m.m[3][1] = 0.f; m.m[3][2] = 0.f; m.m[3][3] = 1.f;

	return (m);
}

t_matrix init_matrix_scale(float scalex, float scaley, float scalez)
{
	t_matrix m;
	m.m[0][0] = scalex; m.m[0][1] = 0.f; m.m[0][2] = 0.f; m.m[0][3] = 0.f;
	m.m[1][0] = 0.f; m.m[1][1] = scaley; m.m[1][2] = 0.f; m.m[1][3] = 0.f;
	m.m[2][0] = 0.f; m.m[2][1] = 0.f; m.m[2][2] = scalez; m.m[2][3] = 0.f;
	m.m[3][0] = 0.f; m.m[3][1] = 0.f; m.m[3][2] = 0.f; m.m[3][3] = 1.f;

	return (m);
}

t_matrix init_matrix_rotate(float rotationx, float rotationy, float rotationz)
{
	t_matrix rx, ry, rz;

	rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f; rx.m[0][2] = 0.0f; rx.m[0][3] = 0.0f;
	rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(rotationx); rx.m[1][2] = -sinf(rotationx); rx.m[1][3] = 0.0f;
	rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(rotationx); rx.m[2][2] = cosf(rotationx); rx.m[2][3] = 0.0f;
	rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f; rx.m[3][2] = 0.0f; rx.m[3][3] = 1.0f;

	ry.m[0][0] = cosf(rotationy); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(rotationy); ry.m[0][3] = 0.0f;
	ry.m[1][0] = 0.0f; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f; ry.m[1][3] = 0.0f;
	ry.m[2][0] = sinf(rotationy); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(rotationy); ry.m[2][3] = 0.0f;
	ry.m[3][0] = 0.0f; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f; ry.m[3][3] = 1.0f;

	rz.m[0][0] = cosf(rotationz); rz.m[0][1] = -sinf(rotationz); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
	rz.m[1][0] = sinf(rotationz); rz.m[1][1] = cosf(rotationz); rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
	rz.m[2][0] = 0.0f; rz.m[2][1] = 0.0f; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
	rz.m[3][0] = 0.0f; rz.m[3][1] = 0.0f; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

	return (mult_matrix(mult_matrix(rx, ry), rz));
}

t_matrix init_matrix_rotate_inv(float rotationx, float rotationy, float rotationz)
{
	t_matrix rx, ry, rz;

	rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f; rx.m[0][2] = 0.0f; rx.m[0][3] = 0.0f;
	rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(rotationx); rx.m[1][2] = -sinf(rotationx); rx.m[1][3] = 0.0f;
	rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(rotationx); rx.m[2][2] = cosf(rotationx); rx.m[2][3] = 0.0f;
	rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f; rx.m[3][2] = 0.0f; rx.m[3][3] = 1.0f;

	ry.m[0][0] = cosf(rotationy); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(rotationy); ry.m[0][3] = 0.0f;
	ry.m[1][0] = 0.0f; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f; ry.m[1][3] = 0.0f;
	ry.m[2][0] = sinf(rotationy); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(rotationy); ry.m[2][3] = 0.0f;
	ry.m[3][0] = 0.0f; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f; ry.m[3][3] = 1.0f;

	rz.m[0][0] = cosf(rotationz); rz.m[0][1] = -sinf(rotationz); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
	rz.m[1][0] = sinf(rotationz); rz.m[1][1] = cosf(rotationz); rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
	rz.m[2][0] = 0.0f; rz.m[2][1] = 0.0f; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
	rz.m[3][0] = 0.0f; rz.m[3][1] = 0.0f; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

	return (mult_matrix(rz, mult_matrix(ry, rx)));
}

t_matrix init_matrix_translate(float x, float y, float z)
{
	t_matrix m;

	m.m[0][0] = 1.f; m.m[0][1] = 0.f; m.m[0][2] = 0.f; m.m[0][3] = x;
	m.m[1][0] = 0.f; m.m[1][1] = 1.f; m.m[1][2] = 0.f; m.m[1][3] = y;
	m.m[2][0] = 0.f; m.m[2][1] = 0.f; m.m[2][2] = 1.f; m.m[2][3] = z;
	m.m[3][0] = 0.f; m.m[3][1] = 0.f; m.m[3][2] = 0.f; m.m[3][3] = 1.f;
	return (m);
}

t_matrix mult_matrix(t_matrix left, t_matrix right)
{
	t_matrix result;
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			result.m[i][j] = left.m[i][0] * right.m[0][j] +
					left.m[i][1] * right.m[1][j] +
					left.m[i][2] * right.m[2][j] +
					left.m[i][3] * right.m[3][j];
		}
	}
	return (result);
}

t_vector3 mult_matrix_vector3(t_matrix left, t_vector3 right)
{
	t_vector3 result;

	int i;
	for (i = 0; i < 3; i++)
	{
		result.data[i] = left.m[i][0] * right.x + left.m[i][1] * right.y + left.m[i][2] * right.z + left.m[i][3];
	}
	return (result);
}
