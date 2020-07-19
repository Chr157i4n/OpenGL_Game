#version 330

layout(location = 0) out vec4 f_color;

in vec2 v_tex_coord;

uniform sampler2D u_texture;
uniform vec4 u_color;

void main()
{
    f_color = vec4(texture2D(u_texture, v_tex_coord).a) * u_color;
}