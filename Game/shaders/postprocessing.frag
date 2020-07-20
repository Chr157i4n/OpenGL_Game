#version 330 core

in vec2 v_tex_coords;

uniform sampler2D u_texture;

void main()
{
    vec4 color = texture2D(u_texture, v_tex_coords);
    //float average = (color.r + color.g + color.b) / 3.0f;
    //gl_FragColor = vec4(average, average, average, color.a);
    gl_FragColor = vec4(1-color.r , 1-color.g , 1-color.b, color.a);
}