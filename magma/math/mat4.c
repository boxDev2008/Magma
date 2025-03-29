#include "mat4.h"

#include <math.h>

mg_mat4 mg_mat4_identity(void)
{
	mg_mat4 result;

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

mg_mat4 mg_mat4_ortho(float bottom, float top, float left, float right, float near, float far)
{
	mg_mat4 result = mg_mat4_identity();

    result.m11 = 2.0f / (right - left); 
    result.m22 = -2.0f / (top - bottom);
    result.m33 = -2.0f / (far - near);
    result.m14 = -(right + left) / (right - left); 
    result.m24 = -(top + bottom) / (top - bottom); 
    result.m34 = -(far + near) / (far - near); 

	return result;
}

mg_mat4 mg_mat4_perspective(float fov, float aspect, float near, float far)
{
    mg_mat4 result = mg_mat4_identity();
    
    float tan_half_fov = tanf(fov / 2.0f);

    result.m11 = 1.0f / (aspect * tan_half_fov);
    result.m22 = 1.0f / tan_half_fov;
    result.m33 = far / (near - far);
    result.m34 = -(near * far) / (near - far);
    result.m43 = -1.0f;

    return result;
}

mg_mat4 mg_mat4_look_at(mg_vec3 eye, mg_vec3 center, mg_vec3 up)
{
    mg_vec3 f = {center.x - eye.x, center.y - eye.y, center.z - eye.z};
    mg_vec3 r = {up.y * f.z - up.z * f.y, up.z * f.x - up.x * f.z, up.x * f.y - up.y * f.x};
    mg_vec3 u = {f.y * r.z - f.z * r.y, f.z * r.x - f.x * r.z, f.x * r.y - f.y * r.x};

    float length_f = sqrtf(f.x * f.x + f.y * f.y + f.z * f.z);
    float length_r = sqrtf(r.x * r.x + r.y * r.y + r.z * r.z);
    float length_u = sqrtf(u.x * u.x + u.y * u.y + u.z * u.z);

    f.x /= length_f;
    f.y /= length_f;
    f.z /= length_f;

    r.x /= length_r;
    r.y /= length_r;
    r.z /= length_r;

    u.x /= length_u;
    u.y /= length_u;
    u.z /= length_u;

    mg_mat4 view_matrix = mg_mat4_identity();

    view_matrix.m11 = r.x;
    view_matrix.m21 = r.y;
    view_matrix.m31 = r.z;
    view_matrix.m41 = -mg_vec3_dot(r, eye);

    view_matrix.m12 = u.x;
    view_matrix.m22 = u.y;
    view_matrix.m32 = u.z;
    view_matrix.m42 = -mg_vec3_dot(u, eye);

    view_matrix.m13 = -f.x;
    view_matrix.m23 = -f.y;
    view_matrix.m33 = -f.z;
    view_matrix.m43 = mg_vec3_dot(f, eye);

    return view_matrix;
}

mg_mat4 mg_mat4_add(mg_mat4 first, mg_mat4 second)
{
	mg_mat4 result;

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

mg_mat4 mg_mat4_multiply(mg_mat4 first, mg_mat4 second)
{
	mg_mat4 result;

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

mg_mat4 mg_mat4_translate(mg_mat4 matrix, mg_vec3 v)
{
	mg_mat4 translation = mg_mat4_identity();
	translation.m14 = v.x;
	translation.m24 = v.y;
	translation.m34 = v.z;

	return mg_mat4_multiply(matrix, translation);
}

mg_mat4 mg_mat4_scale(mg_mat4 matrix, mg_vec3 v)
{
	mg_mat4 scale = mg_mat4_identity();
	scale.m11 = v.x;
	scale.m22 = v.y;
	scale.m33 = v.z;

	return mg_mat4_multiply(matrix, scale);
}

mg_mat4 mg_mat4_rotate_x(mg_mat4 matrix, float angle)
{
    const float cos0 = cosf(angle);
    const float sin0 = sinf(angle);

    mg_mat4 x = mg_mat4_identity();

    x.m22 = cos0;
    x.m23 = -sin0;
    x.m32 = sin0;
    x.m33 = cos0;

    return mg_mat4_multiply(matrix, x);
}

mg_mat4 mg_mat4_rotate_y(mg_mat4 matrix, float angle)
{
    const float cos0 = cosf(angle);
    const float sin0 = sinf(angle);

    mg_mat4 y = mg_mat4_identity();

    y.m11 = cos0;
    y.m13 = sin0;
    y.m31 = -sin0;
    y.m33 = cos0;

    return mg_mat4_multiply(matrix, y);
}

mg_mat4 mg_mat4_rotate_z(mg_mat4 matrix, float angle)
{
	const float cos0 = cosf(angle);
	const float sin0 = sinf(angle);

	mg_mat4 z = mg_mat4_identity();

	z.m11 = cos0;
	z.m12 = -sin0;
	z.m21 = sin0;
	z.m22 = cos0;

	return mg_mat4_multiply(matrix, z);
}

mg_mat4 mg_mat4_rotate(mg_mat4 matrix, float angle, mg_vec3 axis)
{
    mg_mat4 rotation = mg_mat4_identity();

    const float cos_theta = cosf(angle);
    const float sin_theta = sinf(angle);
    const float one_minus_cos_theta = 1.0f - cos_theta;

    rotation.m11 = cos_theta + axis.x * axis.x * one_minus_cos_theta;
    rotation.m12 = axis.x * axis.y * one_minus_cos_theta - axis.z * sin_theta;
    rotation.m13 = axis.x * axis.z * one_minus_cos_theta + axis.y * sin_theta;

    rotation.m21 = axis.y * axis.x * one_minus_cos_theta + axis.z * sin_theta;
    rotation.m22 = cos_theta + axis.y * axis.y * one_minus_cos_theta;
    rotation.m23 = axis.y * axis.z * one_minus_cos_theta - axis.x * sin_theta;

    rotation.m31 = axis.z * axis.x * one_minus_cos_theta - axis.y * sin_theta;
    rotation.m32 = axis.z * axis.y * one_minus_cos_theta + axis.x * sin_theta;
    rotation.m33 = cos_theta + axis.z * axis.z * one_minus_cos_theta;

    return mg_mat4_multiply(matrix, rotation);
}

mg_vec4 mg_mat4_multiply_vec4(mg_mat4 mat, mg_vec4 vec)
{
    mg_vec4 result;

    result.x = mat.m11 * vec.x + mat.m12 * vec.y + mat.m13 * vec.z + mat.m14 * vec.w;
    result.y = mat.m21 * vec.x + mat.m22 * vec.y + mat.m23 * vec.z + mat.m24 * vec.w;
    result.z = mat.m31 * vec.x + mat.m32 * vec.y + mat.m33 * vec.z + mat.m34 * vec.w;
    result.w = mat.m41 * vec.x + mat.m42 * vec.y + mat.m43 * vec.z + mat.m44 * vec.w;

    return result;
}