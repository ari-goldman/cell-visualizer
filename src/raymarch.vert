#version 120

uniform vec2 screenSize;
varying vec2 uv;

varying vec3 rayOrigin;
uniform vec2 th_ph;

void main(){
    gl_Position = gl_Vertex;

    // set the uv coords at the corners
    // and make the smallest dimension from -1 to 1, and the larges can go beyond to keep aspect ratio
    uv = gl_Vertex.xy;
    uv = uv * screenSize.yx / min(screenSize.x, screenSize.y);

    float th = th_ph.x;
    float ph = th_ph.y;
    float cameraDist = 3;

    rayOrigin = vec3(-cameraDist * sin(th) * cos(ph), cameraDist * sin(ph), -cameraDist * cos(th) * cos(ph));
}

