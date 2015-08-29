#version 430

// in-variables
in vec2 passImageCoord;

// textures
uniform sampler2D  back_uvw_map;   // uvw coordinates map of back  faces
uniform sampler2D front_uvw_map;   // uvw coordinates map of front faces
uniform isampler3D volume_texture; // volume 3D integer texture sampler

////////////////////////////////     UNIFORMS      ////////////////////////////////
// color mapping related uniforms 
uniform float uWindowingRange;  // windowing value range
uniform float uWindowingMinVal; // windowing lower bound
uniform float uWindowingMaxVal; // windowing upper bound

// ray traversal related uniforms
uniform float uRayParamStart;  // constrained sampling parameter intervall start
uniform float uRayParamEnd;	// constrained sampling parameter intervall end
uniform float uStepSize;		// ray sampling step size

// LMIP parameter
uniform float uThresholdLMIP;	// LMIP value threshold to be exceeded to trigger

// depth effect parameters
uniform float uColorEffectInfl;    // color    effect: influence parameter [0,1]
uniform float uContrastEffectInfl; // contrast effect: influence parameter [0,1]
uniform vec4  uMaxDistColor; // color effect: color at max distance
uniform vec4  uMinDistColor; // color effect: color at min distance

/********************    EXPERIMENTAL PARAMETERS      ***********************/ 
uniform int  uMinStepsLMIP;    // parameter for LMIP 'smoothing'
uniform int  uMinValThreshold; // minimal value threshold for sample to be considered; deceeding values will be ignored  
uniform int  uMaxValThreshold; // maximal value threshold for sample to be considered; exceeding values will be ignored
uniform float uMinDepthRange; // lower bound of constrained depth intervall; depth is mapped to this interval
uniform float uMaxDepthRange; // upper bound of constrained depth intervall; depth is mapped to this interval 
/****************************************************************************/
///////////////////////////////////////////////////////////////////////////////////

// out-variables
layout(location = 0) out vec4 fragColor;

/**
 * @brief Struct of a volume sample point
 */
struct VolumeSample
{
	int value; // scalar intensity
	vec3 uvw;  // uvw coordinates
};

/**
 * @brief retrieve value for a maximum intensity projection	
 * 
 * @param startUVW start uvw coordinates
 * @param endUVW end uvw coordinates
 * @param stepSize of ray traversal
 * @param thresholdLMIP value to exceed for LMIP to break traversal
 * @param minStepsLMIP since last local maximum before LMIP breaks traversal (experimental parameter)
 * @param minValueThreshold to ignore values when deceeded (experimental parameter)
 * @param maxValueThreshold to ignore values when exceeded (experimental parameter)
 * 
 * @return sample point in volume, holding value and uvw coordinates
 */
VolumeSample mip(vec3 startUVW, vec3 endUVW, float stepSize, int thresholdLMIP, int minStepsLMIP, int minValueThreshold, int maxValueThreshold)
{
	float parameterStepSize = stepSize / length(endUVW - startUVW); // necessary parametric steps to get from start to end

	VolumeSample curMax;	 // result variable
	curMax.value = -10000;   // initialized to arbitrary value out of CT/MRT range
	curMax.uvw   = startUVW; // initialized to arbitrary uvw coordinates

	int stepsSinceLM = 0; 	 // used in conjunction with experimental minStepsLMIP parameter

	// traversa ray, perform mip
	for (float t = 0.0; t < 1.0 + (0.5 * parameterStepSize); t += parameterStepSize)
	{
		vec3 curUVW = mix( startUVW, endUVW, t);
		
		// retrieve current sample
		VolumeSample curSample;
		curSample.value = texture(volume_texture, curUVW).r;
		curSample.uvw   = curUVW;

		/// experimental: ignore values exceeding or deceeding some thresholds
		if ( curSample.value > maxValueThreshold || curSample.value < minValueThreshold)
		{
			continue;
		}

		// found new maximum
		if ( curSample.value > curMax.value)
		{
			curMax = curSample;

			stepsSinceLM = 0; // always reset while approaching local maximum, see usage below
		}
		else // leaving local maximum
		{
			// LMIP is satisfied
			if ( curMax.value > thresholdLMIP ) 
			{
				stepsSinceLM++; // increment steps since departing last local maximum

				/// experimental: minimal offset to local maximum with no new maximum
				if (stepsSinceLM > minStepsLMIP) 
				{
					break; // current max sample is a local maximum AND greater than LMIP threshold
				}
			}
		}		
	}

	// return maximum sample with maximum intensity
	return curMax;
}


/**
 * @brief shifts the relative value closer to 0.5, based on provided distance
 * @param relVal the arbitrary, relative value in [0,1] to be mapped
 * @param dist distance to be used as mixing parameter
 * 
 * @return mapped value with decreased contrast
 */
float contrastAttenuationLinear(float relVal, float dist)
{	
	return  mix(relVal, 0.5, dist);
}

/**
 * @brief shifts the relative value closer to 0.5, based on provided distance. Alternative to above.
 * @param relVal the arbitrary, relative value in [0,1] to be mapped
 * @param dist distance to be used as mixing parameter, squared
 * 
 * @return mapped value with decreased contrast
 */
float contrastAttenuationSquared(float relVal, float dist)
{	
	float squaredDist = dist*dist;
	return  mix(relVal, 0.5, squaredDist);
}

vec4 transferFunction( int value, float distanceToCamera )
{
	// linear mapping to grayscale color [0,1]
	vec4 color = vec4( (float( value ) - uWindowingMinVal) / uWindowingRange );

	// linear mapping to [uMinDistColor, uMaxDistColor] (rgb colors)
	color = color * ( mix( uMinDistColor, uMaxDistColor, distanceToCamera ) );

	return color; 
}

void main()
{
	// define ray start and end points in volume
	vec4 uvwStart = texture( front_uvw_map, passImageCoord );
	vec4 uvwEnd   = texture( back_uvw_map,  passImageCoord );

	// apply offsets to start and end of ray
	uvwStart.rgb = mix (uvwStart.rgb, uvwEnd.rgb, uRayParamStart);
	uvwEnd.rgb   = mix( uvwStart.rgb, uvwEnd.rgb, uRayParamEnd);

	// find maximum intensity sample
	VolumeSample maxSample = mip( 
		uvwStart.rgb, 			// ray start
		uvwEnd.rgb,   			// ray end
		uStepSize,    			// sampling step size
		int(uThresholdLMIP),	// LMIP threshold
		uMinStepsLMIP,			// LMIP steps
		uMinValThreshold,	// min value threshold 
		uMaxValThreshold);   // max value threshold

	// distance to camera 
	// for approximate (faster) distance: remove sqrt and pow( ,2) --> (linear interpolation)
	float depth = pow( mix(
		sqrt( uvwStart.a ), // front depth 
		sqrt( uvwEnd.a ),   // back depth
		min( 1.0, length(maxSample.uvw - uvwStart.rgb) ) // relative distance
		), 2);

	/// experimental: map depth to constrained depth interval
	depth = pow(max(0.0, min(1.0, (sqrt(depth) - uMinDepthRange)/(uMaxDepthRange - uMinDepthRange) )), 2);
	
	// distance color effect: decreasing contrast 
	float relativeIntensity = max(0.0, min(1.0, (float(maxSample.value) - uWindowingMinVal)/ uWindowingRange)); //
	float mappedIntensity   = mix( 
		relativeIntensity,
		contrastAttenuationLinear(relativeIntensity, depth),
		// contrastAttenuationSquared(relativeIntensity, depth), /// experimental: for a more dramatic effect
		uContrastEffectInfl);

	// value mapped according to windowing configuration
	int mappedValue = int( mix(
		uWindowingMinVal,
		uWindowingMaxVal,
		mappedIntensity));
	
	// distance color effect: red/blue color mapping
	vec4 mappedColor = mix( 
		vec4(mappedIntensity),
		transferFunction(mappedValue, depth),
		uColorEffectInfl);

	// final color
	fragColor = mappedColor;
}
