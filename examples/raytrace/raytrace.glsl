@name raytrace_gi
@stage compute

layout(local_size_x = 8, local_size_y = 8) in;

layout(rgba8,   binding = 0) uniform writeonly image2D outImage;    // final LDR image
layout(rgba32f, binding = 1) uniform readonly  image2D colorPrev;   // last frame: rgb = HDR radiance, a = history length
layout(rgba32f, binding = 2) uniform writeonly image2D colorCur;    // this frame (same layout)
layout(rgba32f, binding = 3) uniform readonly  image2D geoPrev;     // last frame: xyz = primary normal, w = hit distance (-1 = sky)
layout(rgba32f, binding = 4) uniform writeonly image2D geoCur;      // this frame (same layout)

layout(binding = 0) uniform Cam
{
    vec3 origin;
    vec3 forward, right, up;
    float fovScale;
}
cam;

layout(binding = 1) uniform Frame
{
    uint frameIndex;
    uint resetHistory;
    uint cameraMoved;
    uint _pad0;
}
frame;

layout(binding = 2) uniform PrevCam
{
    vec3 origin;
    vec3 forward, right, up;
    float fovScale;
}
prevCam;

const int   SAMPLES_PER_PIXEL = 4;
const int   MAX_BOUNCES       = 5;
const float FIREFLY_CLAMP     = 20.0;
const float EXPOSURE          = 1.0;

const float MAX_HISTORY_STATIC    = 512.0;
const float MAX_HISTORY_MOVING    = 12.0;
const float MIRROR_HISTORY_MOVING = 3.0;
const float MIRROR_THRESHOLD      = 0.5;

struct Sphere
{
    vec3 center; float radius;
    vec3 color;  float reflectivity;
    float emission;
};

const int SPHERE_COUNT = 8;
const int GROUND_INDEX = 1;

const Sphere spheres[SPHERE_COUNT] = Sphere[](
    Sphere(vec3( 0.0,    0.0, -3.0),   1.0,  vec3(0.9, 0.3, 0.3),    0.10, 0.0),
    Sphere(vec3( 0.0, -101.0, -3.0), 100.0,  vec3(0.8, 0.8, 0.8),    0.15, 0.0),
    Sphere(vec3( 2.2,   -0.4, -3.8),   0.6,  vec3(0.95, 0.95, 0.95), 0.90, 0.0),
    Sphere(vec3(-2.2,  -0.25, -2.5),  0.75,  vec3(0.3, 0.4, 0.9),    0.30, 0.0),
    Sphere(vec3( 1.3,   -0.6, -1.8),   0.4,  vec3(1.0, 0.75, 0.3),   0.70, 0.0),
    Sphere(vec3(-1.0,   -0.7, -1.5),   0.3,  vec3(0.3, 0.8, 0.3),    0.00, 0.0),
    Sphere(vec3(-1.5,    0.5, -6.0),   1.5,  vec3(0.9, 0.9, 1.0),    0.80, 0.0),
    Sphere(vec3( 0.5,  -0.65, -1.2),  0.35,  vec3(1.0, 0.45, 0.15),  0.00, 5.0) 
);

const vec3  SUN_DIR      = vec3(0.57735027);
const vec3  SUN_COLOR    = vec3(1.0, 0.95, 0.85);
const float SUN_STRENGTH = 2.2;
const float SUN_ANGLE    = 0.04;
const float SKY_STRENGTH = 1.0;
const float EPS          = 0.002;

uint rngState;

uint hash(uint x)
{
    x ^= x >> 16; x *= 0x7feb352du;
    x ^= x >> 15; x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

uint pcg()
{
    rngState = rngState * 747796405u + 2891336777u;
    uint w = ((rngState >> ((rngState >> 28u) + 4u)) ^ rngState) * 277803737u;
    return (w >> 22u) ^ w;
}

float rand() { return float(pcg() >> 8) * (1.0 / 16777216.0); }

vec3 primaryDir(vec3 f, vec3 r, vec3 u, float fs, vec2 pixelPos, vec2 size)
{
    vec2 uv = pixelPos / size * 2.0 - 1.0;
    uv.x *= size.x / size.y;
    uv.y = -uv.y;
    return normalize(f + (uv.x * r + uv.y * u) * fs);
}

bool reproject(vec3 v, vec2 size, out vec2 pf)
{
    pf = vec2(0.0);
    float z = dot(v, prevCam.forward);
    if (z <= 1e-4) return false;

    vec2 uv = vec2(dot(v, prevCam.right), dot(v, prevCam.up)) / (z * prevCam.fovScale);
    uv.x /= size.x / size.y;
    uv.y = -uv.y;
    pf = (uv * 0.5 + 0.5) * size - 0.5;
    return true;
}

void makeBasis(vec3 n, out vec3 t, out vec3 b)
{
    t = normalize(abs(n.x) > 0.9 ? cross(n, vec3(0, 1, 0)) : cross(n, vec3(1, 0, 0)));
    b = cross(n, t);
}

vec3 cosineHemisphere(vec3 n)
{
    float u1 = rand();
    float u2 = rand();
    float r = sqrt(u1);
    float phi = 6.2831853 * u2;
    vec3 t, b;
    makeBasis(n, t, b);
    return normalize(t * (r * cos(phi)) + b * (r * sin(phi)) + n * sqrt(1.0 - u1));
}

// Uniform direction inside the sun's cone -> soft shadows.
vec3 sampleSun()
{
    float cosT = mix(1.0, cos(SUN_ANGLE), rand());
    float sinT = sqrt(max(0.0, 1.0 - cosT * cosT));
    float phi = 6.2831853 * rand();
    vec3 t, b;
    makeBasis(SUN_DIR, t, b);
    return normalize(t * (sinT * cos(phi)) + b * (sinT * sin(phi)) + SUN_DIR * cosT);
}

float hitSphere(vec3 ro, vec3 rd, Sphere s)
{
    vec3 oc = ro - s.center;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - s.radius * s.radius;
    float disc = b * b - c;
    if (disc < 0.0) return -1.0;
    float t = -b - sqrt(disc);
    return t > 0.001 ? t : -1.0;
}

int intersect(vec3 ro, vec3 rd, out float tOut)
{
    float tMin = 1e30;
    int hit = -1;
    for (int i = 0; i < SPHERE_COUNT; i++)
    {
        float t = hitSphere(ro, rd, spheres[i]);
        if (t > 0.0 && t < tMin) { tMin = t; hit = i; }
    }
    tOut = tMin;
    return hit;
}

bool occluded(vec3 ro, vec3 rd)
{
    for (int i = 0; i < SPHERE_COUNT; i++)
        if (hitSphere(ro, rd, spheres[i]) > 0.0) return true;
    return false;
}

vec3 sky(vec3 rd)
{
    float t = clamp(0.5 * (rd.y + 1.0), 0.0, 1.0);
    return mix(vec3(0.9, 0.95, 1.0), vec3(0.5, 0.7, 1.0), t);
}

vec3 pathTrace(vec3 ro, vec3 rd)
{
    vec3 L = vec3(0.0);
    vec3 T = vec3(1.0);
    bool specularPath = true;

    for (int bounce = 0; bounce < MAX_BOUNCES; bounce++)
    {
        float t;
        int hit = intersect(ro, rd, t);

        if (hit < 0)
        {
            L += T * sky(rd) * SKY_STRENGTH;
            if (specularPath && dot(rd, SUN_DIR) > cos(SUN_ANGLE))
                L += T * SUN_COLOR * 20.0;
            break;
        }

        Sphere s = spheres[hit];
        vec3 p = ro + rd * t;
        vec3 n = normalize(p - s.center);

        vec3 albedo = s.color;
        if (hit == GROUND_INDEX)
        {
            float checker = mod(floor(p.x) + floor(p.z), 2.0);
            albedo *= mix(0.35, 1.0, checker);
        }

        L += T * s.color * s.emission;

        ro = p + n * EPS;

        if (rand() < s.reflectivity)
        {
            rd = reflect(rd, n);
            T *= albedo;
            specularPath = true;
        }
        else
        {
            vec3 ld = sampleSun();
            float ndl = dot(n, ld);
            if (ndl > 0.0 && !occluded(ro, ld))
                L += T * albedo * ndl * SUN_COLOR * SUN_STRENGTH;

            T *= albedo;
            rd = cosineHemisphere(n);
            specularPath = false;
        }

        if (bounce >= 2)
        {
            float q = clamp(max(T.r, max(T.g, T.b)), 0.05, 1.0);
            if (rand() > q) break;
            T /= q;
        }
    }
    return L;
}

bool tapValid(ivec2 q, ivec2 size, bool isSky, vec3 P, vec3 n, float t, out vec4 c)
{
    c = vec4(0.0);
    if (any(lessThan(q, ivec2(0))) || any(greaterThanEqual(q, size))) return false;

    vec4 g = imageLoad(geoPrev, q);
    bool prevSky = g.w < 0.0;
    if (isSky != prevSky) return false;

    if (!isSky)
    {
        vec3 pd = primaryDir(prevCam.forward, prevCam.right, prevCam.up, prevCam.fovScale, vec2(q) + 0.5, vec2(size));
        vec3 Pp = prevCam.origin + pd * g.w;
        if (dot(n, g.xyz) < 0.9) return false;
        if (abs(dot(P - Pp, g.xyz)) > 0.02 * t) return false;
    }

    c = imageLoad(colorPrev, q);
    return true;
}

void temporalResolve(ivec2 px, ivec2 size, bool isSky, vec3 P, vec3 n, float t, vec3 rdCenter, float refl, vec3 cur, out vec3 outColor, out float outLen)
{
    outColor = cur;
    outLen = 1.0;

    if (frame.resetHistory != 0u) return;

    vec3 v = isSky ? rdCenter : (P - prevCam.origin);
    vec2 pf;
    if (!reproject(v, vec2(size), pf)) return;

    ivec2 p0 = ivec2(floor(pf));
    vec2 fr = pf - vec2(p0);

    vec3 sumC = vec3(0.0);
    float sumL = 0.0;
    float sumW = 0.0;
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < 2; i++)
        {
            float w = (i == 0 ? 1.0 - fr.x : fr.x) * (j == 0 ? 1.0 - fr.y : fr.y);
            vec4 c;
            if (w > 0.0 && tapValid(p0 + ivec2(i, j), size, isSky, P, n, t, c))
            {
                sumC += c.rgb * w;
                sumL += c.a * w;
                sumW += w;
            }
        }
    }
    if (sumW < 1e-3) return;   // disocclusion: start fresh

    vec3 hist = sumC / sumW;
    float histLen = sumL / sumW;

    float cap = MAX_HISTORY_STATIC;
    if (frame.cameraMoved != 0u)
        cap = refl >= MIRROR_THRESHOLD ? MIRROR_HISTORY_MOVING : MAX_HISTORY_MOVING;

    outLen = min(histLen + 1.0, cap);
    outColor = mix(hist, cur, 1.0 / outLen);
}

vec3 tonemapACES(vec3 x)
{
    return clamp((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14), 0.0, 1.0);
}

void main()
{
    ivec2 px = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(outImage);
    if (px.x >= size.x || px.y >= size.y) return;
    vec2 sz = vec2(size);

    rngState = hash(uint(px.x) + hash(uint(px.y) + hash(frame.frameIndex)));

    vec3 ro = cam.origin;
    vec3 rdCenter = primaryDir(cam.forward, cam.right, cam.up, cam.fovScale, vec2(px) + 0.5, sz);
    float tC;
    int hitC = intersect(ro, rdCenter, tC);
    bool isSky = hitC < 0;

    vec3 P = vec3(0.0);
    vec3 N = vec3(0.0);
    float refl = 0.0;
    if (!isSky)
    {
        P = ro + rdCenter * tC;
        N = normalize(P - spheres[hitC].center);
        refl = spheres[hitC].reflectivity;
    }

    vec3 sum = vec3(0.0);
    for (int s = 0; s < SAMPLES_PER_PIXEL; s++)
    {
        vec2 jitter = vec2(rand(), rand());
        vec3 rd = primaryDir(cam.forward, cam.right, cam.up, cam.fovScale, vec2(px) + jitter, sz);
        sum += min(pathTrace(ro, rd), vec3(FIREFLY_CLAMP));
    }
    vec3 cur = sum / float(SAMPLES_PER_PIXEL);
    if (any(isnan(cur)) || any(isinf(cur))) cur = vec3(0.0);

    vec3 color;
    float len;
    temporalResolve(px, size, isSky, P, N, tC, rdCenter, refl, cur, color, len);

    imageStore(colorCur, px, vec4(color, len));
    imageStore(geoCur, px, isSky ? vec4(0.0, 0.0, 0.0, -1.0) : vec4(N, tC));

    color = tonemapACES(color * EXPOSURE);
    color = pow(color, vec3(1.0 / 2.2));
    imageStore(outImage, px, vec4(color, 1.0));
}
