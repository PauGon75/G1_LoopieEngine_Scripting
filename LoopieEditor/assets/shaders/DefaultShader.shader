[vertex]
#version 460 core
/// DO NOT MODIFY
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec4 a_Color;

uniform mat4 lp_ViewProjection;
uniform mat4 lp_Transform;
///

out vec2 v_TexCoord;
out vec3 v_Normal;

void main()
{
    gl_Position = lp_ViewProjection * lp_Transform * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_Normal = mat3(lp_Transform) * a_Normal;
}


[fragment]
#version 460 core
in vec2 v_TexCoord;
in vec3 v_Normal;
out vec4 FragColor;

uniform sampler2D u_Albedo;
uniform vec4 u_Color;

void main()
{
	vec4 texColor = texture(u_Albedo, v_TexCoord);
	if(texColor.a < 0.5f)
		discard;
	vec3 lightDir = normalize(vec3(0.3, 0.7, 0.5));
    vec3 normal = normalize(v_Normal);

    float diff = max(dot(normal, lightDir), 0.4);

    diff = mix(1.0, diff, 0.8);

    vec3 litColor = texColor.rgb * diff;
    FragColor = vec4(litColor, texColor.a) * u_Color;
}
