#version 450

layout (location = 0) in vec3 iColor;

layout (location = 0) out vec4 FragColor;

layout(push_constant) uniform Push {
    mat4 transform;
    vec3 color;
} push;

void main() {
//    FragColor = vec4(0.0, 0.4157, 1.0, 1.0);
    FragColor = vec4(iColor, 1.0);
}