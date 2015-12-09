#version 430

in vec2 passUV;

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D positionMap;

uniform vec4 vLightPos;

uniform float strength;

out vec4 fragmentColor;

void main() {
    vec4 position = texture(positionMap, passUV);
    vec4 normal =   texture(normalMap, passUV);
    vec4 color =    texture(colorMap, passUV);
    
    if (color.a != 0.0)
    {
        vec3 eye = normalize(-position.xyz);

        //calculate lighting with given position, normal and lightposition
        vec3 nPosToLight = normalize( vLightPos.xyz - position.xyz );

        vec3 nReflection = reflect( -eye, normal.xyz );

        float ambient = 0.1;
        float diffuse = max( dot(normal.xyz, nPosToLight), 0);
        float specular = pow( max( dot( nReflection, nPosToLight ), 0),15);

        // silhouette
        vec2 texStep = vec2(1.0 / 800.0, 1.0 / 600.0);
        float normalDiffX = length( 
              texture(normalMap, passUV + vec2(texStep.x,0.0)) 
            - texture(normalMap, passUV - vec2(texStep.x,0.0)) 
            );
        float normalDiffY = length( 
              texture(normalMap, passUV + vec2(0.0,texStep.y)) 
            - texture(normalMap, passUV - vec2(0.0,texStep.y)) 
        );

        float posDiffX = length( 
              texture(positionMap, passUV + vec2(texStep.x,0.0)) 
            - texture(positionMap, passUV - vec2(texStep.x,0.0)) 
            );
        float posDiffY = length( 
              texture(positionMap, passUV + vec2(0.0,texStep.y)) 
            - texture(positionMap, passUV - vec2(0.0,texStep.y)) 
        );

        float silhouetteInner = 0.0;
        if (  (posDiffX+posDiffY)/2.0 > strength 
            || (posDiffX > strength)
            || (posDiffX > strength) )
        {
            silhouetteInner = 1.0;
        }

        float silhouetteOuter = 0.0;
        if (  (posDiffX+normalDiffY)/2.0 > strength 
            || (posDiffX > strength)
            || (posDiffX > strength) )
        {
            silhouetteOuter = 1.0;
        }

        // depth

        // rim light strength
        float rim = pow( 1.0 - abs(dot(normal.xyz, eye)) , 5);

        fragmentColor = color * ambient 
        + color * diffuse 
        + vec4(1,1,1,1) * specular 
        + vec4(1,1,1,1) * rim
        - vec4(1,1,1,1) * silhouetteInner
        - vec4(1,1,1,1) * silhouetteOuter
        ;
    }
}