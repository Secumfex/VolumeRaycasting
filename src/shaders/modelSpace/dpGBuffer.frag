#version 430

//incoming data for the single textures
// in vec3 passPosition;
// in vec2 passUVCoord;
// in vec3 passNormal;

in VertexData {
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} VertexOut;

uniform vec4  color;
uniform float mixTexture;
uniform sampler2D tex;

uniform int peel_level;
uniform sampler2D lastDepth;

//writable textures for deferred screen space calculations
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormal;
layout(location = 2) out vec4 fragPosition;
layout(location = 3) out vec4 fragUVCoord;
 
void main(){

	fragColor = color;
	if ( mixTexture != 0.0)
	{
		fragColor = mix(color, texture(tex, VertexOut.texCoord), mixTexture );
	}

	//TODO check with before depth peel from lastDepth texture
	if (peel_level > 0)
	{
		vec4 depth = texture(lastDepth, gl_FragCoord.xy);
		if ( gl_FragCoord.z < depth.r)
		{
			discard;
		}
	}

    // fragPosition = vec4(passPosition,1);
    // fragUVCoord = vec4(passUVCoord,0,0);
    // fragNormal = vec4(passNormal,0);

    fragPosition = vec4(VertexOut.position,1);
    fragUVCoord = vec4(VertexOut.texCoord,0,0);
    fragNormal = vec4(VertexOut.normal,0);

}
