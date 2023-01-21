#version 320 es
precision highp float;

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;

const float Ka = 0.05;
const float Kd = 0.95;

#define M_PI 3.1415926535897932384626433832795

uniform mat4 zView;
uniform mat4 zModel;
uniform mat4 local_model;
uniform float theta;

in vec2 uv;
in vec3 normal;
in vec4 position;
in float azimuthal;
in float polar;

out vec4 output_color;

float
get_height_of(float azi, float pol)
{
  float u = azi / (2.0 * M_PI);
  float v = (pol + M_PI / 2.0) / M_PI;
  vec2 uv = vec2(u, v);
  float height = texture(texture1, uv).r;
  return height * 2.0;
}

vec3
rotate(vec3 p, float angle, vec3 axis)
{
  vec3 a = normalize(axis);
  float s = sin(angle);
  float c = cos(angle);
  float r = 1.0 - c;
  mat3 m =
      mat3(a.x * a.x * r + c, a.y * a.x * r + a.z * s, a.z * a.x * r - a.y * s,
          a.x * a.y * r - a.z * s, a.y * a.y * r + c, a.z * a.y * r + a.x * s,
          a.x * a.z * r + a.y * s, a.y * a.z * r - a.x * s, a.z * a.z * r + c);
  return m * p;
}

vec3
calc_normal()
{
  const float d_theta = M_PI / 500.0;
  const float radius = 1737.4 * 2.0;  // half kilo meters
  const float d_l = radius * M_PI / 500.0;

  float dh_dx = get_height_of(azimuthal + d_theta, polar) -
                get_height_of(azimuthal - d_theta, polar);

  float dh_dy = get_height_of(azimuthal, polar + d_theta) -
                get_height_of(azimuthal, polar - d_theta);

  // facing +x
  vec3 normal = vec3(2.0 * d_l, -dh_dy, dh_dx);
  normal = normalize(normal);  // surface local

  normal = rotate(normal, polar, vec3(0.0, 0.0, 1.0));
  normal = rotate(normal, azimuthal, vec3(0.0, 1.0, 0.0));

  mat4 model = zModel * local_model;
  normal =
      (model * vec4(normal, 1.0)).xyz - (model * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

  normal = normalize(normal);

  return normal;
}

void
main()
{
  vec3 light_position = vec3(10.0 * sin(theta), 1.0, 10.0 * cos(theta));

  // global space

  vec4 camera_position = inverse(zView) * vec4(0.0, 0.0, 0.0, 1.0);

  vec3 view = normalize((camera_position - position).xyz);
  vec3 light = normalize(light_position - position.xyz);
  vec3 half_way = normalize(light + view);
  vec3 norm = calc_normal();

  vec4 tex_color = texture(texture0, uv);

  output_color = tex_color * (max(dot(light, norm), 0.0) + Ka);
}
