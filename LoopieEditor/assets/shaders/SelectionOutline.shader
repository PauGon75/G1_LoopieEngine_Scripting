[vertex]
#version 460 core
/// DO NOT MODIFY
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec4 a_Color;

layout (std140, binding = 0) uniform Matrices
{
    mat4 lp_Projection;
    mat4 lp_View;
};

uniform mat4 lp_Transform;
///

uniform float outlineThickness = 0.03;

void main()
{
    // Convert position from local space to view space
    gl_Position = lp_View * lp_Transform * vec4(a_Position, 1.0);
    // Conver normal from local space to view space
    vec3 normalViewSpace = normalize(mat3(lp_View * lp_Transform) * a_Normal);
    // Scale the object by moving its vertices towards normal direction in view space
    gl_Position.xyz += normalViewSpace * outlineThickness;
    // Convert position to Clip space
    gl_Position = lp_Projection * gl_Position;

}

[fragment]
#version 460 core
out vec4 FragColor;

uniform vec4 u_Color = vec4(0.0, 0.5, 0.75, 1.0);

void main()
{
    FragColor = u_Color;
} 
