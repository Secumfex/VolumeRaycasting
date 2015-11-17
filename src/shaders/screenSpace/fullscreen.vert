#version 330

/*
* This shader renders something on a screenfilling quad. 
* The renderpass needs a quad as VAO.
*/

//!< in-variables
in vec4 pos;

//!< out-variables
out vec3 passPosition;
out vec2 passUV;

void main() {
	passPosition = pos.xyz;
	passUV = pos.xy;
	gl_Position = vec4(pos.xy * 2 - 1, 0, 1);
}
