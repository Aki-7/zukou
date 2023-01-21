#version 320 es
precision highp float;

layout(location = 0) in float azimuthal_in;
layout(location = 1) in float polar_in;

#define M_PI 3.1415926535897932384626433832795

uniform mat4 zMVP;
uniform mat4 zModel;
uniform mat4 local_model;

out vec2 uv;
out vec3 normal;
out vec4 position;
out float azimuthal;
out float polar;

void
main()
{
  mat4 model = zModel * local_model;

  float y = sin(polar_in);
  float l = cos(polar_in);
  float x = l * cos(azimuthal_in);
  float z = l * -sin(azimuthal_in);
  float u = azimuthal_in / (2.0 * M_PI);
  float v = (polar_in + M_PI / 2.0) / M_PI;

  vec4 local_position = vec4(x, y, z, 1.0);

  gl_Position = zMVP * local_model * local_position;
  vec4 zero = vec4(0.0, 0.0, 0.0, 1.0);

  position = (model * local_position);
  normal = position.xyz - (model * zero).xyz;

  azimuthal = azimuthal_in;
  polar = polar_in;

  uv = vec2(u, v);
}
