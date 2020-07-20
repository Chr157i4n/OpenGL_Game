#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec2 a_tex_coord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_light_space_matrix;

out vec3 v_position;
out vec4 v_position_light_space;
out vec2 v_tex_coord;
out mat3 v_tbn;

void main()
{
    //a_position ist Modelspace
    //Position of Vertex in Worldspace
    vec4 position              = u_model * vec4(a_position, 1.0f);

    //Create TBN - Matrix (Tangent, Bitangent,Normal)
    mat3 invModelView = mat3(transpose(inverse(u_model*u_view)));
    vec3 t = normalize(mat3(invModelView) * a_tangent);
    vec3 n = normalize(mat3(invModelView) * a_normal);
    t = normalize(t - dot(t, n) * n); // Reorthogonalize with Gram-Schmidt process
    vec3 b = normalize(mat3(invModelView) * cross(n, t));
    mat3 tbn = mat3(t, b, n);
    v_tbn = tbn;

    //Texture coordinates (UVs) output
    v_tex_coord = a_tex_coord;

    v_position = vec3(u_view * position);
    v_position_light_space  =   u_light_space_matrix *  position;
    gl_Position             =   u_proj * u_view *       position;

}