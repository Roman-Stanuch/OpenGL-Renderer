#version 460 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 texCoord;

layout (binding = 0) uniform sampler2D texture1;
layout (binding = 1) uniform sampler2D texture2;

void main() {
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
}