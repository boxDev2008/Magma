#include "mat4.h"

#include <math.h>

mg_mat4_t mg_mat4_identity(void)
{
	mg_mat4_t result;

	result.m11 = 1.0f;
	result.m21 = 0.0f;
	result.m31 = 0.0f;
	result.m41 = 0.0f;

	result.m12 = 0.0f;
	result.m22 = 1.0f;
	result.m32 = 0.0f;
	result.m42 = 0.0f;

	result.m13 = 0.0f;
	result.m23 = 0.0f;
	result.m33 = 1.0f;
	result.m43 = 0.0f;

	result.m14 = 0.0f;
	result.m24 = 0.0f;
	result.m34 = 0.0f;
	result.m44 = 1.0f;

	return result;
}

mg_mat4_t mg_mat4_ortho(float bottom, float top, float left, float right, float near, float far)
{
	mg_mat4_t result = mg_mat4_identity();

    result.m11 = 2.0f / (right - left); 
    result.m22 = -2.0f / (top - bottom);
    result.m33 = -2.0f / (far - near);
    result.m14 = -(right + left) / (right - left); 
    result.m24 = -(top + bottom) / (top - bottom); 
    result.m34 = -(far + near) / (far - near); 

	return result;
}

mg_mat4_t mg_mat4_add(mg_mat4_t first, mg_mat4_t second)
{
	mg_mat4_t result;

	result.m11 = first.m11 + second.m11;
	result.m21 = first.m21 + second.m21;
	result.m31 = first.m31 + second.m31;
	result.m41 = first.m41 + second.m41;

	result.m12 = first.m12 + second.m12;
	result.m22 = first.m22 + second.m22;
	result.m32 = first.m32 + second.m32;
	result.m42 = first.m42 + second.m42;

	result.m13 = first.m13 + second.m13;
	result.m23 = first.m23 + second.m23;
	result.m33 = first.m33 + second.m33;
	result.m43 = first.m43 + second.m43;

	result.m14 = first.m14 + second.m14;
	result.m24 = first.m24 + second.m24;
	result.m34 = first.m34 + second.m34;
	result.m44 = first.m44 + second.m44;

	return result;
}

mg_mat4_t mg_mat4_multiply(mg_mat4_t first, mg_mat4_t second)
{
	mg_mat4_t result;

	result.m11 = first.m11 * second.m11 + first.m21 * second.m12 + first.m31 * second.m13 + first.m41 * second.m14;
	result.m21 = first.m11 * second.m21 + first.m21 * second.m22 + first.m31 * second.m23 + first.m41 * second.m24;
	result.m31 = first.m11 * second.m31 + first.m21 * second.m32 + first.m31 * second.m33 + first.m41 * second.m34;
	result.m41 = first.m11 * second.m41 + first.m21 * second.m42 + first.m31 * second.m43 + first.m41 * second.m44;

	result.m12 = first.m12 * second.m11 + first.m22 * second.m12 + first.m32 * second.m13 + first.m42 * second.m14;
	result.m22 = first.m12 * second.m21 + first.m22 * second.m22 + first.m32 * second.m23 + first.m42 * second.m24;
	result.m32 = first.m12 * second.m31 + first.m22 * second.m32 + first.m32 * second.m33 + first.m42 * second.m34;
	result.m42 = first.m12 * second.m41 + first.m22 * second.m42 + first.m32 * second.m43 + first.m42 * second.m44;

	result.m13 = first.m13 * second.m11 + first.m23 * second.m12 + first.m33 * second.m13 + first.m43 * second.m14;
	result.m23 = first.m13 * second.m21 + first.m23 * second.m22 + first.m33 * second.m23 + first.m43 * second.m24;
	result.m33 = first.m13 * second.m31 + first.m23 * second.m32 + first.m33 * second.m33 + first.m43 * second.m34;
	result.m43 = first.m13 * second.m41 + first.m23 * second.m42 + first.m33 * second.m43 + first.m43 * second.m44;

	result.m14 = first.m14 * second.m11 + first.m24 * second.m12 + first.m34 * second.m13 + first.m44 * second.m14;
	result.m24 = first.m14 * second.m21 + first.m24 * second.m22 + first.m34 * second.m23 + first.m44 * second.m24;
	result.m34 = first.m14 * second.m31 + first.m24 * second.m32 + first.m34 * second.m33 + first.m44 * second.m34;
	result.m44 = first.m14 * second.m41 + first.m24 * second.m42 + first.m34 * second.m43 + first.m44 * second.m44;

	return result;
}

mg_mat4_t mg_mat4_translate(mg_mat4_t matrix, mg_vec3_t v)
{
	mg_mat4_t translation = mg_mat4_identity();
	translation.m14 = v.x;
	translation.m24 = v.y;
	translation.m34 = v.z;

	return mg_mat4_multiply(matrix, translation);
}

mg_mat4_t mg_mat4_scale(mg_mat4_t matrix, mg_vec3_t v)
{
	mg_mat4_t scale = mg_mat4_identity();
	scale.m11 = v.x;
	scale.m22 = v.y;
	scale.m33 = v.z;

	return mg_mat4_multiply(matrix, scale);
}

mg_mat4_t mg_mat4_rotate_z(mg_mat4_t matrix, float angle)
{
	const float cos0 = cos(angle);
	const float sin0 = sin(angle);

	mg_mat4_t z = mg_mat4_identity();

	z.m11 = cos0;
	z.m12 = -sin0;
	z.m21 = sin0;
	z.m22 = cos0;

	return mg_mat4_multiply(matrix, z);
}