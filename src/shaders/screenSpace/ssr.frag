#version 430

in vec2 passUV;

uniform sampler2D normalMap;
uniform sampler2D positionMap;

uniform mat4 view;

uniform mat4 bbModel;
uniform sampler2D bbTex;

uniform float bbWidth;
uniform float bbHeight;

uniform float strength;

out vec4 fragmentColor;

vec3 intersectPlane( vec3 p, vec3 d, vec3 v1, vec3 v2, vec3 v3)
{
   mat3 m = inverse(mat3(d, v2 - v1, v3 - v1));
   
   vec3 result = m * (p - v1);

   return result;
}

void main() {
    vec4 position = texture(positionMap, passUV);
    vec4 normal =   texture(normalMap, passUV);
    
    if (position.a != 0.0)
    {
        vec3 eye = normalize(-position.xyz);

        //calculate lighting with given position, normal and lightposition
        vec3 nReflection = reflect( -eye, normal.xyz );

        // TODO compute Intersection with BB plane
        // check against model size
        vec3 vert0 = vec3( bbWidth, bbHeight, 0.0);
        vec3 vert1 = vec3( 0.0, 0.0, 0.0);
        vec3 vert2 = vec3( 0.0, bbHeight, 0.0);

        vert0 = ( view * bbModel * vec4(vert0,1.0) ).xyz;
        vert1 = ( view * bbModel * vec4(vert1,1.0) ).xyz;
        vert2 = ( view * bbModel * vec4(vert2,1.0) ).xyz;

        vec3 uv = intersectPlane(position.xyz, nReflection, vert2, vert0, vert1);

        if (uv.y <= 1.0 && uv.z <= 1.0 && uv.y >= 0.0 && uv.z >= 0.0 && uv.x < -0.001)
        {
            uv.z = 1.0 - uv.z; //flip v coordinate
            vec4 texColor = texture(bbTex, uv.yz);
            float brightness = max(texColor.r, max(texColor.g, texColor.b));
            fragmentColor = vec4(texColor.rgb, brightness * strength);
        }
        else
        {
            discard;
        }
    }
}