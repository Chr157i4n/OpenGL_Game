#version 330 core

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 color;
};

#define NR_POINT_LIGHTS 10  
struct PointLight {
    vec3 position;

    vec3 color;

    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    // Must be a normalized vector
    vec3 direction;

    float innerCone;
    float outerCone;

    vec3 color;
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
uniform DirLight u_directional_light;
uniform PointLight u_point_lights[NR_POINT_LIGHTS];
uniform SpotLight u_spot_light;

uniform sampler2D u_diffuse_map;    //textureslot 0
uniform sampler2D u_normal_map;     //textureslot 1
uniform sampler2D u_shadow_map;     //textureslot 2
uniform samplerCube u_env_map;      //textureslot 3

uniform vec3 u_camerapos;
uniform vec2 u_textureflow;

uniform int u_showNormalMode;
uniform int u_isgettingdamaged;
uniform vec3 u_mark_color;
uniform int u_isusingfullenvmap;
uniform int u_shadow_mode;           //0 is off, 1 is hard, 2 is soft



//Output
layout(location = 0) out vec4 f_color;




//global variables
float shadow=0;
vec3 normal;
vec2 tex_coord;
vec3 tex_color;
vec3 viewDir;


float calcShadows(vec3 position_light_space)
{
    shadow = 0.0;
    //float bias = 0.0002;
    float bias = max(0.00001 * (1.0 - dot(normal, u_directional_light.direction)), 0.0003);  

    if(u_shadow_mode == 0)  //off
    {
        shadow = 0.0;
    }
    else if(u_shadow_mode == 1) //hard shadow
    {
        vec3 projCoords = position_light_space.xyz;
        projCoords = projCoords * 0.5 + 0.5;    // transform to [0,1] range
        float closestDepth = texture(u_shadow_map, projCoords.xy).r; 
        float currentDepth = projCoords.z;
        shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    }
    else if(u_shadow_mode == 2) //soft shadow
    {
        
        vec3 projCoords = position_light_space.xyz;
        projCoords = projCoords * 0.5 + 0.5;    // transform to [0,1] range
        vec2 texelSize = 1.0 / textureSize(u_shadow_map, 0);
        float currentDepth = projCoords.z;

        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(u_shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
    }
    return shadow;
}


vec3 calcDirLight(DirLight light)
{
    vec3 ambient, diffuse, specular;
    
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.00001), u_material.shininess);
    // combine results
    ambient  = light.color * 0.3  * tex_color;
    diffuse  = light.color * diff * tex_color * (1 - shadow);
    specular = light.color * spec * tex_color * (1 - shadow);

    vec3 sum = ambient + diffuse + specular;
    return sum;
}


vec3 calcPointLight(PointLight light)
{
    vec3 ambient, diffuse, specular;

    if(light.color == vec3(0,0,0))
        return vec3(0,0,0);
    
    vec3 lightDir = normalize(light.position - v_position_view_space);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.00001);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.00001), u_material.shininess);
    // attenuation
    float distance_light    = length(light.position - v_position_view_space);
    float attenuation = 1.0 / (light.linear * distance_light + light.quadratic * (distance_light * distance_light));    
    // combine results
    ambient  = light.color * 0.3  * tex_color;
    diffuse  = light.color * diff * tex_color;
    specular = light.color * spec * tex_color;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 sum = ambient + diffuse + specular;
    return sum;
}


vec3 calcSpotLight(SpotLight light)
{
    vec3 ambient, diffuse, specular;
    
    vec3 lightDir = normalize(light.position - v_position_view_space);
    vec3 reflection = reflect(-lightDir, normal);
    float theta = dot(lightDir, light.direction);
    float epsilon = light.innerCone - light.outerCone;
    float intensity = clamp((theta - light.outerCone) / epsilon, 0.0f, 1.0f);

    ambient = light.color * 0.3 * tex_color;

    if(theta > light.outerCone) 
    {  
        diffuse = light.color * intensity * max(dot(normal, light.direction), 0.0) * tex_color;
        specular = light.color * intensity * pow(max(dot(reflection, viewDir), 0.000001), u_material.shininess) * u_material.specular;
    }

    vec3 sum = ambient + diffuse + specular;
    return sum;
}


void main()
{
    tex_coord = v_tex_coord + u_textureflow;
    tex_color = vec3(texture(u_diffuse_map, tex_coord));

    // Vector from fragment to camera (camera always at 0,0,0)
    viewDir = normalize(-v_position_view_space);

    // Normal from normal map
    vec3 normaltex = texture(u_normal_map, tex_coord).rgb;
    normal = normalize(normaltex * 2.0 - 1.0f);
    normal = normalize(v_tbn * normal);

    vec4 diffuseColor = texture(u_diffuse_map, tex_coord);
    if(diffuseColor.a < 0.1) {
        discard;
    }
    float alpha = diffuseColor.a;

    if(u_showNormalMode == 1) {
        f_color = vec4(normal, 1.0);
        return;
    }

    calcShadows(v_position_light_space); 


    // phase 1: Directional lighting
    vec3 resultColor = calcDirLight(u_directional_light);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        resultColor += calcPointLight(u_point_lights[i]);  
    }
    // phase 3: Spot light
    resultColor += calcSpotLight(u_spot_light);    

    if(u_isgettingdamaged==1)
    {
        resultColor.r += 0.5;
    }

    resultColor += u_mark_color;

    vec3 I = normalize(v_position_world_space - u_camerapos);
    vec3 R = reflect(I, v_normal_world_space) * normaltex;

    vec3 envmapcolor = texture(u_env_map, R).rgb;
    resultColor = mix(resultColor, envmapcolor, u_material.specular.x/2);

    float gamma=1;
    resultColor.rgb = pow(resultColor.rgb, vec3(1.0/gamma));

    f_color = vec4(resultColor, alpha);
}