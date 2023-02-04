#version 450 core

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) smooth out vec3 color;
layout(location = 1) smooth out vec3 normal;
layout(location = 2) smooth out vec3 vertPos;

layout(std140, binding = 0) uniform uniformLayout
{
    mat4 modelViewProjection;
    vec3 cameraPos;
};

void main()
{
    color = inColor;
    normal = inNormal;
    vertPos = inWorldPos;
    gl_Position = modelViewProjection * vec4(inWorldPos, 1.f);
}