#version 320 es

layout(triangles) in;
layout(triangle_strip, max_vertices = 32) out;

uniform mat4 zMVP;
const float gap = 0.2;

in vec2 uv[];
out vec2 uv_in_frag;
out float brightness;

float
rand(vec2 p)
{
  return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void
EmitVertexOfIndex(int i, vec3 mg)
{
  vec4 position = vec4(mg, 1.0) * gap + gl_in[i].gl_Position * (1.0 - gap);
  float r = rand(mg.xy / mg.z);
  float scale = 0.5 + r * 0.5;
  gl_Position = zMVP * vec4(position.xyz * scale, 1.0);
  uv_in_frag = uv[i];
  EmitVertex();
}

void
main()
{
  vec3 mg = vec3(0.0);
  vec2 uv_mg = vec2(0.0);
  for (int i = 1; i < 3; i++) {
    mg += gl_in[i].gl_Position.xyz;
    uv_mg = uv[i];
  }

  mg /= 2.0;
  uv_mg /= 2.0;

  for (int i = 0; i < 3; i++) {
    brightness = 1.0;
    EmitVertexOfIndex(i, mg);
  }
  EndPrimitive();

  for (int i = 0; i < 3; i++) {
    brightness = 0.4;
    EmitVertexOfIndex(i, mg);

    brightness = 0.4;
    EmitVertexOfIndex((i + 1) % 3, mg);

    brightness = 0.4;
    vec4 p3 = vec4(0.0, 0.0, 0.0, 1.0);
    gl_Position = zMVP * p3;
    uv_in_frag = uv_mg;
    EmitVertex();
    EndPrimitive();
  }
}
