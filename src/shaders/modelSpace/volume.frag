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
uniform float uThresholdLMIP;
uniform float uColorEffectInfl;
uniform float uContrastEffectInfl;
uniform vec4 uMaxDistColor;
uniform vec4 uMinDistColor;
uniform int  uMinStepsLMIP; // experimental parameter

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

Sample mip(vec3 startUVW, vec3 endUVW, float stepSize, float thresholdLMIP, int minStepsLMIP)
{
	// length of ray in volume from start to end
	vec3 direction = endUVW - startUVW;
	float rayLength = length(direction);

	// necessary steps to get from start to end
	int steps = max( int( rayLength / stepSize), 1);

	int maxVal = -4000;
	vec3 maxUVW = startUVW;

	// count steps since local maximum
	int stepsSinceLM = 0; 

	// cast ray and perform mip
	for (int i = 0; i < steps; i++)
	{
		vec3 curUVW = samplePos(startUVW, direction, float(i) * stepSize);
		int  curVal = texture(volume_texture, curUVW).r;

		if ( curVal > maxVal) // approaching local maximum
		{
			maxVal = curVal;
			maxUVW = curUVW;

			stepsSinceLM = 0;
		}
		else // leaving local maximum
		{
			if ( maxVal > thresholdLMIP ) // LMIP is satisfied
			{
				stepsSinceLM++;
				if (stepsSinceLM > minStepsLMIP) // experimental: minimal offset to local maximum with no new maximum
				{
					break; // maxVal is a local maximum AND greater than LMIP threshold
				}
			}
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
	// linear mapping to [0,1]
	vec4 color = vec4( (float( value ) - uMinVal) / uRange );

	// linear mapping to [uMinDistColor, uMaxDistColor] (rgb colors)
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
	// threshold for LMIP to break traversal
	// steps since last local maximum before LMIP accepts it as such
	Sample maxSample = mip( uvwStart.rgb, uvwEnd.rgb, uStepSize, uThresholdLMIP, uMinStepsLMIP );

	// approximate distance to camera (linear interpolation)
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

	// distance color effect: red/blue color mapping
	vec4 mappedColor = mix( 
		vec4(mappedIntensity),
		transferFunction(value, distanceToCamera),
		uColorEffectInfl);

	fragColor = mappedColor;

    fragPosition = vec4(passPosition,1);
    fragUVCoord = vec4(passUVWCoord.xy,0,0);
    fragNormal = vec4(passNormal,0);
}
