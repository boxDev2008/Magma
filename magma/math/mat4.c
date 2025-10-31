#include "mat4.h"
#include <math.h>

mg_mat4 mg_mat4_identity(void)
{
    mg_mat4 result = {0};

    result.m11 = 1.0f;
    result.m22 = 1.0f;
    result.m33 = 1.0f;
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
    mg_mat4 result = {0};
    float tan_half_fov = tanf(fov / 2.0f);

    result.m11 = 1.0f / (aspect * tan_half_fov);
    result.m22 = 1.0f / tan_half_fov;
    result.m33 = far / (near - far);
    result.m34 = (far * near) / (near - far);
    result.m43 = -1.0f;

    return result;
}

mg_mat4 mg_mat4_look_at(mg_vec3 eye, mg_vec3 center, mg_vec3 up)
{
    mg_vec3 f = {center.x - eye.x, center.y - eye.y, center.z - eye.z};
    float flen = sqrtf(f.x*f.x + f.y*f.y + f.z*f.z);
    f.x /= flen; f.y /= flen; f.z /= flen;

    mg_vec3 r = mg_vec3_cross(f, up);
    float rlen = sqrtf(r.x*r.x + r.y*r.y + r.z*r.z);
    r.x /= rlen; r.y /= rlen; r.z /= rlen;

    mg_vec3 u = mg_vec3_cross(r, f);

    mg_mat4 view = mg_mat4_identity();

    view.m11 = r.x;
    view.m12 = u.x;
    view.m13 = -f.x;

    view.m21 = r.y;
    view.m22 = u.y;
    view.m23 = -f.y;

    view.m31 = r.z;
    view.m32 = u.z;
    view.m33 = -f.z;

    view.m14 = -mg_vec3_dot(r, eye);
    view.m24 = -mg_vec3_dot(u, eye);
    view.m34 = mg_vec3_dot(f, eye);

    return view;
}

mg_mat4 mg_mat4_add(mg_mat4 a, mg_mat4 b)
{
    mg_mat4 r;
    r.m11 = a.m11 + b.m11; r.m12 = a.m12 + b.m12; r.m13 = a.m13 + b.m13; r.m14 = a.m14 + b.m14;
    r.m21 = a.m21 + b.m21; r.m22 = a.m22 + b.m22; r.m23 = a.m23 + b.m23; r.m24 = a.m24 + b.m24;
    r.m31 = a.m31 + b.m31; r.m32 = a.m32 + b.m32; r.m33 = a.m33 + b.m33; r.m34 = a.m34 + b.m34;
    r.m41 = a.m41 + b.m41; r.m42 = a.m42 + b.m42; r.m43 = a.m43 + b.m43; r.m44 = a.m44 + b.m44;
    return r;
}

mg_mat4 mg_mat4_multiply(mg_mat4 a, mg_mat4 b)
{
    mg_mat4 r;

    r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31 + a.m14*b.m41;
    r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32 + a.m14*b.m42;
    r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33 + a.m14*b.m43;
    r.m14 = a.m11*b.m14 + a.m12*b.m24 + a.m13*b.m34 + a.m14*b.m44;

    r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31 + a.m24*b.m41;
    r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32 + a.m24*b.m42;
    r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33 + a.m24*b.m43;
    r.m24 = a.m21*b.m14 + a.m22*b.m24 + a.m23*b.m34 + a.m24*b.m44;

    r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31 + a.m34*b.m41;
    r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32 + a.m34*b.m42;
    r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33 + a.m34*b.m43;
    r.m34 = a.m31*b.m14 + a.m32*b.m24 + a.m33*b.m34 + a.m34*b.m44;

    r.m41 = a.m41*b.m11 + a.m42*b.m21 + a.m43*b.m31 + a.m44*b.m41;
    r.m42 = a.m41*b.m12 + a.m42*b.m22 + a.m43*b.m32 + a.m44*b.m42;
    r.m43 = a.m41*b.m13 + a.m42*b.m23 + a.m43*b.m33 + a.m44*b.m43;
    r.m44 = a.m41*b.m14 + a.m42*b.m24 + a.m43*b.m34 + a.m44*b.m44;

    return r;
}

mg_mat4 mg_mat4_translate(mg_mat4 m, mg_vec3 v)
{
    mg_mat4 t = mg_mat4_identity();
    t.m14 = v.x;
    t.m24 = v.y;
    t.m34 = v.z;
    return mg_mat4_multiply(m, t);
}

mg_mat4 mg_mat4_scale(mg_mat4 m, mg_vec3 v)
{
    mg_mat4 s = mg_mat4_identity();
    s.m11 = v.x;
    s.m22 = v.y;
    s.m33 = v.z;
    return mg_mat4_multiply(m, s);
}

mg_mat4 mg_mat4_rotate_x(mg_mat4 m, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);

    mg_mat4 r = mg_mat4_identity();
    r.m22 = c;
    r.m23 = -s;
    r.m32 = s;
    r.m33 = c;

    return mg_mat4_multiply(m, r);
}

mg_mat4 mg_mat4_rotate_y(mg_mat4 m, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);

    mg_mat4 r = mg_mat4_identity();
    r.m11 = c;
    r.m13 = s;
    r.m31 = -s;
    r.m33 = c;

    return mg_mat4_multiply(m, r);
}

mg_mat4 mg_mat4_rotate_z(mg_mat4 m, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);

    mg_mat4 r = mg_mat4_identity();
    r.m11 = c;
    r.m12 = -s;
    r.m21 = s;
    r.m22 = c;

    return mg_mat4_multiply(m, r);
}

mg_mat4 mg_mat4_rotate(mg_mat4 m, float angle, mg_vec3 axis)
{
    mg_mat4 r = mg_mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);
    float one_minus_c = 1.0f - c;

    r.m11 = c + axis.x * axis.x * one_minus_c;
    r.m12 = axis.x * axis.y * one_minus_c - axis.z * s;
    r.m13 = axis.x * axis.z * one_minus_c + axis.y * s;

    r.m21 = axis.y * axis.x * one_minus_c + axis.z * s;
    r.m22 = c + axis.y * axis.y * one_minus_c;
    r.m23 = axis.y * axis.z * one_minus_c - axis.x * s;

    r.m31 = axis.z * axis.x * one_minus_c - axis.y * s;
    r.m32 = axis.z * axis.y * one_minus_c + axis.x * s;
    r.m33 = c + axis.z * axis.z * one_minus_c;

    return mg_mat4_multiply(m, r);
}

mg_vec4 mg_mat4_multiply_vec4(mg_mat4 mat, mg_vec4 v)
{
    mg_vec4 r;
    r.x = mat.m11*v.x + mat.m12*v.y + mat.m13*v.z + mat.m14*v.w;
    r.y = mat.m21*v.x + mat.m22*v.y + mat.m23*v.z + mat.m24*v.w;
    r.z = mat.m31*v.x + mat.m32*v.y + mat.m33*v.z + mat.m34*v.w;
    r.w = mat.m41*v.x + mat.m42*v.y + mat.m43*v.z + mat.m44*v.w;
    return r;
}
