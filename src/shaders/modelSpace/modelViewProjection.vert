#version 330
 
 /**
 * This simple shader passes out all important Attributes.
 */

 //!< in-variables
layout(location = 0) in vec4 positionAttribute;
layout(location = 1) in vec2 uvCoordAttribute;
layout(location = 2) in vec4 normalAttribute;

//!< uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//!< out-variables
out vec3 passWorldPosition;
out vec3 passPosition;
out vec2 passUVCoord;
out vec3 passWorldNormal;
out vec3 passNormal;

void main(){
    passUVCoord = uvCoordAttribute;
    vec4 worldPos = (model * positionAttribute);

    passWorldPosition = worldPos.xyz;
    passPosition = (view * worldPos).xyz;
    
    gl_Position =  projection * view * model * positionAttribute;

    passWorldNormal = normalize( ( transpose( inverse( model ) ) * normalAttribute).xyz );
	passNormal = normalize( ( transpose( inverse( view * model ) ) * normalAttribute ).xyz );
}
