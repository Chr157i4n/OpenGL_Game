#version 330 core

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

struct PointLight {
    vec3 position;

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;

    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    // Must be a normalized vector
    vec3 direction;

    float innerCone;
    float outerCone;

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

//Input from VRAM
in vec3 v_position_world_space;
in vec3 v_position_view_space;
in vec3 v_position_light_space;
in vec2 v_tex_coord;
in mat3 v_tbn;
in vec3 v_normal_world_space;

//Input from CPU
uniform Material u_material;
uniform DirectionalLight u_directional_light;
uniform PointLight u_point_light;
uniform SpotLight u_spot_light;

uniform sampler2D u_diffuse_map;    //textureslot 0
uniform sampler2D u_normal_map;     //textureslot 1
uniform sampler2D u_shadow_map;     //textureslot 2
uniform samplerCube u_env_map;      //textureslot 3

uniform vec3 u_camerapos;

uniform int u_showNormalMode;
uniform int u_isgettingdamaged;
uniform int u_isusingfullenvmap;



//Output
layout(location = 0) out vec4 f_color;



float ShadowCalculation(vec3 position_light_space)
{
    float shadow = 0.0;
    vec3 projCoords = position_light_space.xyz;
    projCoords = projCoords * 0.5 + 0.5;    // transform to [0,1] range
    vec2 texelSize = 1.0 / textureSize(u_shadow_map, 0);
    float currentDepth = projCoords.z;

    float bias = 0.00001;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

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

    if(u_showNormalMode == 1) {
        f_color = vec4(normal, 1.0);
        return;
    }



    //DirectionalLight
    vec3 light = normalize(-u_directional_light.direction);
    vec3 reflection = reflect(u_directional_light.direction, normal);
    vec3 ambient = u_directional_light.ambient * diffuseColor.xyz;
    vec3 diffuse = u_directional_light.diffuse * max(dot(normal, light), 0.0) * diffuseColor.xyz;
    vec3 specular = u_directional_light.specular * pow(max(dot(reflection, view), 0.000001), u_material.shininess) * u_material.specular;

    //PointLight
    light = normalize(u_point_light.position - v_position_view_space);
    reflection = reflect(-light, normal);
    float distance = length(u_point_light.position - v_position_view_space);
    float attentuation = 1.0 / ((1.0) + (u_point_light.linear*distance) + (u_point_light.quadratic*distance*distance));
    ambient += attentuation * u_point_light.ambient * diffuseColor.xyz;
    diffuse += attentuation * u_point_light.diffuse * max(dot(normal, light), 0.0) * diffuseColor.xyz;
    specular += attentuation * u_point_light.specular * pow(max(dot(reflection, view), 0.000001), u_material.shininess) * u_material.specular;

    //SpotLight
    light = normalize(u_spot_light.position - v_position_view_space);
    reflection = reflect(-light, normal);
    float theta = dot(light, u_spot_light.direction);
    float epsilon = u_spot_light.innerCone - u_spot_light.outerCone;
    float intensity = clamp((theta - u_spot_light.outerCone) / epsilon, 0.0f, 1.0f);
    if(theta > u_spot_light.outerCone) {
        ambient += u_spot_light.ambient * diffuseColor.xyz;
        diffuse += intensity * u_spot_light.diffuse * max(dot(normal, light), 0.0) * diffuseColor.xyz;
        specular += intensity * u_spot_light.specular * pow(max(dot(reflection, view), 0.000001), u_material.shininess) * u_material.specular;
    } else {
        ambient += u_spot_light.ambient * diffuseColor.xyz;
    }

    float shadow = ShadowCalculation(v_position_light_space); 

    //sum phong elements
    f_color = vec4( ambient + (1.0 - shadow) * (diffuse + specular + u_material.emissive), alpha);
    f_color = clamp( f_color, 0, 1);



    if(u_isgettingdamaged==1)
    {
        f_color.r += 0.5;
    }

    vec3 I = normalize(v_position_world_space - u_camerapos);
    vec3 R = reflect(I, v_normal_world_space);
    //float ratio = 1.00 / 1.52;
    //vec3 R = refract(I, v_normal_world_space, ratio);


    vec4 envmapcolor = vec4(texture(u_env_map, R).rgb, 1.0);
    f_color = mix(f_color, envmapcolor, u_material.specular.x/2);

    float gamma=1;
    f_color.rgb = clamp( pow(f_color.rgb, vec3(1.0/gamma)), 0, 1);
}