#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices=4) out;

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

out vec2 passUVCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	float strength;
// uniform float stiffness;
uniform sampler2D vectorTexture;

/**
 * @brief not really the center, but whatever
 */
vec4 centerPos(vec4 pos1, vec4 pos2, vec4 pos3)
{
	vec4 dir1 = pos2 - pos1;
	vec4 dir2 = pos3 - pos2;
	return (pos1 + 0.5 * dir1 + 0.5 * dir2);
}

void main()
{
	vec4 pos1 = vec4(gl_in[0].gl_Position.xyz, 1.0);
	vec4 pos2 = vec4(gl_in[1].gl_Position.xyz, 1.0);
	vec4 pos3 = vec4(gl_in[2].gl_Position.xyz, 1.0);
	
	vec4 center = centerPos(pos1,pos2,pos3);
	vec4 centerView = (view * model * center);

	vec4 offset = (texture(vectorTexture,VertexIn[0].texCoord) * 2.0 - 1.0 )* strength;

	//bottom left
	vec4 point = vec4(-strength, 0.0, 0.0, 0.0) + centerView;
	VertexOut.texCoord = vec2(0.0,0.0);
	passUVCoord = vec2(0.0,0.0);
	VertexOut.position = point.xyz;
	VertexOut.normal = vec3(0.0, 0.0, -1.0);
	gl_Position = projection * point;
	EmitVertex();

	point = vec4(-strength, strength*2.0, 0.0, 0.0) + centerView;
	point += offset;
	VertexOut.texCoord = vec2(0.0,1.0);
	passUVCoord = vec2(0.0,1.0);
	VertexOut.position = point.xyz;
	VertexOut.normal = vec3(0.0, 0.0, -1.0);
	gl_Position = projection * point;
	EmitVertex();
	
	point = vec4(strength, 0.0, 0.0, 0.0) + centerView;
	VertexOut.texCoord = vec2(1.0,0.0);
	passUVCoord  = vec2(1.0,0.0);
	VertexOut.position = point.xyz;
	VertexOut.normal = vec3(0.0, 0.0, -1.0);
	gl_Position = projection * point;
	EmitVertex();
	
	point = ( vec4(strength, strength*2.0, 0.0, 0.0)  + centerView);
	point += offset;
	VertexOut.texCoord = vec2(1.0,1.0);
	passUVCoord  = vec2(1.0,1.0);
	VertexOut.position = point.xyz;
	VertexOut.normal = vec3(0.0, 0.0, -1.0);
	gl_Position = projection * point ;
	EmitVertex();

	EndPrimitive();

	// gl_Position = mvp * pos1;
	// EmitVertex();
	// gl_Position = mvp * ( center + strength * centNorm );
	// EmitVertex();
	// gl_Position = mvp * pos2;
	// EmitVertex();
	// // gl_Position = mvp * pos1;
	// // EmitVertex();
	// EndPrimitive();

	// gl_Position = mvp * pos2;
	// EmitVertex();
	// gl_Position = mvp * ( center + strength * centNorm );
	// EmitVertex();
	// gl_Position = mvp * pos3;
	// EmitVertex();
	// // gl_Position = mvp * pos2;
	// // EmitVertex();
	// EndPrimitive();

	// gl_Position = mvp * pos1;
	// EmitVertex();
	// gl_Position = mvp * ( center + strength * centNorm );
	// EmitVertex();
	// gl_Position = mvp * pos3;
	// EmitVertex();
	// // gl_Position = mvp * pos1;
	// // EmitVertex();
	// EndPrimitive();

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