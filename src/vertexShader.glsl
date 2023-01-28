#version 450 core

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) smooth out vec3 color;
layout(location = 1) smooth out vec3 normal;

layout(std140, binding = 0) uniform Matrix
{
    mat4 modelViewProjection;
};

void main()
{
    color = inColor;
    normal = inNormal;
    gl_Position = modelViewProjection * vec4(inWorldPos, 1.);
}