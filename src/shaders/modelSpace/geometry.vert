#version 330
 
 /**
 * This simple shader passes out all important Attributes without computing anything.
 * Use this when all computation is done in a Geometry Shader
 */

 //!< in-variables
layout(location = 0) in vec4 positionAttribute;
layout(location = 1) in vec2 uvCoordAttribute;
layout(location = 2) in vec4 normalAttribute;

//!< out-variables
out VertexData {
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} VertexIn;

void main(){
    gl_Position = positionAttribute;
    VertexIn.texCoord = uvCoordAttribute;
    VertexIn.position = positionAttribute.xyz;
    VertexIn.normal = normalAttribute.xyz;
}
