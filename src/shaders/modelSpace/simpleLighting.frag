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
	vec4 col;
	if (blendColor != 0.0)
	{
		vec4 tex = texture(tex, vec2(passUVCoord.x, passUVCoord.y));
		col = mix(color, tex, blendColor);
	}
	else{
		col = color;
	}

	vec3 to_light = normalize(vec3( 0.5, 0.5, 1.0));
	vec3 eye = normalize(-passPosition);
	float cos_phi = max(dot(passNormal, to_light), 0.0f);


	fragColor = col * cos_phi + col*0.1;
    fragPosition = vec4(passPosition,1);
    fragUVCoord = vec4(passUVCoord,0,0);
    fragNormal = vec4(passNormal,0);
	// fragColor = vec4(passNormal,1);
}
