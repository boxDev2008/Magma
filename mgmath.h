#pragma once

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { float x, y; }       mg_vec2;
typedef struct { float x, y, z; }    mg_vec3;
typedef struct { float x, y, z, w; } mg_vec4;
typedef struct { float m[16]; }      mg_mat4;

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

static inline mg_vec2 mg_vec2_add(mg_vec2 a, mg_vec2 b) { return (mg_vec2){a.x+b.x, a.y+b.y}; }
static inline mg_vec2 mg_vec2_sub(mg_vec2 a, mg_vec2 b) { return (mg_vec2){a.x-b.x, a.y-b.y}; }
static inline mg_vec2 mg_vec2_mul(mg_vec2 a, mg_vec2 b) { return (mg_vec2){a.x*b.x, a.y*b.y}; }
static inline mg_vec2 mg_vec2_scale(mg_vec2 v, float s) { return (mg_vec2){v.x*s, v.y*s}; }
static inline mg_vec2 mg_vec2_neg(mg_vec2 v) { return (mg_vec2){-v.x, -v.y}; }
static inline mg_vec2 mg_vec2_perp(mg_vec2 v) { return (mg_vec2){-v.y, v.x}; }
static inline float mg_vec2_dot(mg_vec2 a, mg_vec2 b) { return a.x*b.x + a.y*b.y; }
static inline float mg_vec2_cross(mg_vec2 a, mg_vec2 b) { return a.x*b.y - a.y*b.x; }
static inline float mg_vec2_len2(mg_vec2 v) { return v.x*v.x + v.y*v.y; }
static inline float mg_vec2_len(mg_vec2 v) { return sqrtf(mg_vec2_len2(v)); }

static inline mg_vec2 mg_vec2_norm(mg_vec2 v)
{
    float l = mg_vec2_len(v);
    return l ? mg_vec2_scale(v, 1.0f/l) : (mg_vec2){0, 0};
}

static inline mg_vec2 mg_vec2_lerp(mg_vec2 a, mg_vec2 b, float t)
{
    return (mg_vec2){a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t};
}

static inline mg_vec3 mg_vec3_add(mg_vec3 a, mg_vec3 b) { return (mg_vec3){a.x+b.x, a.y+b.y, a.z+b.z}; }
static inline mg_vec3 mg_vec3_sub(mg_vec3 a, mg_vec3 b) { return (mg_vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
static inline mg_vec3 mg_vec3_mul(mg_vec3 a, mg_vec3 b) { return (mg_vec3){a.x*b.x, a.y*b.y, a.z*b.z}; }
static inline mg_vec3 mg_vec3_scale(mg_vec3 v, float s) { return (mg_vec3){v.x*s, v.y*s, v.z*s}; }
static inline mg_vec3 mg_vec3_neg(mg_vec3 v) { return (mg_vec3){-v.x, -v.y, -v.z}; }
static inline float mg_vec3_dot(mg_vec3 a, mg_vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline float mg_vec3_len2(mg_vec3 v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
static inline float mg_vec3_len(mg_vec3 v) { return sqrtf(mg_vec3_len2(v)); }

static inline mg_vec3 mg_vec3_cross(mg_vec3 a, mg_vec3 b)
{
    return (mg_vec3){a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
}

static inline mg_vec3 mg_vec3_norm(mg_vec3 v)
{
    float l = mg_vec3_len(v);
    return l ? mg_vec3_scale(v, 1.0f/l) : (mg_vec3){0, 0, 0};
}

static inline mg_vec3 mg_vec3_lerp(mg_vec3 a, mg_vec3 b, float t)
{
    return (mg_vec3){a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t, a.z+(b.z-a.z)*t};
}

static inline mg_vec3 mg_vec3_reflect(mg_vec3 v, mg_vec3 n)
{
    return mg_vec3_sub(v, mg_vec3_scale(n, 2.0f*mg_vec3_dot(v, n)));
}

static inline mg_vec4 mg_vec4_add(mg_vec4 a, mg_vec4 b) { return (mg_vec4){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }
static inline mg_vec4 mg_vec4_sub(mg_vec4 a, mg_vec4 b) { return (mg_vec4){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }
static inline mg_vec4 mg_vec4_mul(mg_vec4 a, mg_vec4 b) { return (mg_vec4){a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w}; }
static inline mg_vec4 mg_vec4_scale(mg_vec4 v, float s) { return (mg_vec4){v.x*s, v.y*s, v.z*s, v.w*s}; }
static inline mg_vec4 mg_vec4_neg(mg_vec4 v) { return (mg_vec4){-v.x, -v.y, -v.z, -v.w}; }
static inline float mg_vec4_dot(mg_vec4 a, mg_vec4 b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
static inline float mg_vec4_len2(mg_vec4 v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }
static inline float mg_vec4_len(mg_vec4 v) { return sqrtf(mg_vec4_len2(v)); }

static inline mg_vec4 mg_vec4_norm(mg_vec4 v)
{
    float l = mg_vec4_len(v);
    return l ? mg_vec4_scale(v, 1.0f/l) : (mg_vec4){0, 0, 0, 0};
}

static inline mg_vec4 mg_vec4_lerp(mg_vec4 a, mg_vec4 b, float t)
{
    return (mg_vec4){a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t, a.z+(b.z-a.z)*t, a.w+(b.w-a.w)*t};
}

static inline mg_vec4 mg_quat_identity(void) { return (mg_vec4){0, 0, 0, 1}; }
static inline mg_vec4 mg_quat_conj(mg_vec4 q) { return (mg_vec4){-q.x, -q.y, -q.z, q.w}; }
static inline mg_vec4 mg_quat_norm(mg_vec4 q) { return mg_vec4_norm(q); }

static inline mg_vec4 mg_quat_mul(mg_vec4 a, mg_vec4 b)
{
    return (mg_vec4){
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
    };
}

static inline mg_vec4 mg_quat_from_axis_angle(mg_vec3 axis, float angle)
{
    float s = sinf(angle * 0.5f);
    return (mg_vec4){axis.x*s, axis.y*s, axis.z*s, cosf(angle * 0.5f)};
}

static inline mg_vec4 mg_quat_from_euler(float pitch, float yaw, float roll)
{
    float cp = cosf(pitch*0.5f), sp = sinf(pitch*0.5f);
    float cy = cosf(yaw*0.5f),   sy = sinf(yaw*0.5f);
    float cr = cosf(roll*0.5f),  sr = sinf(roll*0.5f);
    return (mg_vec4){
        sr*cp*cy - cr*sp*sy,
        cr*sp*cy + sr*cp*sy,
        cr*cp*sy - sr*sp*cy,
        cr*cp*cy + sr*sp*sy
    };
}

static inline mg_vec3 mg_quat_rotate_vec3(mg_vec4 q, mg_vec3 v)
{
    mg_vec3 qv = {q.x, q.y, q.z};
    mg_vec3 t  = mg_vec3_scale(mg_vec3_cross(qv, v), 2.0f);
    return mg_vec3_add(mg_vec3_add(v, mg_vec3_scale(t, q.w)), mg_vec3_cross(qv, t));
}

static inline mg_vec4 mg_quat_slerp(mg_vec4 a, mg_vec4 b, float t)
{
    float dot = mg_vec4_dot(a, b);
    if (dot < 0.0f) { b = mg_vec4_neg(b); dot = -dot; }
    if (dot > 0.9995f) return mg_vec4_norm(mg_vec4_lerp(a, b, t));
    float theta0 = acosf(dot);
    float theta = theta0 * t;
    float sin_theta = sinf(theta);
    float sin_theta0 = sinf(theta0);
    float s0 = cosf(theta) - dot * sin_theta / sin_theta0;
    float s1 = sin_theta / sin_theta0;
    return mg_vec4_add(mg_vec4_scale(a, s0), mg_vec4_scale(b, s1));
}

static inline mg_mat4 mg_mat4_identity(void)
{
    mg_mat4 m = { 0 };
    m.m[0] = m.m[5] = m.m[10] = m.m[15] = 1.0f;
    return m;
}

static inline mg_mat4 mg_mat4_mul(mg_mat4 a, mg_mat4 b)
{
    mg_mat4 r = { 0 };
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                r.m[i*4+j] += a.m[i*4+k] * b.m[k*4+j];
    return r;
}

static inline mg_vec4 mg_mat4_mul_vec4(mg_mat4 m, mg_vec4 v)
{
    return (mg_vec4){
        m.m[0]*v.x + m.m[1]*v.y + m.m[2]*v.z + m.m[3]*v.w,
        m.m[4]*v.x + m.m[5]*v.y + m.m[6]*v.z + m.m[7]*v.w,
        m.m[8]*v.x + m.m[9]*v.y + m.m[10]*v.z + m.m[11]*v.w,
        m.m[12]*v.x + m.m[13]*v.y + m.m[14]*v.z + m.m[15]*v.w
    };
}

static inline mg_vec3 mg_mat4_mul_vec3(mg_mat4 m, mg_vec3 v, float w)
{
    mg_vec4 r = mg_mat4_mul_vec4(m, (mg_vec4){v.x, v.y, v.z, w});
    return (mg_vec3){r.x, r.y, r.z};
}

static inline mg_mat4 mg_mat4_transpose(mg_mat4 m)
{
    mg_mat4 r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            r.m[i*4+j] = m.m[j*4+i];
    return r;
}

static inline mg_mat4 mg_mat4_translate(mg_vec3 t)
{
    mg_mat4 m  = mg_mat4_identity();
    m.m[3]     = t.x;
    m.m[7]     = t.y;
    m.m[11]    = t.z;
    return m;
}

static inline mg_mat4 mg_mat4_scale(mg_vec3 s)
{
    mg_mat4 m  = mg_mat4_identity();
    m.m[0]     = s.x;
    m.m[5]     = s.y;
    m.m[10]    = s.z;
    return m;
}

static inline mg_mat4 mg_mat4_rotate_x(float a)
{
    mg_mat4 m  = mg_mat4_identity();
    m.m[5]     =  cosf(a);
    m.m[6]     = -sinf(a);
    m.m[9]     =  sinf(a);
    m.m[10]    =  cosf(a);
    return m;
}

static inline mg_mat4 mg_mat4_rotate_y(float a)
{
    mg_mat4 m  = mg_mat4_identity();
    m.m[0]     =  cosf(a);
    m.m[2]     =  sinf(a);
    m.m[8]     = -sinf(a);
    m.m[10]    =  cosf(a);
    return m;
}

static inline mg_mat4 mg_mat4_rotate_z(float a)
{
    mg_mat4 m  = mg_mat4_identity();
    m.m[0]     =  cosf(a);
    m.m[1]     = -sinf(a);
    m.m[4]     =  sinf(a);
    m.m[5]     =  cosf(a);
    return m;
}

static inline mg_mat4 mg_mat4_from_quat(mg_vec4 q)
{
    mg_mat4 m  = mg_mat4_identity();
    float xx   = q.x*q.x, yy = q.y*q.y, zz = q.z*q.z;
    float xy   = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
    float wx   = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;
    m.m[0]     = 1-2*(yy+zz); m.m[1]  = 2*(xy-wz);   m.m[2]  = 2*(xz+wy);
    m.m[4]     = 2*(xy+wz);   m.m[5]  = 1-2*(xx+zz); m.m[6]  = 2*(yz-wx);
    m.m[8]     = 2*(xz-wy);   m.m[9]  = 2*(yz+wx);   m.m[10] = 1-2*(xx+yy);
    return m;
}

static inline mg_mat4 mg_mat4_look_at(mg_vec3 eye, mg_vec3 center, mg_vec3 up)
{
    mg_vec3 f  = mg_vec3_norm(mg_vec3_sub(center, eye));
    mg_vec3 r  = mg_vec3_norm(mg_vec3_cross(f, up));
    mg_vec3 u  = mg_vec3_cross(r, f);
    mg_mat4 m  = mg_mat4_identity();
    m.m[0]     =  r.x; m.m[1]  =  r.y; m.m[2]  =  r.z; m.m[3]  = -mg_vec3_dot(r, eye);
    m.m[4]     =  u.x; m.m[5]  =  u.y; m.m[6]  =  u.z; m.m[7]  = -mg_vec3_dot(u, eye);
    m.m[8]     = -f.x; m.m[9]  = -f.y; m.m[10] = -f.z; m.m[11] =  mg_vec3_dot(f, eye);
    return m;
}

static inline mg_mat4 mg_mat4_perspective(float fov_y, float aspect, float z_near, float z_far)
{
    float t    = 1.0f / tanf(fov_y * 0.5f);
    mg_mat4 m = { 0 };
    m.m[0] = t / aspect;
    m.m[5] = t;
    m.m[10] = -(z_far + z_near) / (z_far - z_near);
    m.m[11] = -(2.0f * z_far * z_near) / (z_far - z_near);
    m.m[14] = -1.0f;
    return m;
}

static inline mg_mat4 mg_mat4_ortho(float left, float right, float bottom, float top, float z_near, float z_far)
{
    mg_mat4 m = { 0 };
    m.m[0] =  2.0f / (right - left);
    m.m[5] =  2.0f / (top - bottom);
    m.m[10] = -2.0f / (z_far - z_near);
    m.m[3] = -(right + left) / (right - left);
    m.m[7] = -(top + bottom) / (top - bottom);
    m.m[11] = -(z_far + z_near) / (z_far - z_near);
    m.m[15] =  1.0f;
    return m;
}

static inline mg_mat4 mg_mat4_inverse(mg_mat4 m)
{
    float a = m.m[0],  b = m.m[1],  c = m.m[2],  d = m.m[3];
    float e = m.m[4],  f = m.m[5],  g = m.m[6],  h = m.m[7];
    float i = m.m[8],  j = m.m[9],  k = m.m[10], l = m.m[11];
    float p = m.m[12], q = m.m[13], r = m.m[14], s = m.m[15];

    float t[6];
    mg_mat4 o;
    float det;

    t[0] = k*s - r*l; t[1] = j*s - q*l; t[2] = j*r - q*k;
    t[3] = i*s - p*l; t[4] = i*r - p*k; t[5] = i*q - p*j;

    o.m[0]  =  (f*t[0] - g*t[1] + h*t[2]);
    o.m[4]  = -(e*t[0] - g*t[3] + h*t[4]);
    o.m[8]  =  (e*t[1] - f*t[3] + h*t[5]);
    o.m[12] = -(e*t[2] - f*t[4] + g*t[5]);

    o.m[1]  = -(b*t[0] - c*t[1] + d*t[2]);
    o.m[5]  =  (a*t[0] - c*t[3] + d*t[4]);
    o.m[9]  = -(a*t[1] - b*t[3] + d*t[5]);
    o.m[13] =  (a*t[2] - b*t[4] + c*t[5]);

    t[0] = g*s - r*h; t[1] = f*s - q*h; t[2] = f*r - q*g;
    t[3] = e*s - p*h; t[4] = e*r - p*g; t[5] = e*q - p*f;

    o.m[2]  =  (b*t[0] - c*t[1] + d*t[2]);
    o.m[6]  = -(a*t[0] - c*t[3] + d*t[4]);
    o.m[10] =  (a*t[1] - b*t[3] + d*t[5]);
    o.m[14] = -(a*t[2] - b*t[4] + c*t[5]);

    t[0] = g*l - k*h; t[1] = f*l - j*h; t[2] = f*k - j*g;
    t[3] = e*l - i*h; t[4] = e*k - i*g; t[5] = e*j - i*f;

    o.m[3]  = -(b*t[0] - c*t[1] + d*t[2]);
    o.m[7]  =  (a*t[0] - c*t[3] + d*t[4]);
    o.m[11] = -(a*t[1] - b*t[3] + d*t[5]);
    o.m[15] =  (a*t[2] - b*t[4] + c*t[5]);

    det = 1.0f / (a*o.m[0] + b*o.m[4] + c*o.m[8] + d*o.m[12]);
    for (int x = 0; x < 16; x++) o.m[x] *= det;

    return o;
}

static inline mg_mat4 mg_mat4_trs(mg_vec3 t, mg_vec4 q, mg_vec3 s)
{
    return mg_mat4_mul(mg_mat4_mul(mg_mat4_translate(t), mg_mat4_from_quat(q)), mg_mat4_scale(s));
}

#ifdef __cplusplus
}
#endif