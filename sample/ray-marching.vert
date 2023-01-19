#version 320 es

layout(location = 0) in vec2 position;

uniform mat4 zMVP;
uniform vec3 half_size;

out vec4 clip_coord;

float
max_8(float a, float b, float c, float d, float e, float f, float g, float h)
{
  float ab = max(a, b);
  float cd = max(c, d);
  float ef = max(e, f);
  float gh = max(g, f);
  float abcd = max(ab, cd);
  float efgh = max(ef, gh);
  return max(abcd, efgh);
}

float
min_8(float a, float b, float c, float d, float e, float f, float g, float h)
{
  float ab = min(a, b);
  float cd = min(c, d);
  float ef = min(e, f);
  float gh = min(g, f);
  float abcd = min(ab, cd);
  float efgh = min(ef, gh);
  return min(abcd, efgh);
}

void
main()
{
  vec3 A = half_size;
  vec3 B = vec3(half_size.x, -half_size.y, half_size.z);
  vec3 C = vec3(half_size.x, half_size.y, -half_size.z);
  vec3 D = vec3(half_size.x, -half_size.y, -half_size.z);
  vec3 E = -A;
  vec3 F = -B;
  vec3 G = -C;
  vec3 H = -D;

  vec4 a = (zMVP * vec4(A, 1.0));
  vec4 b = (zMVP * vec4(B, 1.0));
  vec4 c = (zMVP * vec4(C, 1.0));
  vec4 d = (zMVP * vec4(D, 1.0));
  vec4 e = (zMVP * vec4(E, 1.0));
  vec4 f = (zMVP * vec4(F, 1.0));
  vec4 g = (zMVP * vec4(G, 1.0));
  vec4 h = (zMVP * vec4(H, 1.0));
  a.x = a.x / a.w;
  b.x = b.x / b.w;
  c.x = c.x / c.w;
  d.x = d.x / d.w;
  e.x = e.x / e.w;
  f.x = f.x / f.w;
  g.x = g.x / g.w;
  h.x = h.x / h.w;
  a.y = a.y / a.w;
  b.y = b.y / b.w;
  c.y = c.y / c.w;
  d.y = d.y / d.w;
  e.y = e.y / e.w;
  f.y = f.y / f.w;
  g.y = g.y / g.w;
  h.y = h.y / h.w;

  float x;
  float y;

  if (position.x == 1.0) {
    x = max_8(a.x, b.x, c.x, d.x, e.x, f.x, g.x, h.x);
  } else {
    x = min_8(a.x, b.x, c.x, d.x, e.x, f.x, g.x, h.x);
  }

  if (position.y == 1.0) {
    y = max_8(a.y, b.y, c.y, d.y, e.y, f.y, g.y, h.y);
  } else {
    y = min_8(a.y, b.y, c.y, d.y, e.y, f.y, g.y, h.y);
  }

  gl_Position = vec4(x, y, 0, 1);
  clip_coord = gl_Position;
}
