#version 330 core

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 color;
};

//Input from VRAM
in vec3 v_position_world_space;
in vec3 v_position_view_space;
in vec2 v_tex_coord;
in mat3 v_tbn;
in vec3 v_normal_world_space;

//Input from CPU
uniform Material u_material;
uniform DirectionalLight u_directional_light;

uniform sampler2D u_diffuse_map;    //textureslot 0
uniform sampler2D u_normal_map;     //textureslot 1
uniform samplerCube u_env_map;      //textureslot 3

uniform int u_isgettingdamaged;


//Output
layout(location = 0) out vec4 f_color;


void main()
{
   // Vector from fragment to camera (camera always at 0,0,0)
    vec3 view = normalize(-v_position_view_space);

    // Normal from normal map
    vec3 normal = texture(u_normal_map, v_tex_coord).rgb;
    normal = normalize(normal * 2.0 - 1.0f);
    normal = normalize(v_tbn * normal);

    vec4 diffuseColor = texture(u_diffuse_map, v_tex_coord);
    if(diffuseColor.a < 0.1) {
        discard;
    }
    float alpha = diffuseColor.a ;

    //DirectionalLight
    vec3 light = normalize(-u_directional_light.direction);
    vec3 reflection = reflect(u_directional_light.direction, normal);
    vec3 ambient = u_directional_light.color * 0.3 * diffuseColor.xyz;
    vec3 diffuse = u_directional_light.color * max(dot(normal, light), 0.0) * diffuseColor.xyz;
    vec3 specular = u_directional_light.color * pow(max(dot(reflection, view), 0.000001), u_material.shininess) * u_material.specular;


    //sum phong elements
    f_color = vec4(ambient + /*(1.0 - shadow) **/ (diffuse + specular + u_material.emissive), alpha);

    if(u_isgettingdamaged==1)
    {
        f_color.r += 0.5;
    }

    float gamma=1;
    f_color.rgb = clamp( pow(f_color.rgb, vec3(1.0/gamma)), 0, 1);
    
}