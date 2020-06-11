#version 330 core

layout(location = 0) out vec4 f_color;

in vec3 v_normal;
in vec3 v_position;

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

uniform Material u_material;
uniform DirectionalLight u_directional_light;
uniform PointLight u_point_light;
uniform SpotLight u_spot_light;

void main()
{
    // Vector from fragment to camera (camera always at 0,0,0)
    vec3 view = normalize(-v_position);
    vec3 normal = normalize(v_normal);

    vec3 light = normalize(-u_directional_light.direction);
    vec3 reflection = reflect(u_directional_light.direction, normal);
    vec3 ambient = u_directional_light.ambient * u_material.diffuse;
    vec3 diffuse = u_directional_light.diffuse * max(dot(normal, light), 0.0) * u_material.diffuse;
    vec3 specular = u_directional_light.specular * pow(max(dot(reflection, view), 0.000001), u_material.shininess) * u_material.specular;

    light = normalize(u_point_light.position - v_position);
    reflection = reflect(-light, normal);
    float distance = length(u_point_light.position - v_position);
    float attentuation = 1.0 / ((1.0) + (u_point_light.linear*distance) + (u_point_light.quadratic*distance*distance));
    ambient += attentuation * u_point_light.ambient * u_material.diffuse;
    diffuse += attentuation * u_point_light.diffuse * max(dot(normal, light), 0.0) * u_material.diffuse;
    specular += attentuation * u_point_light.specular * pow(max(dot(reflection, view), 0.000001), u_material.shininess) * u_material.specular;

    light = normalize(u_spot_light.position - v_position);
    reflection = reflect(-light, normal);
    float theta = dot(light, u_spot_light.direction);
    float epsilon = u_spot_light.innerCone - u_spot_light.outerCone;
    float intensity = clamp((theta - u_spot_light.outerCone) / epsilon, 0.0f, 1.0f);
    if(theta > u_spot_light.outerCone) {
        ambient += u_spot_light.ambient * u_material.diffuse;
        diffuse += intensity * u_spot_light.diffuse * max(dot(normal, light), 0.0) * u_material.diffuse;
        specular += intensity * u_spot_light.specular * pow(max(dot(reflection, view), 0.000001), u_material.shininess) * u_material.specular;
    } else {
        ambient += u_spot_light.ambient * u_material.diffuse;
    }

    f_color = vec4(ambient + diffuse + specular + u_material.emissive, 1.0f);
}