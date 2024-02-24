#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 oColor;
layout(location = 1) out vec3 oNormal;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec3 directionalLightPos;
} globalUbo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main()
{
    oColor = color;
//    mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
    oNormal = normalize(mat3(push.normalMatrix) * normal);

    gl_Position = globalUbo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);
}
