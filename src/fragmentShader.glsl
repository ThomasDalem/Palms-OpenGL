#version 450 core

layout(location = 0) out vec4 outColor;

layout(location = 0) smooth in vec3 color;
layout(location = 1) smooth in vec3 normal;

layout(std140, binding = 0) uniform Matrix
{
    mat4 modelViewProjection;
};

void main()
{
    const vec3 N = normalize(gl_FrontFacing ? normal : -normal);
    outColor = vec4(color * N, 1.0f);
}