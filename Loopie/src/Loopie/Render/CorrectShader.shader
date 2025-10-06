[vertex]
#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
out vec3 vertexColor;
uniform mat4 modelViewProj;
void main()
{
gl_Position = modelViewProj * vec4(inPos, 1.0);
vertexColor = inColor;
}

[fragment]
#version 460 core
in vec3 vertexColor;
out vec4 FragColor;
void main()
{
   FragColor = vec4(vertexColor, 1.0f);
}