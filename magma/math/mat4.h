#pragma once

#include "defines.h"

#include "vec3.h"

/*
	Matrix 4×4 representation:
	0/m11 4/m12  8/m13 12/m14
	1/m21 5/m22  9/m23 13/m24
	2/m31 6/m32 10/m33 14/m34
	3/m41 7/m42 11/m43 15/m44
*/
typedef union mat4
{
	struct
	{
		f32 m11;
		f32 m21;
		f32 m31;
		f32 m41;
		f32 m12;
		f32 m22;
		f32 m32;
		f32 m42;
		f32 m13;
		f32 m23;
		f32 m33;
		f32 m43;
		f32 m14;
		f32 m24;
		f32 m34;
		f32 m44;
	};
	f32 v[16];
}
mat4;

MG_API mat4 mg_mat4_identity	(void);
// MG_API mat4 mg_mat4_ortho		(f32 bottom, f32 top, f32 left, f32 right, f32 near, f32 far); Not working at the moment

MG_API mat4 mg_mat4_add		(mat4 first, mat4 second);
MG_API mat4 mg_mat4_multiply	(mat4 first, mat4 second);

MG_API mat4 mg_mat4_translate	(mat4 matrix, vec3 v);
MG_API mat4 mg_mat4_scale		(mat4 matrix, vec3 v);

MG_API mat4 mg_mat4_rotate_z(mat4 matrix, float angle);

MG_API mat4 mg_mat4_rotate		(mat4 matrix, vec3 axis, float angle);