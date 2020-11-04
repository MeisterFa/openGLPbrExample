#version 410

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;
layout (location = 2) in vec2 aTexCoord;

layout (location=0) out vec3 vColor;
layout (location =1) out vec2 TexCoord;

void main()
{
    vColor = VertexColor;
    TexCoord = aTexCoord;
    gl_Position = vec4(VertexPosition,1.0);
}
