#version 430

//!< in-variables
in vec3 passPosition;
in vec2 passUVCoord;
in vec3 passNormal;

//!< uniforms
uniform sampler2D debugTex;
uniform isampler3D tex;
uniform float uSinus;
uniform float uRange;
uniform vec4 uColor;

//!< out-variables
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragPosition;
layout(location = 2) out vec4 fragUVCoord;
layout(location = 3) out vec4 fragNormal;

void main()
{
	float pulse = (uSinus / 2.0) + 0.5;

	vec2 uv = passUVCoord;

	ivec4 smple = texture(tex, vec3(uv, pulse));
	int vIntensity = smple.r;
	
	vec4 color = vec4( float(vIntensity) / uRange );

	fragColor = vec4(color.rgb, 1.0);

    fragPosition = vec4(passPosition,1);
    fragUVCoord = vec4(passUVCoord,0,0);
    fragNormal = vec4(passNormal,0);
}
