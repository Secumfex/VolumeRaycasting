#version 430

in vec2 passUV;

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D positionMap;

uniform vec4 vLightPos;

out vec4 fragmentColor;

void main() {
    vec4 position = texture(positionMap, passUV);
    if (position.a == 0.0) { discard; }
    vec4 normal =   texture(normalMap, passUV);
    vec4 color =    texture(colorMap, passUV);
    
    //calculate lighting with given position, normal and lightposition
    vec3 nPosToLight = normalize( vLightPos.xyz - position.xyz );

    vec3 nReflection = normalize( reflect( normalize(position.xyz), normalize(normal.xyz) ) );

    float ambient = 0.1;
    float diffuse = max( dot(normal.xyz, nPosToLight), 0);
    float specular = pow( max( dot( nReflection, nPosToLight ), 0),15);

    // float resX = 1.0/800.0;
    // float resY = 1.0/600.0;

    // int strength = 5;

    // vec4 glow = vec4(0,0,0,1);
    // for(int i = -strength; i < strength; i++) {
    //     for(int j = -strength; j < strength; j++) {
    //         vec4 c = texture(colorMap, passUV + vec2(resX * i, resY * j));
    //         if(c.r + c.g + c.b > 2.0) {                
    //             glow += c;
    //         }
    //     }
    // }
    // glow /= strength * strength * 4;

    fragmentColor = vec4(color.rgb * ambient + color.rgb * diffuse + vec3(specular), color.a);
    
    // fragmentColor += glow;
    // fragmentColor = vec4(nReflection.rgb, 1.0);
}