#version 450

layout (location = 0) in vec3 iColor;
layout (location = 1) in vec3 iNormal;

layout (location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec3 directionalLightPos;
} globalUbo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
//    FragColor = vec4(0.0, 0.4157, 1.0, 1.0);
    vec3 normal = normalize(iNormal);

    float diffuse = max(dot(globalUbo.directionalLightPos, normal), 0.0);
    float ambient = 0.01;

    vec3 color = iColor * diffuse + ambient;

    FragColor = vec4(color, 1.0);
}