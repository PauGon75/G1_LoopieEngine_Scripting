[vertex]
#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inColor;

out vec2  v_TexCoord;

uniform mat4 modelViewProj;

void main()
{
	gl_Position = modelViewProj * vec4(inPos, 1.0);
	v_TexCoord = inTexCoord;
}

[fragment]
#version 460 core
in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord);
	FragColor = texColor;
}
