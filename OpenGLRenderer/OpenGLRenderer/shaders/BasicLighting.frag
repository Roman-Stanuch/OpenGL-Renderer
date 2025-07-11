#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform vec3 lightColor;

layout (binding = 0) uniform sampler2D texture1;
layout (binding = 1) uniform sampler2D texture2;

void main() {
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2) * vec4(lightColor, 1.0);
}