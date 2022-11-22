#version 460 core
#define VERTICES_BETWEEN 10

layout (lines) in;
layout(line_strip, max_vertices = VERTICES_BETWEEN) out;

in Attributes {
	vec3 fragPosition;
	vec3 tangent;
} inAttributes[];

out Attributes {
	vec3 fragPosition;
	vec3 tangent;
} outAttributes;

uniform mat4 projection;
uniform mat4 view;

void main(void)
{
	gl_Position = projection * view * vec4(gl_in[0].gl_Position.xyz, 1.f);
	outAttributes.fragPosition = inAttributes[0].fragPosition;
	outAttributes.tangent = normalize(inAttributes[1].fragPosition - inAttributes[0].fragPosition);
	EmitVertex();

	gl_Position = projection * view * vec4(gl_in[1].gl_Position.xyz, 1.f);
	outAttributes.fragPosition = inAttributes[1].fragPosition;
	outAttributes.tangent = normalize(inAttributes[1].fragPosition - inAttributes[0].fragPosition);
	EmitVertex();

	EndPrimitive();
}