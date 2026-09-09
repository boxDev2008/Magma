#pragma once

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { float x, y; }       vec2;
typedef struct { float x, y, z; }    vec3;
typedef struct { float x, y, z, w; } vec4;
typedef struct { float m[16]; }      mat4;

#ifndef MG_PI
#define MG_PI 3.14159265358979323846f
#endif

#define MG_DEG2RAD 0.01745329251f
#define MG_RAD2DEG 57.2957795131f
#define MG_CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#define MG_MIN(a, b) ((a) < (b) ? (a) : (b))
#define MG_MAX(a, b) ((a) > (b) ? (a) : (b))
#define MG_LERP(a, b, t) ((a) + ((b) - (a)) * (t))
#define MG_ABS(x) ((x) < 0 ? -(x) : (x))

static inline vec2 vec2_add(vec2 a, vec2 b) { return (vec2){a.x+b.x, a.y+b.y}; }
static inline vec2 vec2_sub(vec2 a, vec2 b) { return (vec2){a.x-b.x, a.y-b.y}; }
static inline vec2 vec2_mul(vec2 a, vec2 b) { return (vec2){a.x*b.x, a.y*b.y}; }
static inline vec2 vec2_scale(vec2 v, float s) { return (vec2){v.x*s, v.y*s}; }
static inline vec2 vec2_neg(vec2 v) { return (vec2){-v.x, -v.y}; }
static inline vec2 vec2_perp(vec2 v) { return (vec2){-v.y, v.x}; }
static inline float vec2_dot(vec2 a, vec2 b) { return a.x*b.x + a.y*b.y; }
static inline float vec2_cross(vec2 a, vec2 b) { return a.x*b.y - a.y*b.x; }
static inline float vec2_len2(vec2 v) { return v.x*v.x + v.y*v.y; }
static inline float vec2_len(vec2 v) { return sqrtf(vec2_len2(v)); }

static inline vec2 vec2_norm(vec2 v)
{
    float l = vec2_len(v);
    return l ? vec2_scale(v, 1.0f/l) : (vec2){0, 0};
}

static inline vec2 vec2_lerp(vec2 a, vec2 b, float t)
{
    return (vec2){a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t};
}

static inline vec3 vec3_add(vec3 a, vec3 b) { return (vec3){a.x+b.x, a.y+b.y, a.z+b.z}; }
static inline vec3 vec3_sub(vec3 a, vec3 b) { return (vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
static inline vec3 vec3_mul(vec3 a, vec3 b) { return (vec3){a.x*b.x, a.y*b.y, a.z*b.z}; }
static inline vec3 vec3_scale(vec3 v, float s) { return (vec3){v.x*s, v.y*s, v.z*s}; }
static inline vec3 vec3_neg(vec3 v) { return (vec3){-v.x, -v.y, -v.z}; }
static inline float vec3_dot(vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline float vec3_len2(vec3 v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
static inline float vec3_len(vec3 v) { return sqrtf(vec3_len2(v)); }

static inline vec3 vec3_cross(vec3 a, vec3 b)
{
    return (vec3){a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
}

static inline vec3 vec3_norm(vec3 v)
{
    float l = vec3_len(v);
    return l ? vec3_scale(v, 1.0f/l) : (vec3){0, 0, 0};
}

static inline vec3 vec3_lerp(vec3 a, vec3 b, float t)
{
    return (vec3){a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t, a.z+(b.z-a.z)*t};
}

static inline vec3 vec3_reflect(vec3 v, vec3 n)
{
    return vec3_sub(v, vec3_scale(n, 2.0f*vec3_dot(v, n)));
}

static inline vec4 vec4_add(vec4 a, vec4 b) { return (vec4){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }
static inline vec4 vec4_sub(vec4 a, vec4 b) { return (vec4){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }
static inline vec4 vec4_mul(vec4 a, vec4 b) { return (vec4){a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w}; }
static inline vec4 vec4_scale(vec4 v, float s) { return (vec4){v.x*s, v.y*s, v.z*s, v.w*s}; }
static inline vec4 vec4_neg(vec4 v) { return (vec4){-v.x, -v.y, -v.z, -v.w}; }
static inline float vec4_dot(vec4 a, vec4 b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
static inline float vec4_len2(vec4 v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }
static inline float vec4_len(vec4 v) { return sqrtf(vec4_len2(v)); }

static inline vec4 vec4_norm(vec4 v)
{
    float l = vec4_len(v);
    return l ? vec4_scale(v, 1.0f/l) : (vec4){0, 0, 0, 0};
}

static inline vec4 vec4_lerp(vec4 a, vec4 b, float t)
{
    return (vec4){a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t, a.z+(b.z-a.z)*t, a.w+(b.w-a.w)*t};
}

static inline vec4 quat_identity(void) { return (vec4){0, 0, 0, 1}; }
static inline vec4 quat_conj(vec4 q) { return (vec4){-q.x, -q.y, -q.z, q.w}; }
static inline vec4 quat_norm(vec4 q) { return vec4_norm(q); }

static inline vec4 quat_mul(vec4 a, vec4 b)
{
    return (vec4){
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
    };
}

static inline vec4 quat_from_axis_angle(vec3 axis, float angle)
{
    float s = sinf(angle * 0.5f);
    return (vec4){axis.x*s, axis.y*s, axis.z*s, cosf(angle * 0.5f)};
}

static inline vec4 quat_from_euler(float pitch, float yaw, float roll)
{
    float cp = cosf(pitch*0.5f), sp = sinf(pitch*0.5f);
    float cy = cosf(yaw*0.5f),   sy = sinf(yaw*0.5f);
    float cr = cosf(roll*0.5f),  sr = sinf(roll*0.5f);
    return (vec4){
        sr*cp*cy - cr*sp*sy,
        cr*sp*cy + sr*cp*sy,
        cr*cp*sy - sr*sp*cy,
        cr*cp*cy + sr*sp*sy
    };
}

static inline vec3 quat_rotate_vec3(vec4 q, vec3 v)
{
    vec3 qv = {q.x, q.y, q.z};
    vec3 t  = vec3_scale(vec3_cross(qv, v), 2.0f);
    return vec3_add(vec3_add(v, vec3_scale(t, q.w)), vec3_cross(qv, t));
}

static inline vec4 quat_slerp(vec4 a, vec4 b, float t)
{
    float dot = vec4_dot(a, b);
    if (dot < 0.0f) { b = vec4_neg(b); dot = -dot; }
    if (dot > 0.9995f) return vec4_norm(vec4_lerp(a, b, t));
    float theta0 = acosf(dot);
    float theta = theta0 * t;
    float sin_theta = sinf(theta);
    float sin_theta0 = sinf(theta0);
    float s0 = cosf(theta) - dot * sin_theta / sin_theta0;
    float s1 = sin_theta / sin_theta0;
    return vec4_add(vec4_scale(a, s0), vec4_scale(b, s1));
}

static inline mat4 mat4_identity(void)
{
    mat4 m = { 0 };
    m.m[0] = m.m[5] = m.m[10] = m.m[15] = 1.0f;
    return m;
}

static inline mat4 mat4_mul(mat4 a, mat4 b)
{
    mat4 r = { 0 };
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            for (int k = 0; k < 4; k++)
                r.m[col*4+row] += a.m[k*4+row] * b.m[col*4+k];
    return r;
}

static inline vec4 mat4_mul_vec4(mat4 m, vec4 v)
{
    return (vec4){
        m.m[0]*v.x + m.m[4]*v.y + m.m[8]*v.z  + m.m[12]*v.w,
        m.m[1]*v.x + m.m[5]*v.y + m.m[9]*v.z  + m.m[13]*v.w,
        m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z + m.m[14]*v.w,
        m.m[3]*v.x + m.m[7]*v.y + m.m[11]*v.z + m.m[15]*v.w
    };
}

static inline vec3 mat4_mul_vec3(mat4 m, vec3 v, float w)
{
    vec4 r = mat4_mul_vec4(m, (vec4){v.x, v.y, v.z, w});
    return (vec3){r.x, r.y, r.z};
}

static inline mat4 mat4_transpose(mat4 m)
{
    mat4 r;
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            r.m[col*4+row] = m.m[row*4+col];
    return r;
}

static inline mat4 mat4_translate(vec3 t)
{
    mat4 m  = mat4_identity();
    m.m[12]    = t.x;
    m.m[13]    = t.y;
    m.m[14]    = t.z;
    return m;
}

static inline mat4 mat4_scale(vec3 s)
{
    mat4 m  = mat4_identity();
    m.m[0]     = s.x;
    m.m[5]     = s.y;
    m.m[10]    = s.z;
    return m;
}

static inline mat4 mat4_rotate_x(float a)
{
    mat4 m  = mat4_identity();
    m.m[5]     =  cosf(a);
    m.m[9]     = -sinf(a);
    m.m[6]     =  sinf(a);
    m.m[10]    =  cosf(a);
    return m;
}

static inline mat4 mat4_rotate_y(float a)
{
    mat4 m  = mat4_identity();
    m.m[0]     =  cosf(a);
    m.m[8]     =  sinf(a);
    m.m[2]     = -sinf(a);
    m.m[10]    =  cosf(a);
    return m;
}

static inline mat4 mat4_rotate_z(float a)
{
    mat4 m  = mat4_identity();
    m.m[0]     =  cosf(a);
    m.m[4]     = -sinf(a);
    m.m[1]     =  sinf(a);
    m.m[5]     =  cosf(a);
    return m;
}

static inline mat4 mat4_from_quat(vec4 q)
{
    mat4 m  = mat4_identity();
    float xx   = q.x*q.x, yy = q.y*q.y, zz = q.z*q.z;
    float xy   = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
    float wx   = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;
    m.m[0]  = 1-2*(yy+zz); m.m[4]  = 2*(xy-wz);   m.m[8]  = 2*(xz+wy);
    m.m[1]  = 2*(xy+wz);   m.m[5]  = 1-2*(xx+zz); m.m[9]  = 2*(yz-wx);
    m.m[2]  = 2*(xz-wy);   m.m[6]  = 2*(yz+wx);   m.m[10] = 1-2*(xx+yy);
    return m;
}

static inline mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up)
{
    vec3 f  = vec3_norm(vec3_sub(center, eye));
    vec3 r  = vec3_norm(vec3_cross(f, up));
    vec3 u  = vec3_cross(r, f);
    mat4 m  = mat4_identity();
    m.m[0] =  r.x; m.m[4] =  r.y; m.m[8]  =  r.z; m.m[12] = -vec3_dot(r, eye);
    m.m[1] =  u.x; m.m[5] =  u.y; m.m[9]  =  u.z; m.m[13] = -vec3_dot(u, eye);
    m.m[2] = -f.x; m.m[6] = -f.y; m.m[10] = -f.z; m.m[14] =  vec3_dot(f, eye);
    return m;
}

static inline mat4 mat4_perspective(float fov_y, float aspect, float z_near, float z_far)
{
    float t    = 1.0f / tanf(fov_y * 0.5f);
    mat4 m = { 0 };
    m.m[0]  = t / aspect;
    m.m[5]  = -t;
    m.m[10] = z_far / (z_near - z_far);
    m.m[14] = (z_near * z_far) / (z_near - z_far);
    m.m[11] = -1.0f;
    return m;
}

static inline mat4 mat4_ortho(float left, float right, float bottom, float top, float z_near, float z_far)
{
    mat4 m = { 0 };
    m.m[0]  =  2.0f / (right - left);
    m.m[5]  = -2.0f / (top - bottom);
    m.m[10] = -1.0f / (z_far - z_near);
    m.m[12] = -(right + left) / (right - left);
    m.m[13] = -(top + bottom) / (top - bottom);
    m.m[14] = -z_near / (z_far - z_near);
    m.m[15] =  1.0f;
    return m;
}

static inline mat4 mat4_inverse(mat4 m)
{
    float a = m.m[0],  e = m.m[4],  i = m.m[8],  p = m.m[12];
    float b = m.m[1],  f = m.m[5],  j = m.m[9],  q = m.m[13];
    float c = m.m[2],  g = m.m[6],  k = m.m[10], r = m.m[14];
    float d = m.m[3],  h = m.m[7],  l = m.m[11], s = m.m[15];

    float t[6];
    mat4 o;
    float det;

    t[0] = k*s - r*l; t[1] = j*s - q*l; t[2] = j*r - q*k;
    t[3] = i*s - p*l; t[4] = i*r - p*k; t[5] = i*q - p*j;

    float c00 =  (f*t[0] - g*t[1] + h*t[2]);
    float c01 = -(e*t[0] - g*t[3] + h*t[4]);
    float c02 =  (e*t[1] - f*t[3] + h*t[5]);
    float c03 = -(e*t[2] - f*t[4] + g*t[5]);

    float c10 = -(b*t[0] - c*t[1] + d*t[2]);
    float c11 =  (a*t[0] - c*t[3] + d*t[4]);
    float c12 = -(a*t[1] - b*t[3] + d*t[5]);
    float c13 =  (a*t[2] - b*t[4] + c*t[5]);

    t[0] = g*s - r*h; t[1] = f*s - q*h; t[2] = f*r - q*g;
    t[3] = e*s - p*h; t[4] = e*r - p*g; t[5] = e*q - p*f;

    float c20 =  (b*t[0] - c*t[1] + d*t[2]);
    float c21 = -(a*t[0] - c*t[3] + d*t[4]);
    float c22 =  (a*t[1] - b*t[3] + d*t[5]);
    float c23 = -(a*t[2] - b*t[4] + c*t[5]);

    t[0] = g*l - k*h; t[1] = f*l - j*h; t[2] = f*k - j*g;
    t[3] = e*l - i*h; t[4] = e*k - i*g; t[5] = e*j - i*f;

    float c30 = -(b*t[0] - c*t[1] + d*t[2]);
    float c31 =  (a*t[0] - c*t[3] + d*t[4]);
    float c32 = -(a*t[1] - b*t[3] + d*t[5]);
    float c33 =  (a*t[2] - b*t[4] + c*t[5]);

    o.m[0] = c00; o.m[4] = c01; o.m[8]  = c02; o.m[12] = c03;
    o.m[1] = c10; o.m[5] = c11; o.m[9]  = c12; o.m[13] = c13;
    o.m[2] = c20; o.m[6] = c21; o.m[10] = c22; o.m[14] = c23;
    o.m[3] = c30; o.m[7] = c31; o.m[11] = c32; o.m[15] = c33;

    det = 1.0f / (a*c00 + b*c01 + c*c02 + d*c03);
    for (int x = 0; x < 16; x++) o.m[x] *= det;

    return o;
}

static inline mat4 mat4_trs(vec3 t, vec4 q, vec3 s)
{
    return mat4_mul(mat4_mul(mat4_translate(t), mat4_from_quat(q)), mat4_scale(s));
}

#ifdef __cplusplus
}
#endif