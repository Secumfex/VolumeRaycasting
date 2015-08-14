#version 330

/*
* Simple phong lightning shader.
* Only point- and directional light are supported. 
* Spotlight and fog aren't nessecary for this context.
* Uniforms are handled by class PhongShaderProgram.h 
*/

//!< in-variables
in vec3 passPosition;
in vec2 passUVCoord;
in vec3 passNormal;

//!< uniforms
uniform mat4 view;

uniform struct
{
 vec3 diffuse;
 vec3 specular;
 float alpha;
 float shininess;
} mat;

uniform struct
{
 vec3 ambient;
 vec3 dir;
 vec3 color;
} light;

uniform int useColorTexture;
uniform sampler2D colorTexture; 

//! <out-variables
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragPosition;
layout(location = 2) out vec4 fragUVCoord;
layout(location = 3) out vec4 fragNormal;


void main() {
	vec3 lightVector;
	vec3 diffuse_color;
	float diffuse_alpha;

	//Diffuse
	vec3 light_cam = ( inverse(view) * vec4(light.dir,0)).xyz;
	lightVector = normalize(light_cam.xyz);
	float cos_phi = max(dot(passNormal, lightVector), 0.0f);

	//Specular
	vec3 eye = normalize(-passPosition);
	vec3 reflection = normalize(reflect(-lightVector, passNormal));
	float cos_psi_n = pow(max(dot(reflection, eye), 0.0f), mat.shininess);
		
	// Textures 
	if (useColorTexture != 0)
	{
		diffuse_color = texture( colorTexture, passUVCoord ).rgb;
		diffuse_alpha = texture( colorTexture, passUVCoord ).a;
	}
	else //Material
	{
		diffuse_color = mat.diffuse;
		diffuse_alpha = mat.alpha;
	}


	//All together
	fragColor.rgb = diffuse_color * light.ambient;
	fragColor.rgb += diffuse_color * cos_phi * light.color;
	fragColor.rgb += mat.specular * cos_psi_n * light.color;
	
	fragColor.a = diffuse_alpha;

	fragPosition = vec4(passPosition,1);
    fragUVCoord = vec4(passUVCoord,0,0);
    fragNormal.xyz = passNormal;
}

