/*
 *
 *  Ari Goldman
 * 
 *  Colors: https://lospec.com/palette-list/mulfok32
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "CSCIx229.h"
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

typedef struct {float x,y,z;} vtx;

unsigned int texture[8];
int shaders[]  = {0,0,0,0,0,0,0,0,0,0}; //  Shader programs
float screenSize[] = {0.0, 0.0};

int fov = 70;
double asp;
double dim = 2.5;
int mouseX = 0;
int mouseY = 0;

double distance = 3;

int V = 0;

int th=0;          //  Azimuth of view angle
int ph=0;          //  Elevation of view angle
int lth = 0;       //  light angle
int lph = 45;       //  light elevation
int dt = 0;
double t = 0;
double Ex, Ey, Ez;
char pause = 0;

#define NUCLEUS 1
#define MITOCHONDRION 2
#define GOLGI 3
#define ENDOPLASMIC 4
#define VESICLE 5
#define VACUOLE 6
#define RIBOSOME 7

char zoomLevel = 2; // 0 = cell, >0 = organelle  


// float ModelMatrix[16];

/*
  * Map t from [x,y] to [u,v]
  */
double map(double t, double x, double y, double u, double v){
   return (t-x)/(y-x) * (v-u) + u;
}

double lerpd(double a, double b, double t){
   return a + (b-a) * t;
}

vtx lerpvtx(vtx a, vtx b, double t){
   vtx V = {
      a.x + (b.x-a.x) * t,
      a.y + (b.y-a.y) * t,
      a.z + (b.z-a.z) * t
   };

   return V;
}

vtx lerp3d(double x1, double y1, double z1, double x2, double y2, double z2, double t){
   vtx V = {
      x1 + (x2-x1) * t,
      y1 + (y2-y1) * t,
      z1 + (z2-z1) * t
   };

   return V;
}


/*
 * 1 unit diameter sphere at 0, 0, 0
 * Translated by x, y, z
 * Scaled by scale in each direction
 */
void sphere(int partitions, double x, double y, double z, double scale){
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(0.5,0.5,0.5); // to make it 1 in diameter as opposed to radius
   glScaled(scale, scale, scale);
   
   int dth = 360 / partitions;
   dth = 10;
   for(int ph = -90; ph <= 90; ph += dth){
      glBegin(GL_QUAD_STRIP);
      for(int th = 0; th <= 360; th += dth){
         x = Sin(th) * Cos(ph);
         y = Sin(ph);
         z = Cos(th) * Cos(ph);
         glNormal3d(x, y, z);
         glVertex3d(x, y, z);
         x = Sin(th) * Cos(ph + dth);
         y = Sin(ph + dth);
         z = Cos(th) * Cos(ph + dth);
         glNormal3d(x, y, z);
         glVertex3d(x, y, z);
      }
      glEnd();
   }

   glPopMatrix();
}

/*
 * Unit cylinder centered at 0, 0, 0 along Y-axes
 * Translated by (x, y, z)
 * Rotated thx, thy, thz about x, y, z axes
 * Scaled by sx, sy, sz along x, y , z axes
 */
void cylinder(int partitions, double x, double y, double z, double thx, double thy, double thz, double sx, double sy, double sz){
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(thz, 0, 0, 1);
   glRotated(thy, 0, 1, 0);
   glRotated(thx, 1, 0, 0);
   glScaled(sx, sy, sz);


   int dth = 360 / partitions;
   dth = 10;
   glBegin(GL_QUAD_STRIP);

   // sides
   for(int th = 0; th <= 360; th += dth){
      x = Cos(th) / 2;
      z = Sin(th) / 2;
      glNormal3d(x, 0, z);
      glVertex3d(x, -0.5, z);
      glNormal3d(x, 0, z);
      glVertex3d(x, +0.5, z);
   }
   glEnd();


   // bottom cap
   glBegin(GL_POLYGON);
   for(int th = 0; th <= 360; th += dth){
      x = Cos(th) / 2;
      z = Sin(th) / 2;
      glNormal3d(0, -1, 0);
      glVertex3d(x, -0.5, z);
   }
   glEnd();


   // top cap
   glBegin(GL_POLYGON);
   for(int th = 0; th <= 360; th += dth){
      x = Cos(th) / 2;
      z = Sin(th) / 2;
      glNormal3d(0, +1, 0);
      glVertex3d(x, +0.5, z);
   }
   glEnd();

   glPopMatrix();

}

/*
 * Sets the current color based on the given base
 * Bases are chars 0, 1, 2, 3
 */
void colorBase(char base){
   // 0 = Adenine = Green
   // 1 = Cytosine = Red
   // 2 = Thymine = Purple
   // 3 = Guanine = Blue
   // 0 + 2, 1 + 3
   if(base == 0){
      glColor3d(0.6705882352941176, 0.8666666666666667, 0.39215686274509803);
   }else if(base == 1){
      glColor3d(0.9176470588235294, 0.3843137254901961, 0.3843137254901961);
   }else if(base == 2){
      glColor3d(0.5137254901960784, 0.30196078431372547, 0.7686274509803922);
   }else if(base == 3){
      glColor3d(0.42745098039215684, 0.5019607843137255, 0.9803921568627451);
   }else{
      glColor3d(1,1,1);
   }
}

/*
 *  Draw a DNA molecule
 * (unused, but was foundation for RNA)
 */
char bases[] = {3, 2, 0, 2, 1, 3, 2, 0, 0, 0, 3, 3, 0, 1, 2, 1, 2, 3, 3, 3, 2, 3, 2, 0, 1, 1, 0, 0, 2, 0, 1, 3, 0, 1, 1, 3, 2, 0, 2, 2, 3, 3, 0, 3, 0, 2, 1, 2, 2, 2, 0, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 2, 1, 0, 0, 2, 1, 1, 2, 2, 1, 2, 1, 2, 1, 1, 3, 3, 3, 3, 1, 3, 3, 2, 2, 0, 3, 1, 1, 0, 2, 1, 1, 3, 2, 1, 3, 3, 3, 0}; // len = 100
void DNA(int pairs){
   glPushMatrix();
   glTranslated(0, -pairs / 2.0 * 0.2, 0);
   float diffuse[] = {0.553, 0.718, 1, 1};

   int partitions = 10;
   int base = 0;
   for(int i = 0; i < pairs; i++){
      glColor3d(0.427, 0.502, 0.98);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
      sphere(partitions, -0.375, 0, 0, 0.25);
      sphere(partitions, +0.375, 0, 0, 0.25);
      colorBase(bases[base]);
      cylinder(partitions, -0.15, 0, 0, 0, 0, 90, 0.1, 0.25, 0.1);
      colorBase((bases[base] + 2) % 4);
      cylinder(partitions, +0.15, 0, 0, 0, 0, 90, 0.1, 0.25, 0.1);
      glTranslated(0,0.2,0);
      glRotated(15, 0, 1, 0);
      base = (base + 1) % 100;
   }
   glPopMatrix();
}


/*
 * Draw RNA for the ribosome molecule
 *   animated to cycle 3 bases every other second
 */
void RibosomeRNA(int pairs){
   glPushMatrix(); // 1+
   glRotated(90,0,0,1);

   double length = pairs * 0.2;
   int partitions = 10; 

   // calculate starting base from time
   int base = (t + 1) / 2;
   base *= 3;
   base = 100 - (base % 100);

   // translate to center at 0 and move by animation
   glTranslated(0,length / -2 + 0.1, 0);
   double dy = fmod(t, 2) < 1 ? (1.5 - 1.5 * cos(M_PI * t)) * 0.2 : 0;
   glTranslated(0, dy, 0);

   // draw the strand with "pairs" number of bases
   for(int i = 0; i < pairs; i++){
      // calculate the signed distance from the center
      float distFromCenter = (length / -2 + 0.1) + (0.2 * i) + dy;
      distFromCenter = distFromCenter * distFromCenter * 10 * (distFromCenter < 0 ? -1 : 1);

      base %= 100;

      glPushMatrix(); // 2+
      glRotatef(distFromCenter,0,1,0); // get the twisting effect - bases near origin should be flat
      glColor3d(0.6823529411764706, 0.8862745098039215, 1); // outside color
      sphere(partitions, -0.375, 0, 0, 0.25); 
      colorBase(bases[base]); // set appropriate base color
      cylinder(partitions, -0.15, 0, 0, 0, 0, 90, 0.1, 0.25, 0.1);
      glPopMatrix(); // 2-

      glTranslated(0,0.2,0); // move to next base in strand
      base++; // move to next base color
   }

   glPopMatrix(); // 1-
}

/*
 * Draw the protein coming out of the ribosome
 *  animated to output an acid every other second
 */
void RibosomeProtein(){
   // glUseProgram(shaders[0]);

   glColor3f(1, 0.722, 0.475);
   int partitions = 20;
   int acids = t > 200 ? 100 : t / 2 + 1;
   glPushMatrix(); // 1+
   glTranslated(0,0.2,0);
   double dy = fmod(t, 2) > 1 ? (1 + cos(M_PI * t)) * 0.15 : 0;
   //  dy += 0.3 * (acids-1);
   glTranslated(0,dy,0);
   for(int i = 0; i < acids; i++){
      sphere(partitions,0,0,0,0.35);
      glTranslated(0,0.3,0);
      // glRotated(1,0,1,1);
      // glRotated((4 * acids - 4*i),0,0,1);
   }
   glPopMatrix(); // 1-

   glUseProgram(0);
}

/*
  * Draw a simple ribosome scene
  */
void Ribosome(){
   glUseProgram(shaders[0]);

   glColor3f(0.4,0.1,0.3);
   glPushMatrix(); // 1+
   glScalef(0.7,0.8,.65);
   sphere(100,0,0,0,1);
   glPopMatrix(); // 1-


   glPushMatrix(); // 1+
   glTranslated(0,-0.35,0);
   glScalef(0.8,0.5,0.5);
   sphere(100,0,0,0,1);
   glPopMatrix(); // 1-

   if(zoomLevel == RIBOSOME){
      RibosomeRNA(100);
      RibosomeProtein();
   }

   glUseProgram(0);
   glDisable(GL_FOG);
}

/*
 * Recursively draws a lightning bolt
 * length is how many recursions
 */
void lightning(int length){
   if(length == 0) return;
   glPushMatrix();
   cylinder(10,0,0.25,0,0,0,0,0.1,1,0.1);
   glTranslated(0,0.5,0);
   glRotated(rand() % 70 - 35, 0, 0, 1);
   glRotated(rand() % 70 - 35, 1, 0, 0);
   glScaled(1,0.5,1);
   lightning(length - 1);
   if(rand() % 10 > 7){
      glRotated(rand() % 70 - 35, 0, 0, 1);
      glRotated(rand() % 70 - 35, 1, 0, 0);
      lightning(length);
   }
   glPopMatrix();
}

/*
  * Create a lightning bolt at a "random" spot on the mitochondrion
  */
void instantiateLightning(){
   if(fmod(t * (sin(t * 1.830181) + 1),1) >= 0.9){
      float lightPos[] = {0,0,0,1};
      double rando = fmod(t * 1.64057185, 1); //fmod(t * 1.9819411952,1);
      lightPos[0] = Cos(rando * 180);
      lightPos[1] = Sin(rando * 180);
      lightPos[2] = rando - 0.5;

      glPushMatrix();
      glTranslated(-0.5 * lightPos[0], -0.5 * lightPos[1], lightPos[2]);
      glRotated(rando * 180 + 90,0,0,1);
      glScaled(0.4,0.4,0.4);
      lightning(rand() % 15);
      glPopMatrix();

      lightPos[0] *= -4;
      lightPos[1] *= -4;
      lightPos[2] = rando - 0.5;
      glLightfv(GL_LIGHT0,GL_POSITION, lightPos);
   }
}

/*
  * Draw the mitochondrion scene
  */
void Mitochondrion(){
   int partitions = 36;
   double dth = 360 / partitions;
   dth = 10; // just hard-set it to avoid problems 
   double x, y, z;

   glEnable(GL_FOG);
   glFogi(GL_FOG_MODE, GL_LINEAR);
   // float fogColor[] = {0.682, 0.886, 1, 1};
   // glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
   // glFogfv(GL_FOG_COLOR, fogColor);
   glFogf(GL_FOG_START,6);
   glFogf(GL_FOG_END,10);

   float lightPos[] = {0,2,0,1};

   float dark[] = {0.1,0.1,0.1,1};
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  dark);
   glLightfv(GL_LIGHT0, GL_SPECULAR, dark);
   glLightfv(GL_LIGHT0,GL_POSITION, lightPos);

   glUseProgram(shaders[0]);
   glPushMatrix(); // 1+
   glColor3d(0.988, 0.937, 0.553);

   double insideScale = 0.75; // scale of the inside of the organelle "shell"

   // inner textured plane
   glPushMatrix(); // 2+
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[0]);

   glScaled(0.357,1,0.4);
   glBegin(GL_POLYGON);
   glNormal3d(0,1,0);
   for(int th = 0; th <= 180; th += dth){
      double x = Cos(th);
      double z = Sin(th);
      glTexCoord2d(x / 4 + 0.48, z / 4 + 0.75); glVertex3d(x, -0.1, z + 1);
   }
   for(int th = 180; th <= 360; th += dth){
      double x = Cos(th);
      double z = Sin(th);
      glTexCoord2d(x / 4 + 0.48, z / 4 + 0.25); glVertex3d(x, -0.1, z - 1);
   }
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix(); // 2-


   // shell top vertical lines
   glBegin(GL_QUADS);
   glNormal3d(0,1,0);
   glVertex3d(-0.5,0,-0.5);
   glVertex3d(-0.5 * insideScale,0,-0.5);
   glVertex3d(-0.5 * insideScale,0,0.5);
   glVertex3d(-0.5,0,0.5);
   glVertex3d(0.5,0,-0.5);
   glVertex3d(0.5 * insideScale,0,-0.5);
   glVertex3d(0.5 * insideScale,0,0.5);
   glVertex3d(0.5,0,0.5);
   glEnd();


   glPushMatrix(); // 2+
   glTranslated(0,0,-0.5);
   glScaled(0.5,0.5,0.5);
   for(int i = 0; i < 2; i++){
      glBegin(GL_QUAD_STRIP);
      glNormal3d(0,1,0);
      for(int th = 90; th <= 270; th += dth){
         x = Sin(th);
         z = Cos(th);
         glVertex3d(x, 0, z);
         x *= insideScale;
         z *= insideScale;
         glVertex3d(x, 0, z);
      }
      glEnd();
      glRotated(180,0,1,0);
      glTranslated(0,0,-2);
   }
   glPopMatrix(); // 2-

   for(int i = 0; i < 2; i++){
      char normalDir = i == 1 ? 1 : -1;
      double scale = i == 1 ? 1 : insideScale;

      glPushMatrix(); // 2+
      glTranslated(0,0,-0.5);
      glScaled(0.5,0.5,0.5);
      glScaled(scale,scale,scale);
      for(int ph = -90; ph < 0; ph += dth){
         glBegin(GL_QUAD_STRIP);
         for(int th = 90; th <= 270; th += dth){
            x = Sin(th) * Cos(ph);
            y = Sin(ph);
            z = Cos(th) * Cos(ph);
            glNormal3d(x*normalDir, y*normalDir, z*normalDir);
            glVertex3d(x, y, z);
            x = Sin(th) * Cos(ph + dth);
            y = Sin(ph + dth);
            z = Cos(th) * Cos(ph + dth);
            glNormal3d(x*normalDir, y*normalDir, z*normalDir);
            glVertex3d(x, y, z);
         }
         glEnd();
      }
      glPopMatrix(); // 2-

      glPushMatrix(); // 2+
      glTranslated(0,0,0.5);
      glScaled(0.5,0.5,0.5);
      glScaled(scale,scale,scale);
      for(int ph = -90; ph < 0; ph += dth){
         glBegin(GL_QUAD_STRIP);
         for(int th = 270; th <= 450; th += dth){
            x = Sin(th) * Cos(ph);
            y = Sin(ph);
            z = Cos(th) * Cos(ph);
            glNormal3d(x*normalDir, y*normalDir, z*normalDir);
            glVertex3d(x, y, z);
            x = Sin(th) * Cos(ph + dth);
            y = Sin(ph + dth);
            z = Cos(th) * Cos(ph + dth);
            glNormal3d(x*normalDir, y*normalDir, z*normalDir);
            glVertex3d(x, y, z);
         }
         glEnd();
      }
      glPopMatrix(); // 2-

      glPushMatrix(); // 2+
      glScaled(scale, scale, 1);
      glBegin(GL_QUAD_STRIP);
      for(int th = 180; th <= 360; th += dth){
         x = Cos(th) / 2;
         z = Sin(th) / 2;
         glNormal3d(x*normalDir, z*normalDir, 0);
         glVertex3d(x, z, -0.5);
         glNormal3d(x*normalDir, z*normalDir, 0);
         glVertex3d(x, z, +0.5);
      }
      glEnd();
      glPopMatrix(); // 2-

      glUseProgram(shaders[1]);
      // create the lightning such that it only effects the bottom of the mitochondrion
      instantiateLightning();
      glUseProgram(shaders[0]);

   }

   glUseProgram(0);
   glDisable(GL_FOG);
   glEnable(GL_LIGHT0);

   glPopMatrix(); // 1-
}

void Nucleus(){}

void Golgi(){

   // enable raymarching and draw the quad
   glUseProgram(shaders[2]);
   glUniform2f(glGetUniformLocation(shaders[2],"screenSize"), screenSize[0], screenSize[1]);
   glUniform1f(glGetUniformLocation(shaders[2],"t"), t);
   glUniform3f(glGetUniformLocation(shaders[2],"cameraPos"), Ex, Ey, Ez);
   glUniform2f(glGetUniformLocation(shaders[2],"th_ph"), th * M_PI / 180, ph * M_PI / 180);
   glPushAttrib(GL_FOG_BIT);
   glFogf(GL_FOG_START,5);
   glFogf(GL_FOG_END,6);
   glPushMatrix();
   glLoadIdentity();
   glBegin(GL_QUADS);
   glVertex3d(-1,-1,0);
   glVertex3d(1,-1,0);
   glVertex3d(1,1,0);
   glVertex3d(-1,1,0);
   glEnd();
   glPopMatrix();
   glPopAttrib();
   glUseProgram(0);
}

void Endoplasmic(){}

void Vesicle(){}

void Vacuole(){}

void Membrane(){}




/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   // glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();

   //  3rd person perspective
   Ex = -2*dim*Sin(th)*Cos(ph);
   Ey = +2*dim        *Sin(ph);
   Ez = +2*dim*Cos(th)*Cos(ph);
   gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);


   int ambient = 40;
   int diffuse = 50;
   int specular = 100;
   // float distance = 3;
   float lightPos[] = {0,1,1,1}; //{distance*Cos(lth),distance * Sin(lph),distance*Sin(lth),1.0};
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT, GL_SHININESS, 100); // doesn't work?
   // if(zoomLevel != MITOCHONDRION) sphere(10, lightPos[0], lightPos[1], lightPos[2], 0.1);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_NORMALIZE);
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   glLightfv(GL_LIGHT0, GL_AMBIENT,  Ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  Diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, 15);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION, black);

   glEnable(GL_FOG);
   glFogi(GL_FOG_MODE, GL_LINEAR);
   float fogColor[] = {0.682, 0.886, 1, 1};
   glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
   glFogfv(GL_FOG_COLOR, fogColor);
   glFogf(GL_FOG_DENSITY, 0.1);
   glFogf(GL_FOG_START,7);
   glFogf(GL_FOG_END,11);

   glPushMatrix();
   // glScalef(0.5,0.5,0.5);
   // Mitochondria();
   switch(zoomLevel){
      case NUCLEUS:
         Nucleus();
         break;
      case MITOCHONDRION:
         Mitochondrion();
         break;
      case GOLGI:
         Golgi();
         break;
      case ENDOPLASMIC:
         Endoplasmic();
         break;
      case VESICLE:
         Vesicle();
         break;
      case VACUOLE:
         Vacuole();
         break;
      case RIBOSOME:
         Ribosome();
         break;
      default:
         break;
   }

   glPopMatrix();

   glDisable(GL_LIGHTING);

   // glWindowPos2i(10,10);

   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void Projection(){
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if(zoomLevel != GOLGI){
      gluPerspective(fov, asp, dim / 16, dim*8);
   }else{
      // for raymarching, don't want perspective on the quad
      glOrtho(-1,1,-1,1,-1,1);
   }
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   else if (ch == 'w' || ch == 'W')
      t += (10 + (990 * (ch == 'W'))) / 1000.0; // time control (shift W = +1 second) // overly complicated
   else if (ch == 's' || ch == 'S')
      t -= (10 + (990 * (ch == 'S'))) / 1000.0; // time control (shift S = -1 second)
   else if (ch == ' ')
      pause = !pause;
   else if ('1' <= ch && ch <= '8' && zoomLevel >= 0){
      switch (ch )
      {
      case '1':
         t = zoomLevel != 2 ? 2 : t; // reset time if changing scenes
         zoomLevel = 2;
         break;
      case '2':
         t = zoomLevel != 3 ? 0 : t; // reset time if changing scenes
         zoomLevel = 3;
         break;
      case '3':
         t = zoomLevel != 7 ? 0 : t; // reset time if changing scenes
         zoomLevel = 7;
         break;
      
      default:
         break;
      }
   }
   
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}


/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   screenSize[0] = RES*height;
   screenSize[1] = RES*width;
   // printf("%f %f\n", screenSize[0], screenSize[1]);
   Projection();
}

/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
         // adding deltaTimes to t allows for time manipulation
   if(!pause) t += (glutGet(GLUT_ELAPSED_TIME) - dt)/1000.0;
   lth = fmod(90*t,360);
   lph = Sin(fmod(73*t,360)) * 45;
   glutPostRedisplay();
   dt = glutGet(GLUT_ELAPSED_TIME);
}

void mouseClickMove(int x, int y){
   if(!(mouseX & mouseY)){
      mouseX = x;
      mouseY = y;
   }
   th = (th + (x - mouseX)) % 360;
   ph = (ph + (y - mouseY)) % 360;
   ph = ph < -90 ? -90 : (ph > 90 ? 90 : ph);
   mouseX = x;
   mouseY = y;
}

void mouseMove(int x, int y){
   mouseX = x;
   mouseY = y;
}

/*
 *  Read text file
 */
char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
void PrintShaderLog(int obj,char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{


   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(900,900);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("final");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);

   shaders[0] = CreateShaderProg("toonPhong.vert", "toonPhong.frag");
   shaders[1] = CreateShaderProg("lightning.vert", "lightning.frag");
   shaders[2] = CreateShaderProg("raymarch.vert", "raymarch.frag" );

   texture[0] = LoadTexBMP("innerMitochondrion.bmp");

   glutMotionFunc(mouseClickMove);
   glutPassiveMotionFunc(mouseMove);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
