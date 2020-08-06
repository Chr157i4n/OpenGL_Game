#version 330 core

struct PostProcessingEffect {
    int blood;
    int negative;
    int uncolored;
    int menu;
};


in vec2 v_tex_coords;

uniform PostProcessingEffect u_postprocessingeffect;
uniform sampler2D u_texture;

layout(location = 0) out vec4 f_color;

void main()
{
    vec4 color = texture2D(u_texture, v_tex_coords);
    
    if(u_postprocessingeffect.blood==1)
    {
        color.r += 0.5;
    }
    if(u_postprocessingeffect.negative==1)
    {
        color = 1-color;
    }
    if(u_postprocessingeffect.uncolored==1)
    {
        float average = (color.r + color.g + color.b) /3;
        color = vec4(average, average, average, 1);
    }
    if(u_postprocessingeffect.menu==1)
    {
        color.rgb = color.rgb*0.2;
    }

    f_color = vec4(color.r , color.g , color.b, color.a);
}