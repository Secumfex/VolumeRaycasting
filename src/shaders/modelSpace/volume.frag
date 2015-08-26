#version 430

// in-variables
in vec3 passPosition;
in vec3 passUVWCoord;
in vec3 passNormal;
in vec2 passImageCoord;

// textures
uniform sampler2D back_uvw_map;
uniform sampler2D front_uvw_map;

// uniforms
uniform isampler3D volume_texture;
uniform float uRange;
uniform float uMinVal;
uniform float uStepSize;
uniform float uColorEffectInfl;
uniform float uContrastEffectInfl;
uniform vec4 uMaxDistColor;
uniform vec4 uMinDistColor;

// out-variables
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragPosition;
layout(location = 2) out vec4 fragUVCoord;
layout(location = 3) out vec4 fragNormal;

struct Sample
{
	int value;
	vec3 uvw;
};

vec3 samplePos(vec3 start, vec3 direction, float t)
{
	return (start + t * direction);
}

Sample mip(vec3 startUVW, vec3 endUVW, float stepSize)
{
	// length of ray in volume from start to end
	vec3 direction = endUVW - startUVW;
	float rayLength = length(direction);

	// necessary steps to get from start to end
	int steps = max( int( rayLength / stepSize), 1);

	int maxVal = -4000;
	vec3 maxUVW = startUVW;

	// cast ray and perform mip
	for (int i = 0; i < steps; i++)
	{
		vec3 curUVW = samplePos(startUVW, direction, float(i) * stepSize);
		int  curVal = texture(volume_texture, curUVW).r;

		if ( curVal > maxVal)
		{
			maxVal = curVal;
			maxUVW = curUVW;
		}		
	}

	Sample result;
	result.value = maxVal;
	result.uvw = maxUVW;

	return result;
}

float contrastAttenuationQuadratic(float relVal, float dist)
{	
	float quadrDist = dist*dist;
	return  0.5 * quadrDist + relVal * (1.0 - quadrDist);
}

float contrastAttenuationLinear(float relVal, float dist)
{	
	return  0.5 * dist + relVal * (1.0 - dist);
}

vec4 transferFunction( int value, float distanceToCamera )
{
	vec4 color = vec4( (float( value ) - uMinVal) / uRange );
	color = color * ( mix( uMinDistColor, uMaxDistColor, distanceToCamera ) );

	return color; 
}

void main()
{
	// define ray boundaries
	vec4 uvwStart = texture( front_uvw_map, passImageCoord );
	vec4 uvwEnd = texture( back_uvw_map, passImageCoord );

	// value received for a maximum intensity projection 
	// from start to end point in volume using step size for ray sampling
	Sample maxSample = mip( uvwStart.rgb, uvwEnd.rgb, uStepSize );

//	float distanceToCamera = min(1.0, length(maxSample.uvw - uvwStart.rgb)); // placeholder
	float distanceToCamera = mix(
		uvwStart.a, // depth buffer 
		uvwEnd.a,   // depth buffer
		min(1.0, length(maxSample.uvw - uvwStart.rgb)));

	// distance color effect: decreasing contrast 
	float relativeIntensity = ( float( maxSample.value ) - uMinVal ) / uRange;
	float mappedIntensity = mix( 
		relativeIntensity,
		contrastAttenuationQuadratic(relativeIntensity, distanceToCamera),
		uContrastEffectInfl);

	int value = int( uMinVal + mappedIntensity * uRange );

	// color 
	vec4 mappedColor = mix( 
		vec4(mappedIntensity),
		transferFunction(value, distanceToCamera),
		uColorEffectInfl);

	fragColor = mappedColor;

    fragPosition = vec4(passPosition,1);
    fragUVCoord = vec4(passUVWCoord.xy,0,0);
    fragNormal = vec4(passNormal,0);
}
