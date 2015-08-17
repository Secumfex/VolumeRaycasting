#version 430
 
// in-variables
layout(location = 0) in vec4 positionAttribute;
layout(location = 1) in vec3 uvwCoordAttribute;
layout(location = 2) in vec4 normalAttribute;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// out-variables
out vec3 passWorldPosition;
out vec3 passPosition;
out vec3 passUVWCoord; // 3-scalar uvs
out vec3 passWorldNormal;
out vec3 passNormal;
out vec2 passImageCoord;

void main(){
    passUVWCoord = uvwCoordAttribute;
    vec4 worldPos = (model * positionAttribute);

    passWorldPosition = worldPos.xyz;
    passPosition = (view * worldPos).xyz;
 
    gl_Position =  projection * view * model * positionAttribute;
	passImageCoord = (gl_Position.xy / gl_Position.w) * 0.5 + 0.5;

    passWorldNormal = normalize( ( transpose( inverse( model ) ) * normalAttribute).xyz );
	passNormal = normalize( ( transpose( inverse( view * model ) ) * normalAttribute ).xyz );
}
