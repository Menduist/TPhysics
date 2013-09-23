#ifndef __MATRIX_H__
#define __MATRIX_H__

typedef struct	s_matrix
{
	float m[4][4];
}				t_matrix;

#include "vector.h"

t_matrix init_matrix_identity();
t_matrix init_matrix_scale(float scalex, float scaley, float scalez);
t_matrix init_matrix_rotate(float rotationx, float rotationy, float rotationz);
t_matrix init_matrix_rotate_inv(float rotationx, float rotationy, float rotationz);
t_matrix init_matrix_translate(float x, float y, float z);

t_matrix mult_matrix(t_matrix left, t_matrix right);
t_vector3 mult_matrix_vector3(t_matrix left, t_vector3 right);

#endif
