#version 430

// in-variables
in vec3 passPosition;
in vec3 passUVWCoord;
in vec3 passNormal;
in vec2 passImageCoord;

// textures
uniform sampler2D front_uvw_map;
uniform sampler2D back_uvw_map;

// uniforms
uniform isampler3D volume_texture;
uniform float uRange;
uniform float uMinVal;
uniform float uStepSize;

// out-variables
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragPosition;
layout(location = 2) out vec4 fragUVCoord;
layout(location = 3) out vec4 fragNormal;

vec3 samplePos(vec3 start, vec3 direction, float t)
{
	return (start + t * direction);
}

int mip(vec3 startUVW, vec3 endUVW, float stepSize)
{
	// length of ray in volume from start to end
	vec3 direction = endUVW - startUVW;
	float rayLength = length(direction);

	// necessary steps to get from start to end
	int steps = max( int( rayLength / stepSize), 1);

	int maxVal = -4000;

	// cast ray and perform mip
	for (int i = 0; i < steps; i++)
	{
		vec3 curUVW = samplePos(startUVW, direction, float(i) * stepSize);
		int  curVal = texture(volume_texture, curUVW).r;

		maxVal = max( maxVal, curVal );
	}

	return maxVal;
}

vec4 transferFunction( int value )
{
	return vec4( (float( value ) - uMinVal) / uRange );
}

void main()
{
	// define ray boundaries
	vec3 uvwStart = texture( front_uvw_map, passImageCoord ).xyz;
	vec3 uvwEnd = texture( back_uvw_map, passImageCoord ).xyz;

	// value received for a maximum intensity projection 
	// from start to end point in volume using step size for ray sampling
	int maxVal = mip( uvwStart, uvwEnd, uStepSize );
	
	// compute fragment color
	vec4 color = transferFunction( maxVal );

	fragColor = vec4(color.rgb, 1.0);

    fragPosition = vec4(passPosition,1);
    fragUVCoord = vec4(passUVWCoord.xy,0,0);
    fragNormal = vec4(passNormal,0);
}
