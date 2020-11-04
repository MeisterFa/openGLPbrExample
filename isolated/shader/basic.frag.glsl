#version 410

layout(location=0) in vec3 vColor;
layout(location=1) in vec2 TexCoord;

layout (location=0) out vec4 FragColor;

uniform sampler2D ourTexture;


void main() {
    FragColor = texture(ourTexture, TexCoord);
}
