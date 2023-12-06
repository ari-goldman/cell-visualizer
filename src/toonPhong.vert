#version 120

// varying vec3 P; // eye coordinates of vertex // unused
varying vec3 N; // normal to vertex
// varying vec3 LightPos; // position of light zero // unused
varying vec3 L; // direction to light from vertex
varying vec3 V; // view vector
varying vec4 baseColor; // just the color specified for the object
varying float fogFactor;

void main() {
    vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);

    N = normalize(gl_NormalMatrix * gl_Normal);

    vec3 LightPos = vec3(gl_LightSource[0].position);

    L = normalize(LightPos - P);

    V = normalize(-P);

    baseColor = gl_Color;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    gl_FogFragCoord = length(P);
    fogFactor = (gl_FogFragCoord - gl_Fog.start) / (gl_Fog.end - gl_Fog.start); // linear fog to allow for start/end easily
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    gl_TexCoord[0] = gl_MultiTexCoord0; 
}