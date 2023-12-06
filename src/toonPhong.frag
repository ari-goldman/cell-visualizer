#version 120

// varying vec3 P; // unused
varying vec3 N;
// varying vec3 LightPos; // unused
varying vec3 L;
varying vec3 V;
varying vec4 baseColor;
varying float fogFactor;
uniform sampler2D tex;

void main() {
    float shininess = 10; //gl_FrontMaterial.shininess;
    vec4 specularColor = gl_LightSource[0].specular; //vec4(1,1,1,1);
    vec4 ambientColor = gl_LightSource[0].ambient; //vec4(0.4,0.4,0.4,1);
    vec4 diffuseColor = gl_LightSource[0].diffuse; //vec4(0.5,0.5,0.5,1);
    vec4 rimColor = baseColor * 1.3;
    float rimAmount = 0.7;
    float rimThreshold = 0.2;

    float NdotL = dot(N, L);

    // https://thebookofshaders.com/glossary/?search=smoothstep
    //   smoothstep with small bounds acts like an on/off switch
    //   here it determines whether or not to have diffuse light for toon look
    float diffuseIntensity = smoothstep(0, 0.01, NdotL);
    vec4 diffuse = diffuseIntensity * diffuseColor;
    // gl_FragColor = baseColor * (diffuse + ambientColor);

    vec3 R = reflect(-L, N);
    float specularIntensity = pow(max(dot(R,V),0.0), shininess * shininess);
    specularIntensity = smoothstep(0.005, 0.01, specularIntensity);
    vec4 specular = specularIntensity * specularColor;

    float rimDot = 1 - dot(V,N);
    float rimIntensity = rimDot * pow(NdotL, rimThreshold);
    vec4 rim = max(0,rimIntensity) * rimColor;
    // rim = vec4(0);

    // set final color, factoring in all ambient, diffuse, rim, 
    // but not distance from diffuse, it looks strange for cartoony
    vec4 fragColor = baseColor * (ambientColor + (diffuse + specular) /*/ distance(P,L)*/) + rim;
    fragColor *= texture2D(tex,gl_TexCoord[0].xy);
    gl_FragColor = mix(fragColor, gl_Fog.color, fogFactor);
    // gl_FragColor = vec4(vec3(fogFactor),1);;
    
    

    // gl_FragColor = vec4(specularIntensity);

    // gl_FragColor = vec4(viewDir,1);
}