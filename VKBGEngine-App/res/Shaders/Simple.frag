#version 450

layout(location = 0) in vec3 aColor;
layout (location = 0) out vec4 FragColor;

void main() {
//    FragColor = vec4(0.0, 0.4157, 1.0, 1.0);
    FragColor = vec4(aColor, 1.0);
}