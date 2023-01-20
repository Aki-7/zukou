#version 320 es
precision mediump float;

uniform sampler2D texture0;

in vec2 uv_in_frag;
in float brightness;

out vec4 outputColor;

void
main()
{
  outputColor = texture(texture0, uv_in_frag) * brightness;
}
