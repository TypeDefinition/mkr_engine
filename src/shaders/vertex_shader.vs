#version 460 core
layout (location = 0) in vec3 a_position;

uniform mat4 u_mat_mvp;

void main()
{
    gl_Position = u_mat_mvp * vec4(a_position, 1.0);
}