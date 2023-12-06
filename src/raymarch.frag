#version 120

#define PI 3.1415926538
#define PI_2 1.5707963268
#define identity3 mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0)
#define SQUARE(x) (x * x)

uniform float t;
varying vec3 rayOrigin;
uniform vec2 th_ph;

varying vec2 uv;


mat3 rotX(in float th){
    return mat3(
        1.0, 0.0, 0.0,
        0.0, cos(th), sin(th),
        0.0, -sin(th), cos(th)
    );
}

mat3 rotY(in float th){
    return mat3(
        cos(th), 0.0, -sin(th),
        0.0, 1.0, 0.0,
        sin(th), 0.0, cos(th)
    );
}

mat3 rotZ(in float th){
    return mat3(
        cos(th), sin(th), 0.0,
        -sin(th), cos(th), 0.0,
        0.0, 0.0, 1.0
    );
}


float unionSDF(in float a, in float b){
    return min(a, b);
}

float smoothUnionSDF(float a, float b, float k ) {
  float h = clamp(0.5 + 0.5*(a-b)/k, 0.0, 1.0);
  return mix(a, b, h) - k*h*(1.-h); 
}

float differenceSDF(in float a, in float b){
    return max(a, -b);
}

float smoothDifferenceSDF(in float a, in float b, in float k){
    float h = clamp(0.5 - 0.5 * (a + b) / k, 0.0, 1.0);
    return mix(a, -b, h) - k * h * (1.0 - h);
}

float intersectSDF(float a, float b){
    return max(a, b);
}

float smoothIntersectSDF(float distA, float distB, float k ) {
  float h = clamp(0.5 - 0.5*(distA-distB)/k, 0., 1.);
  return mix(distA, distB, h ) + k*h*(1.-h); 
}

// sphere SDF
float Sphere(in vec3 pos, in vec3 center, in float radius){
    return length(pos - center) - radius;
}

float Box( vec3 p, vec3 b ){
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdBoxFrame( vec3 p, vec3 b, float e ){
       p = abs(p  )-b;
  vec3 q = abs(p+e)-e;
  return min(min(
      length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
      length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
      length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

float sdTorus( vec3 p, vec2 t ){
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float sdPlane( vec3 p, vec3 n, float h ){
  // n must be normalized
  return dot(p,n) + h;
}

float cylinder(vec3 position, float radius, float halfHeight, float cornerRadius) {
   vec2 d = vec2(length(position.xz), abs(position.y)) - vec2(radius, halfHeight) + cornerRadius; 
   return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - cornerRadius;
}

float shellify(in float sd, in float thickness){
    return abs(sd) - thickness;
}

float DistToWorld(in vec3 pos){
    float time = t / 1000.0;
    // float displacement = sin(5.0 * pos.x + th_ph.x) * sin(5.0 * pos.y + th_ph.x) * sin(5.0 * pos.z + th_ph.y) * 0.2
    float result = 1;
    float viscosity = 0.1;
    
    float thickness = 0.01;


    float disc_1 = cylinder(rotX(PI_2) * pos + vec3(0.0, SQUARE(length(pos.xy * 2))   + 3.6, 0.0) / 4, 0.5, 0.1, 0.1);
    float disc_2 = cylinder(rotX(PI_2) * pos + vec3(0.0, SQUARE(length(pos.xy * 1.3)) + 2.4, 0.0) / 4, 0.9, 0.1, 0.1);
    float disc_3 = cylinder(rotX(PI_2) * pos + vec3(0.0, SQUARE(length(pos.xy * 0.8)) + 1.2, 0.0) / 4, 1.4, 0.1, 0.1);
    float disc_4 = cylinder(rotX(PI_2) * pos + vec3(0.0, SQUARE(length(pos.xy * 0.5)) + 0.0, 0.0) / 4, 1.8, 0.1, 0.1);
    float disc_5 = cylinder(rotX(PI_2) * pos - vec3(0.0, SQUARE(length(pos.xy * 0.4)) + 1.2, 0.0) / 4, 1.6, 0.1, 0.1);
    float disc_6 = cylinder(rotX(PI_2) * pos - vec3(0.0, SQUARE(length(pos.xy * 0.9)) + 2.4, 0.0) / 4, 0.9, 0.1, 0.1);
    float disc_7 = cylinder(rotX(PI_2) * pos - vec3(0.0, SQUARE(length(pos.xy * 1.3)) + 3.6, 0.0) / 4, 0.4, 0.1, 0.1);

    float vesicle = Sphere(pos, vec3(0.0, -0.1, mod(t, 5.0) - 2.5), 0.1);

    float boundingBox = Box(pos, vec3(5, 5, 2));
    
    // result = unionSDF(result, shellify(disc_1, thickness));
    // result = unionSDF(result, shellify(disc_2, thickness));
    // result = unionSDF(result, shellify(disc_3, thickness));
    // result = unionSDF(result, shellify(disc_4, thickness));

    // result = smoothUnionSDF(result, shellify(vesicle, thickness), viscosity);

    // result = differenceSDF(result, disc_1);
    // result = differenceSDF(result, disc_2);
    // result = differenceSDF(result, disc_3);
    // result = differenceSDF(result, disc_4);

    // result = differenceSDF(result, vesicle);

    result = unionSDF(result, disc_1);
    result = unionSDF(result, disc_2);
    result = unionSDF(result, disc_3);
    result = unionSDF(result, disc_4);
    result = unionSDF(result, disc_5);
    result = unionSDF(result, disc_6);
    result = unionSDF(result, disc_7);

    result = smoothUnionSDF(result, vesicle, viscosity);


    float unshelled = result;
    result = shellify(result, thickness);


    result = differenceSDF(result, unshelled);

    result = intersectSDF(result, sdPlane(pos, vec3(0,1,0), 0));

    result = intersectSDF(result, boundingBox);

    return result;
}

vec3 CalcNormal(in vec3 pos){
    const vec3 SMALL_STEP = vec3(0.0001, 0.0, 0.0);

    // thic calculates the gradient which thus is the normal!
    float _dX = DistToWorld(pos + SMALL_STEP.xyy) - DistToWorld(pos - SMALL_STEP.xyy);
    float _dY = DistToWorld(pos + SMALL_STEP.yxy) - DistToWorld(pos - SMALL_STEP.yxy);
    float _dZ = DistToWorld(pos + SMALL_STEP.yyx) - DistToWorld(pos - SMALL_STEP.yyx);

    return normalize(vec3(_dX, _dY, _dZ));
}

vec3 RayMarch(in vec3 rayOrigin, in vec3 rayDirection){
    float distTraveled = 0.0;
    const int MAX_STEPS = 80;
    const float HIT_DISTANCE = 0.001;
    const float MAX_RAY_LENGTH = 1000.0;


    for(int i = 0; i < MAX_STEPS; i++){
        vec3 currentPos = rayOrigin + rayDirection * distTraveled;

        float distToClosest = DistToWorld(currentPos);

        if(abs(distToClosest)  <= HIT_DISTANCE){
            vec3 normal = CalcNormal(currentPos);

            vec3 lightPos = vec3(2.0,-5.0,3.0);
            vec3 toLight = normalize(currentPos - lightPos);

            float diffuseIntensity = max(0.1, dot(normal, toLight));


            return vec3(1.0, 0.5, 0.5) * diffuseIntensity;
        }

        if(distTraveled > MAX_RAY_LENGTH){
            break;
        }

        distTraveled += distToClosest * 0.5; // move the ray forward by the distance to closest object
    }

    return vec3(0.0);
    // return cameraPos;
}

mat3 lookAtMatrix(in vec3 eye, in vec3 center, in vec3 up){
    vec3 F = center - eye;
    F = F / F;
    up = up / up;

    vec3 s = cross(F, up);
    vec3 u = cross(s/s, up);
    mat3 matrix = mat3(
        s.x, u.x, -F.x, // column 1
        s.y, u.y, -F.y, // column 2
        s.z, u.z, -F.z // column 3
    );
    return matrix;
}

void main(){
    // uv coordinates from -1 to 1 
    // vec2 uv = 2 * gl_FragCoord.xy / screenSize.yx - 1;
    // uv.x = min(uv.x,)
    float th = th_ph.x;
    float ph = th_ph.y;
    float cameraDist = 3;

    // vec3 rayOrigin = vec3(-cameraDist * sin(th) * cos(ph), cameraDist * sin(ph), -cameraDist * cos(th) * cos(ph));
    // rayOrigin = vec3(0.0,0.0,-5.0);
    // vec3 toOrigin = normalize(-1 * cameraPos);

    vec3 rayDirection = vec3(uv,1.0);
    // mat3 mat = lookAtMatrix(normalize(cameraPos), vec3(0.0), vec3(0.0,1.0,0.0));
    // rayDirection = mat * rayDirection;


    // mat3 rotX = mat3(
    //     1, 0, 0,
    //     0, cos(ph), sin(ph),
    //     0, -sin(ph), cos(ph)
    // );

    rayDirection = rotY(th) * rotX(ph) * rayDirection;
    rayDirection = normalize(rayDirection);

    vec3 color = RayMarch(rayOrigin, rayDirection);
    // gl_FragColor = vec4(uv,0,1);
    // gl_FragColor = vec4(rayDirection, 1.0);
    gl_FragColor = vec4(color, 1.0);
}


