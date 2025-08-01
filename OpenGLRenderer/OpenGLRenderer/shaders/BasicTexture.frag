#version 460 core
out vec4 FragColor;

in vec2 texCoord;

layout (binding = 0) uniform sampler2D texture0;
layout (binding = 1) uniform sampler2D texture1;

void main() {
    FragColor = texture(texture0, texCoord);
}