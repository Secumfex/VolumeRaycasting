#version 330


/*
* Basic Fragmentshader.
*/

//!< in-variables
in vec3 passPosition;
in vec2 passUVCoord;
in vec3 passNormal;

//!< uniforms
uniform vec4 color;
uniform float blendColor;
uniform sampler2D tex;


//!< out-variables
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragPosition;
layout(location = 2) out vec4 fragUVCoord;
layout(location = 3) out vec4 fragNormal;

void main()
{
	if (blendColor != 0.0)
	{
		vec4 tex = texture(tex, vec2(passUVCoord.x, 1.0 - passUVCoord.y));
//		fragColor = mix(color, tex, blendColor);
		fragColor = vec4(tex.r, tex.g, tex.b,tex.a);
		// fragColor = vec4(tex.r * tex.a, tex.g * tex.a, tex.b * tex.a,tex.a);
	}
	else
	{
		fragColor = color;
	}

    fragPosition = vec4(passPosition,1);
    fragUVCoord = vec4(passUVCoord,0,0);
    fragNormal = vec4(passNormal,0);
}
