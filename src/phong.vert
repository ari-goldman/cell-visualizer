//  Phong lighting
#version 120

void main()
{

   vec4 vert = gl_Vertex;

   //  P is the vertex coordinate on body
   vec3 P = vec3(gl_ModelViewMatrix * vert);
   // vert += vec4(0,0,length(P),0);
   //  N is the object normal at P
   vec3 N = normalize(gl_NormalMatrix * gl_Normal);
   //  Light Position for light 0
   vec3 LightPos = vec3(gl_LightSource[0].position);
   //  L is the light vector
   vec3 L = normalize(LightPos - P);
   //  R is the reflected light vector R = 2(L.N)N - L
   vec3 R = reflect(-L, N);
   //  V is the view vector (eye at the origin)
   vec3 V = normalize(-P);

   //  Diffuse light intensity is cosine of light and normal vectors
   float Id = max(dot(L,N) , 0.0);
   //  Shininess intensity is cosine of light and reflection vectors to a power
   float Is = (Id>0.0) ? pow(max(dot(R,V) , 0.0) , gl_FrontMaterial.shininess) : 0.0;

   //  Vertex color
   gl_FrontColor = gl_FrontLightProduct[0].ambient
              + Id*gl_FrontLightProduct[0].diffuse
              + Is*gl_FrontLightProduct[0].specular;


   //  Return fixed transform coordinates for this vertex
   gl_Position = gl_ModelViewProjectionMatrix * vert;
   // gl_FrontColor = vec4((gl_ModelViewMatrix * gl_Vertex).xyz,1);
}
