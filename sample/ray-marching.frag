#version 320 es
precision highp float;

uniform mat4 zVP;
uniform mat4 zMVP;
uniform mat4 zView;
uniform mat4 zModel;
uniform vec3 half_size;

in vec4 clip_coord;
out vec4 output_color;

const vec3 Ka = vec3(0.8, 0.8, 0.8);
const vec3 Kd = vec3(0.8, 0.8, 0.8);
const vec3 Ks = vec3(0.5, 0.5, 0.5);
const float alpha = 32.0;

#define PI 3.1415925359
#define TWO_PI 6.2831852
#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURFACE_DIST .001

bool
IsIntersectWithBound(
    vec3 ray_origin, vec3 ray_direciton, vec3 half_size, mat4 model)
{
  mat4 model_inv = inverse(model);
  vec3 origin = (model_inv * vec4(ray_origin, 1.0)).xyz;
  vec3 tip = (model_inv * vec4(ray_origin + ray_direciton, 1.0)).xyz;
  vec3 direction = normalize(tip - origin);

  vec3 t_min = (-half_size - origin) / direction;
  vec3 t_max = (half_size - origin) / direction;
  vec3 t1 = min(t_min, t_max);
  vec3 t2 = max(t_min, t_max);
  float t_near = max(max(t1.x, t1.y), t1.z);
  float t_far = min(min(t2.x, t2.y), t2.z);
  return t_near < t_far;
}

float
SmoothMin(float d1, float d2, float k)
{
  float h = exp(-k * d1) + exp(-k * d2);
  return -log(h) / k;
}

float
GetDist(vec3 p)
{
  float max_edge = max(max(half_size.x, half_size.y), half_size.z);
  vec4 sphere1 = vec4(half_size * vec3(0.3, 0.2, 0.1), max_edge * 0.2);
  sphere1.xyz = (zModel * vec4(sphere1.xyz, 1.0)).xyz;
  float d1 = distance(sphere1.xyz, p) - sphere1.w;

  vec4 sphere2 = vec4(half_size * vec3(-0.2, 0.1, 0.13), max_edge * 0.23);
  sphere2.xyz = (zModel * vec4(sphere2.xyz, 1.0)).xyz;
  float d2 = distance(sphere2.xyz, p) - sphere2.w;

  vec4 sphere3 = vec4(half_size * vec3(0.5, 0.6, -0.4), max_edge * 0.32);
  sphere3.xyz = (zModel * vec4(sphere3.xyz, 1.0)).xyz;
  float d3 = distance(sphere3.xyz, p) - sphere3.w;

  return SmoothMin(SmoothMin(d1, d2, 100.0), d3, 100.0);
}

vec3
GetNormal(vec3 p)
{
  float d = 0.001;
  return normalize(
      vec3(GetDist(p + vec3(d, 0.0, 0.0)) - GetDist(p + vec3(-d, 0.0, 0.0)),
          GetDist(p + vec3(0.0, d, 0.0)) - GetDist(p + vec3(0.0, -d, 0.0)),
          GetDist(p + vec3(0.0, 0.0, d)) - GetDist(p + vec3(0.0, 0.0, -d))));
}

bool
RayMarch(vec3 origin, vec3 direction, out vec3 pos)
{
  float len = 0.;
  float dist;
  for (int i = 0; i < MAX_STEPS; i++) {
    vec3 p = origin + direction * len;
    dist = GetDist(p);  // Distance Scene
    len += dist;
    pos = p;
    if (len > MAX_DIST) return false;
    if (dist < SURFACE_DIST) return true;
  }

  return false;
}

void
main()
{
  vec3 local_light = vec3(0, half_size.y * 2.0, 0);
  vec3 origin = (inverse(zView) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
  vec4 inversed = inverse(zVP) * clip_coord;
  vec3 target = inversed.xyz / inversed.w;
  vec3 direction = normalize(target - origin);

  vec3 pos;
  if (!RayMarch(origin, direction, pos)) discard;

  vec3 normal = GetNormal(pos);
  vec3 light = normalize((zModel * vec4(local_light, 1.0)).xyz - pos);
  vec3 view = normalize(origin - pos);
  vec3 half_way = normalize(light + view);

  vec3 ambient = Ka;
  vec3 diffuse = Kd * max(dot(light, normal), 0.0);
  vec3 specular = Ks * pow(max(dot(half_way, normal), 0.0), alpha);

  vec4 clip = zMVP * vec4(pos, 1.0);
  float depth = clip.z / clip.w;

  gl_FragDepth = depth;
  output_color = vec4(ambient + diffuse + specular, 1.0);
}
