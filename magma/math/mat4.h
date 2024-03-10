#pragma once

#include "base.h"

#include "vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
	Matrix 4×4 representation:
	0/m11 4/m12  8/m13 12/m14
	1/m21 5/m22  9/m23 13/m24
	2/m31 6/m32 10/m33 14/m34
	3/m41 7/m42 11/m43 15/m44
*/
typedef union mg_mat4
{
	struct
	{
		float m11;
		float m21;
		float m31;
		float m41;
		float m12;
		float m22;
		float m32;
		float m42;
		float m13;
		float m23;
		float m33;
		float m43;
		float m14;
		float m24;
		float m34;
		float m44;
	};
	float v[16];
}
mg_mat4_t;

MG_API mg_mat4_t mg_mat4_identity		(void);
MG_API mg_mat4_t mg_mat4_ortho			(float bottom, float top, float left, float right, float near, float far);
MG_API mg_mat4_t mg_mat4_perspective	(float fov, float aspect, float near, float far);

MG_API mg_mat4_t mg_mat4_look_at		(mg_vec3_t eye, mg_vec3_t center, mg_vec3_t up);

MG_API mg_mat4_t mg_mat4_add		(mg_mat4_t first, mg_mat4_t second);
MG_API mg_mat4_t mg_mat4_multiply	(mg_mat4_t first, mg_mat4_t second);

MG_API mg_mat4_t mg_mat4_translate	(mg_mat4_t matrix, mg_vec3_t v);
MG_API mg_mat4_t mg_mat4_scale		(mg_mat4_t matrix, mg_vec3_t v);

MG_API mg_mat4_t mg_mat4_rotate_x	(mg_mat4_t matrix, float angle);
MG_API mg_mat4_t mg_mat4_rotate_y	(mg_mat4_t matrix, float angle);
MG_API mg_mat4_t mg_mat4_rotate_z	(mg_mat4_t matrix, float angle);

MG_API mg_mat4_t mg_mat4_rotate		(mg_mat4_t matrix, float angle, mg_vec3_t axis);

#ifdef __cplusplus
}
#endif