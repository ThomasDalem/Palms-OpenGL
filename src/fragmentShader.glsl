#version 450 core

layout(location = 0) out vec4 outColor;

layout(location = 0) smooth in vec3 color;
layout(location = 1) smooth in vec3 normal;
layout(location = 2) smooth in vec3 vertPos;

layout(std140, binding = 0) uniform uniformLayout
{
    mat4 modelViewProjection;
    vec3 cameraPos;
};

void main()
{
    float ambiantStrength = 0.1;
    vec3 finalColor = color * ambiantStrength;

    const vec3 lightPos = vec3(50.0f, 10.0f, 15.0f);
    const vec3 lightDir = normalize(lightPos - vertPos);
    const vec3 N = normalize(gl_FrontFacing ? normal : -normal);

    const float lambertTerm = max(dot(N, -lightDir), 0.0f);
    const vec3 r = reflect(lightDir, N);

    finalColor += lambertTerm;

    const vec3 viewDir = normalize(cameraPos - vertPos);
    //const vec3 halfVector = normalize(viewDir + L);
    const float specularFactor = pow(max(dot(r, viewDir), 0.f), 64);

    finalColor += vec3(specularFactor);

    outColor = vec4(finalColor, 1.0f);
}