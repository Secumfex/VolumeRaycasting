#version 430

layout (triangles) in;
layout (line_strip, max_vertices=12) out;

in VertexData {
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} VertexIn[];

out VertexData {
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} VertexOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	float strength;

vec4 centerPos(vec4 pos1, vec4 pos2, vec4 pos3)
{
	vec4 dir1 = pos2 - pos1;
	vec4 dir2 = pos3 - pos2;
	return (pos1 + 0.5 * dir1 + 0.5 * dir2);
}

void main()
{
	mat4 mvp = projection * view * model;

	vec4 pos1 = vec4(gl_in[0].gl_Position.xyz, 1.0);
	vec4 pos2 = vec4(gl_in[1].gl_Position.xyz, 1.0);
	vec4 pos3 = vec4(gl_in[2].gl_Position.xyz, 1.0);
	
	vec4 norm1 = vec4(VertexIn[0].normal, 0.0);
	vec4 norm2 = vec4(VertexIn[1].normal, 0.0);
	vec4 norm3 = vec4(VertexIn[2].normal, 0.0);

	vec4 center = centerPos(pos1,pos2,pos3);
	vec4 centNorm = normalize(mix(norm1,norm3,0.5));

	gl_Position = mvp * pos1;
	EmitVertex();
	gl_Position = mvp * ( center + strength * centNorm );
	EmitVertex();
	gl_Position = mvp * pos2;
	EmitVertex();
	// gl_Position = mvp * pos1;
	// EmitVertex();
	EndPrimitive();

	gl_Position = mvp * pos2;
	EmitVertex();
	gl_Position = mvp * ( center + strength * centNorm );
	EmitVertex();
	gl_Position = mvp * pos3;
	EmitVertex();
	// gl_Position = mvp * pos2;
	// EmitVertex();
	EndPrimitive();

	gl_Position = mvp * pos1;
	EmitVertex();
	gl_Position = mvp * ( center + strength * centNorm );
	EmitVertex();
	gl_Position = mvp * pos3;
	EmitVertex();
	// gl_Position = mvp * pos1;
	// EmitVertex();
	EndPrimitive();

	// gl_Position = mvp * pos1;
	// EmitVertex();
	// gl_Position = mvp * pos2;
	// EmitVertex();
	// gl_Position = mvp * pos3;
	// EmitVertex();
	// gl_Position = mvp * pos1;
	// EmitVertex();

	// EndPrimitive();
}