/*
 * Final Project: Star Wars IV Death Star Trench Scene
 * Christopher Jordan and Jeremy Granger
 * CSCI 4229 Fall 2014
 *
 *
 *  Key bindings:
 */
#include "CSCIx229.h"
#include <math.h>
 #include <stdlib.h>
#include <time.h>

// World variables
int mode=0;       //  Texture mode
int view=1;       //  Projection mode
int move=0;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5000.0;   //  Size of world

// Texture array
unsigned int texture[18]; // Texture names
GLuint cockpitTex;
GLuint space[6];
GLuint trenchTex[6];
GLuint laser[2];

// Light values
int one       =   1;  // Unit value
int distance  =  50;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

int camera = 1;       // Camera selector
int trenchAnim = 0;   // Used to for trench animation (distance variable)
int laserAnim = 0;
int pause = 0;        // Pauses animations
int laserDelay = 500;
double rotateZ = 0;

int laserFired = 0;
// Xwing positioning
int xwing_x = 0;
int xwing_y = 0;
int xwing_z = 10;
int xwing_r = -15;
int xwing_d = 1;

// For idle function
unsigned long ot;
unsigned long dt;

// For initclock
int first=0;



/*
 *  Determine normal for triangle using the first point as
 *  an origin for the vectors that go to points 2 and 3
 *     
 */
 static void normal(double x1, double y1, double z1,
                  double x2, double y2, double z2,
                  double x3, double y3, double z3){
   // Build first vector
   double v1x = x2 - x1;
   double v1y = y2 - y1;
   double v1z = z2 - z1;

   // Build second vector
   double v2x = x3 - x1;
   double v2y = y3 - y1;
   double v2z = z3 - z1;

   // Get Cross Product
   double nx = (v1y*v2z) - (v1z*v2y);
   double ny = (v1z*v2x) - (v1x*v2z);
   double nz = (v1x*v2y) - (v1y*v2x);

   // Set normal for trianlge plane
   glNormal3f(nx,ny,nz);
 }

 /*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

// Draw skybox
static void skybox(double D)
{
   glPushMatrix();
   glRotated(180, 0, 1, 0);
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   
   //  Sides
   glBindTexture(GL_TEXTURE_2D,space[0]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0,1); glVertex3f(-D,+D,-D);
   glEnd();
   
   
   glBindTexture(GL_TEXTURE_2D,space[1]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(1,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0,1); glVertex3f(+D,+D,-D);
   glEnd();
   
   
   glBindTexture(GL_TEXTURE_2D,space[2]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(1,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0,1); glVertex3f(+D,+D,+D);
   glEnd();
   
   
   glBindTexture(GL_TEXTURE_2D,space[3]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0,1); glVertex3f(-D,+D,+D);
   glEnd();
   
   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,space[4]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(1,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(1,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0,1); glVertex3f(-D,+D,-D);
   glEnd();
   
   
   glBindTexture(GL_TEXTURE_2D,space[5]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(1,1); glVertex3f(+D,-D,-D);
   glTexCoord2f(0,1); glVertex3f(-D,-D,-D);
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

static void vader(double x,double y,double z,double r, double angle, double rx, double ry, double rz)
{
   int d = 5;
   int th, ph;
   double color = 0.1;
   
   //  Save transformation
   glPushMatrix();
   
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   glRotated(angle, rx, ry, rz);
   
  
   
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, cockpitTex);
   
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
        glTexCoord2d(th/360.0,ph/180.0+0.5);
        Vertex(th,ph);
        glTexCoord2d(th/360.0,(ph+inc)/180.0+0.5);
        Vertex(th,ph+inc);
      }
      glEnd();
   }

   // Disable Textures
   glDisable(GL_TEXTURE_2D);
   
   /* Front windscreen */
   glBegin(GL_TRIANGLE_FAN);
   glColor3d(0.5, 0.5, 0.5);
   glNormal3d(0, 0, 1);
   glVertex3d(0, 0, 1);
   
   for (th = 0; th <= 360; th += 45)
   {
      glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 1);
      
   }
   glEnd();
   
   // Octa-windows, outside
   glColor3d(0, 0, 0);

   glBegin(GL_QUADS);
   normal(0.2 * Cos(3),0.2 * Sin(3),1.001, 0.47 * Cos(3),0.47 * Sin(3),1.001, 0.47 * Cos(42),0.47 * Sin(42),1.001);
   glVertex3d(0.2 * Cos(3), 0.2 * Sin(3), 1.001);
   glVertex3d(0.47 * Cos(3), 0.47 * Sin(3), 1.001);
   glVertex3d(0.47 * Cos(42), 0.47 * Sin(42), 1.001);
   glVertex3d(0.2 * Cos(42), 0.2 * Sin(42), 1.001);
   glEnd();
   
   glBegin(GL_QUADS);
   normal(0.2 * Cos(48),0.2 * Sin(48),1.001, 0.47 * Cos(48),0.47 * Sin(48),1.001, 0.47 * Cos(87),0.47 * Sin(87),1.001);
   glVertex3d(0.2 * Cos(48), 0.2 * Sin(48), 1.001);
   glVertex3d(0.47 * Cos(48), 0.47 * Sin(48), 1.001);
   glVertex3d(0.47 * Cos(87), 0.47 * Sin(87), 1.001);
   glVertex3d(0.2 * Cos(87), 0.2 * Sin(87), 1.001);
   glEnd();
   
   glBegin(GL_QUADS);
   normal(0.2 * Cos(93),0.2 * Sin(93),1.001, 0.47 * Cos(93),0.47 * Sin(93),1.001, 0.47 * Cos(132),0.47 * Sin(132),1.001);
   glVertex3d(0.2 * Cos(93), 0.2 * Sin(93), 1.001);
   glVertex3d(0.47 * Cos(93), 0.47 * Sin(93), 1.001);
   glVertex3d(0.47 * Cos(132), 0.47 * Sin(132), 1.001);
   glVertex3d(0.2 * Cos(132), 0.2 * Sin(132), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(138),0.2 * Sin(138),1.001, 0.47 * Cos(138),0.47 * Sin(138),1.001, 0.47 * Cos(177),0.47 * Sin(177),1.001);
   glVertex3d(0.2 * Cos(138), 0.2 * Sin(138), 1.001);
   glVertex3d(0.47 * Cos(138), 0.47 * Sin(138), 1.001);
   glVertex3d(0.47 * Cos(177), 0.47 * Sin(177), 1.001);
   glVertex3d(0.2 * Cos(177), 0.2 * Sin(177), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(183),0.2 * Sin(183),1.001, 0.47 * Cos(183),0.47 * Sin(183),1.001, 0.47 * Cos(222),0.47 * Sin(222),1.001);
   glVertex3d(0.2 * Cos(183), 0.2 * Sin(183), 1.001);
   glVertex3d(0.47 * Cos(183), 0.47 * Sin(183), 1.001);
   glVertex3d(0.47 * Cos(222), 0.47 * Sin(222), 1.001);
   glVertex3d(0.2 * Cos(222), 0.2 * Sin(222), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(228),0.2 * Sin(228),1.001, 0.47 * Cos(228),0.47 * Sin(228),1.001, 0.47 * Cos(267),0.47 * Sin(267),1.001);
   glVertex3d(0.2 * Cos(228), 0.2 * Sin(228), 1.001);
   glVertex3d(0.47 * Cos(228), 0.47 * Sin(228), 1.001);
   glVertex3d(0.47 * Cos(267), 0.47 * Sin(267), 1.001);
   glVertex3d(0.2 * Cos(267), 0.2 * Sin(267), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(273),0.2 * Sin(273),1.001, 0.47 * Cos(273),0.47 * Sin(273),1.001, 0.47 * Cos(312),0.47 * Sin(312),1.001);
   glVertex3d(0.2 * Cos(273), 0.2 * Sin(273), 1.001);
   glVertex3d(0.47 * Cos(273), 0.47 * Sin(273), 1.001);
   glVertex3d(0.47 * Cos(312), 0.47 * Sin(312), 1.001);
   glVertex3d(0.2 * Cos(312), 0.2 * Sin(312), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(318),0.2 * Sin(318),1.001, 0.47 * Cos(318),0.47 * Sin(318),1.001, 0.47 * Cos(357),0.47 * Sin(357),1.001);
   glVertex3d(0.2 * Cos(318), 0.2 * Sin(318), 1.001);
   glVertex3d(0.47 * Cos(318), 0.47 * Sin(318), 1.001);
   glVertex3d(0.47 * Cos(357), 0.47 * Sin(357), 1.001);
   glVertex3d(0.2 * Cos(357), 0.2 * Sin(357), 1.001);
   glEnd();
   
   // Center window
   glBegin(GL_POLYGON);
   glColor3d(0, 0, 0);
   glNormal3d(0,0,1);
   glVertex3d(0.17 * Cos(0), 0.17 * Sin(0), 1.001);
   glVertex3d(0.17 * Cos(45), 0.17 * Sin(45), 1.001);
   glVertex3d(0.17 * Cos(90), 0.17 * Sin(90), 1.001);
   glVertex3d(0.17 * Cos(135), 0.17 * Sin(135), 1.001);
   glVertex3d(0.17 * Cos(180), 0.17 * Sin(180), 1.001);
   glVertex3d(0.17 * Cos(225), 0.17 * Sin(225), 1.001);
   glVertex3d(0.17 * Cos(270), 0.17 * Sin(270), 1.001);
   glVertex3d(0.17 * Cos(315), 0.17 * Sin(315), 1.001);
   glVertex3d(0.17 * Cos(315), 0.17 * Sin(315), 1.001);
   glVertex3d(0.17 * Cos(360), 0.17 * Sin(360), 1.001);
   glEnd();
   
   
   // Lasers - Positioning
   
   glPushMatrix();
   glTranslated(0, -0.8, 0.9);
   
   // Laser - Starboard
   
   glPushMatrix();
   glTranslated(-0.27, 0.18, 0);
   glScaled(0.05, 0.05, 0.05);
   
   for (ph=-90;ph<90;ph+=d)
   {
      
      glColor3d(1, 0, 0);
      
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
         
      }
      glEnd();
      
   }
   
   glPopMatrix();
   
   glBegin(GL_TRIANGLES);
   glColor3d(0.15, 0.15, 0.15);
   glNormal3d(0, 0, 1);
   glVertex3d(-0.1, 0.2, 0);
   glVertex3d(-0.3, 0, 0);
   glVertex3d(-0.5, 0.4, 0);
   glEnd();
   
   glBegin(GL_QUADS);
   glColor3d(0.35, 0.35, 0.35);
   normal(-0.1,0.2,0, -0.1,0.2,-0.5, -0.5,0.4,0);
   glVertex3d(-0.1, 0.2, -0.5);
   glVertex3d(-0.1, 0.2, 0);
   glVertex3d(-0.5, 0.4, 0);
   glVertex3d(-0.5, 0.4, -0.5);
   glEnd();
   
   glBegin(GL_QUADS);
   glColor3d(0.35, 0.35, 0.35);
   normal(-0.3,0,0, -0.3,0,-0.5, -0.5,0.4,0);
   glVertex3d(-0.5, 0.4, -0.5);
   glVertex3d(-0.5, 0.4, 0);
   glVertex3d(-0.3, 0, 0);
   glVertex3d(-0.3, 0, -0.5);
   glEnd();
   
   glBegin(GL_QUADS);
   glColor3d(0.35, 0.35, 0.35);
   normal(-0.3,0,0, -0.3,0,-0.5, -0.1,0.2,-0.5);
   glVertex3d(-0.3, 0, -0.5);
   glVertex3d(-0.3, 0, 0);
   glVertex3d(-0.1, 0.2, 0);
   glVertex3d(-0.1, 0.2, -0.5);
   glEnd();
   
   // Laser - Port
   
   glPushMatrix();
   glTranslated(0.27, 0.18, 0);
   glScaled(0.05, 0.05, 0.05);
   
   for (ph=-90;ph<90;ph+=d)
   {
      
      glColor3d(1, 0, 0);
      
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
         
      }
      glEnd();
      
   }
   glPopMatrix();
   
   glBegin(GL_TRIANGLES);
   glColor3d(0.15, 0.15, 0.15);
   glNormal3d(0,0,1);
   glVertex3d(0.1, 0.2, 0);
   glVertex3d(0.3, 0, 0);
   glVertex3d(0.5, 0.4, 0);
   glEnd();
   
   glBegin(GL_QUADS);
   glColor3d(0.35, 0.35, 0.35);
   normal(0.1,0.2,0, 0.5,0.4,0, 0.1,0.2,-0.5);
   glVertex3d(0.1, 0.2, -0.5);
   glVertex3d(0.1, 0.2, 0);
   glVertex3d(0.5, 0.4, 0);
   glVertex3d(0.5, 0.4, -0.5);
   glEnd();
   
   glBegin(GL_QUADS);
   glColor3d(0.35, 0.35, 0.35);
   normal(0.3,0,0, 0.3,0,-0.5, 0.5,0.4,0);
   glVertex3d(0.5, 0.4, -0.5);
   glVertex3d(0.5, 0.4, 0);
   glVertex3d(0.3, 0, 0);
   glVertex3d(0.3, 0, -0.5);
   glEnd();
   
   glBegin(GL_QUADS);
   glColor3d(0.35, 0.35, 0.35);
   normal(0.3,0,0, 0.1,0.2,0, 0.3,0,-0.5);
   glVertex3d(0.3, 0, -0.5);
   glVertex3d(0.3, 0, 0);
   glVertex3d(0.1, 0.2, 0);
   glVertex3d(0.1, 0.2, -0.5);
   glEnd();
   
   /* Beams*/
   
   /*
   if (beamPos < 4) {
      
      glLineWidth(2);
      glBegin(GL_LINES);
      glColor3d(0, 1, 0);
      glVertex3d(-0.27, 0.18, 0);
      glVertex3d(-0.27, 0.18, beamPos);
      
      glVertex3d(0.27, 0.18, 0);
      glVertex3d(0.27, 0.18, beamPos);
      glEnd();
      glLineWidth(1);
   }
   else {
      
      glLineWidth(2);
      glBegin(GL_LINES);
      glColor3d(0, 1, 0);
      glVertex3d(-0.27, 0.18, beamPos - 2);
      glVertex3d(-0.27, 0.18, 2 + beamPos);
      
      glVertex3d(0.27, 0.18, beamPos - 2);
      glVertex3d(0.27, 0.18, 2 + beamPos);
      glEnd();
      glLineWidth(1);
   }
   */
   
   
   
   glPopMatrix();
   
   
   
   // End lasers
   
   
   // Front windscreen border trim (joins windscreen to fuselage)
   glBegin(GL_QUAD_STRIP);
   
   color = 0.5;
   for (th = 0; th <= 360; th += 45, color += 0.015)
   {
      glColor3d(color, color, color);
      
      // x = sin(t)
      // y = cos(t)
      glNormal3d(Cos(th), Sin(th), 0);
      glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 1);
      glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 0.7);
      
   }
   glEnd();
   /* End front windscreen */
   
   /* Sponsons */
   //  Enable textures
   glEnable(GL_TEXTURE_2D);

   // Forward-Top face starboard
   glBegin(GL_QUADS);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0, 1, 1);
   glColor3d(0.7, 0.7, 0.7);
   glTexCoord2f(0.43,0); glVertex3d(0, 0, 0.7);
   glTexCoord2f(0,0); glVertex3d(0, 0.7, 0);
   glTexCoord2f(1,0.26); glVertex3d(-2.3, 0.3, 0);
   glTexCoord2f(1,0.17); glVertex3d(-2.3, 0, 0.3);
   glEnd();
   
   // Forward-Bottom face starboard
   glBegin(GL_QUADS);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0, -1, 1);
   glTexCoord2f(0,0); glVertex3d(0, -0.7, 0);
   glTexCoord2f(0.43,0); glVertex3d(0, 0, 0.7);
   glTexCoord2f(1,0.17); glVertex3d(-2.3, 0, 0.3);
   glTexCoord2f(1,0.26); glVertex3d(-2.3, -0.3, 0);
   glEnd();
   
   // Aft-Bottom face starboard
   glBegin(GL_QUADS);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0, -1, -1);
   glTexCoord2f(1,0.26); glVertex3d(-2.3, -0.3, 0);
   glTexCoord2f(1,0.17); glVertex3d(-2.3, 0, -0.3);
   glTexCoord2f(0.43,0); glVertex3d(0, 0, -0.7);
   glTexCoord2f(0,0); glVertex3d(0, -0.7, 0);
   glEnd();
   
   // Aft-Top face starboard
   glBegin(GL_QUADS);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0, 1, -1);
   glTexCoord2f(1,0.17); glVertex3d(-2.3, 0, -0.3);
   glTexCoord2f(1,0.26); glVertex3d(-2.3, 0.3, 0);
   glTexCoord2f(0,0); glVertex3d(0, 0.7, 0);
   glTexCoord2f(0.43,0); glVertex3d(0, 0, -0.7);
   glEnd();
   
   
   /* Sponson fuel cylinders */
   glPushMatrix();
   glTranslated(-2, 0, 0.3);
   glRotated(90, 0, 1, 0);

   
   glBegin(GL_TRIANGLE_FAN);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glColor3d(0.43, 0.43, 0.43);
   glNormal3d(-1, 0, 0);
   glVertex3d(0, 0, 4);
   for (th = 0; th <= 360; th += 5)
   {
      
      // x = sin(t)
      // y = cos(t)
      glTexCoord2f(0.5 + cos(th), 0.5 + sin(th)); glVertex3d(0.2 * Cos(th), 0.2 * Sin(th), 4);
      
   }
   glEnd();
   
   glBegin(GL_TRIANGLE_FAN);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(1, 0, 0);
   glVertex3d(0, 0, 3);
   for (th = 0; th <= 360; th += 5)
   {
      
      // x = sin(t)
      // y = cos(t)
      glTexCoord2f(0.5 + cos(th), 0.5 + sin(th)); glVertex3d(0.3 * Cos(th), 0.3 * Sin(th), 3);
      
   }
   glEnd();
   
   color = 0.7;
   glBegin(GL_QUAD_STRIP);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   for (th = 0; th <= 360; th += 5)
   {
      
      glColor3d(color, color, color);
      
      // x = sin(t)
      // y = cos(t)
      glNormal3d(Cos(th), Sin(th), 0);
      glTexCoord2d(th/360.0,ph/180.0+0.5); glVertex3d(0.3 * Cos(th), 0.3 * Sin(th), 3);
      glTexCoord2d(th/360.0,(ph+1)/180.0+0.5); glVertex3d(0.2 * Cos(th), 0.2 * Sin(th), 4);
      
      color -= 0.007;
      
   }
   
   glEnd();
   
   glBegin(GL_TRIANGLE_FAN);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glColor3d(0.43, 0.43, 0.43);
   glNormal3d(0, 0, 1);
   glVertex3d(0, 0, 1);
   for (th = 0; th <= 360; th += 5)
   {
      
      // x = sin(t)
      // y = cos(t)
      glTexCoord2f(0.5 + cos(th), 0.5 + sin(th)); glVertex3d(0.3 * Cos(th), 0.3 * Sin(th), 1);
      
   }
   glEnd();
   
   glBegin(GL_TRIANGLE_FAN);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0, 0, 1);
   glVertex3d(0, 0, 0);
   for (th = 0; th <= 360; th += 5)
   {
      
      // x = sin(t)
      // y = cos(t)
      glTexCoord2f(0.5 + cos(th), 0.5 + sin(th)); glVertex3d(0.2 * Cos(th), 0.2 * Sin(th), 0);
      
   }
   glEnd();
   
   color = 0.7;
   glBegin(GL_QUAD_STRIP);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   for (th = 0; th <= 360; th += 5)
   {
      
      glColor3d(color, color, color);
      
      // x = sin(t)
      // y = cos(t)
      glNormal3d(Cos(th), Sin(th), 0);
      glTexCoord2d(th/360.0,(ph+1)/180.0+0.5); glVertex3d(0.3 * Cos(th), 0.3 * Sin(th), 1);
      glTexCoord2d(th/360.0,ph/180.0+0.5); glVertex3d(0.2 * Cos(th), 0.2 * Sin(th), 0);
      
      color -= 0.007;
      
   }
   
   glEnd();
   glPopMatrix();
   /* End sponson fuel cylinders */
   
   // Forward-Top face port
   glColor3d(0.7, 0.7, 0.7);
   glBegin(GL_QUADS);
   normal(0,0.7,0, 2.3,0,0.3, 2.3,0.3,0);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(0.43,0); glVertex3d(0, 0, 0.7);
   glTexCoord2f(0,0); glVertex3d(0, 0.7, 0);
   glTexCoord2f(1,0.26); glVertex3d(2.3, 0.3, 0);
   glTexCoord2f(1,0.17); glVertex3d(2.3, 0, 0.3);
   glEnd();
   
   // Forward-Bottom face port
   glBegin(GL_QUADS);
   glNormal3d(0, -1, 1);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(0,0); glVertex3d(0, -0.7, 0);
   glTexCoord2f(0.43,0); glVertex3d(0, 0, 0.7);
   glTexCoord2f(1,0.17); glVertex3d(2.3, 0, 0.3);
   glTexCoord2f(1,0.26); glVertex3d(2.3, -0.3, 0);
   glEnd();
   
   // Aft-Bottom face port
   glBegin(GL_QUADS);
   glNormal3d(0, -1, -1);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(1,0.26); glVertex3d(2.3, -0.3, 0);
   glTexCoord2f(1,0.17); glVertex3d(2.3, 0, -0.3);
   glTexCoord2f(0.43,0); glVertex3d(0, 0, -0.7);
   glTexCoord2f(0,0); glVertex3d(0, -0.7, 0);
   glEnd();
   
   // Aft-Top face port
   glBegin(GL_QUADS);
   glNormal3d(0, 1, -1);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(1,0.17); glVertex3d(2.3, 0, -0.3);
   glTexCoord2f(1,0.26); glVertex3d(2.3, 0.3, 0);
   glTexCoord2f(0,0); glVertex3d(0, 0.7, 0);
   glTexCoord2f(0.43,0); glVertex3d(0, 0, -0.7);
   glEnd();
   
   /* End sponsons */
   
   /* ------------------------------------------- Wings ---------------------------------------- */
   // Center panel starboard
   // Inner
   glColor3d(0.9, 0.9, 0.9);

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(1,0,0);
   glTexCoord2f(1,0); glVertex3d(-2.3, -1, 2);
   glTexCoord2f(1,0.25); glVertex3d(-2.3, 1, 2);
   glTexCoord2f(0,0.25); glVertex3d(-2.3, 1, -6);
   glTexCoord2f(0,0); glVertex3d(-2.3, -1, -6);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glEnd();
   
   
   //Outer
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(-1,0,0);
   glTexCoord2f(0,0); glVertex3d(-2.4, -1, 2);
   glTexCoord2f(0,0.25); glVertex3d(-2.4, 1, 2);
   glTexCoord2f(1,0.25); glVertex3d(-2.4, 1, -6);
   glTexCoord2f(1,0); glVertex3d(-2.4, -1, -6);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   // Center Startboard Panels
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-1,-1);
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[17]);
   glNormal3d(-1,0,0);
   glTexCoord2f(1,0); glVertex3d(-2.4, -0.75, -0.92);
   glTexCoord2f(1,0.25); glVertex3d(-2.4, 0.75, -0.92);
   glTexCoord2f(0,0.25); glVertex3d(-2.4, 0.75, -5.75);
   glTexCoord2f(0,0); glVertex3d(-2.4, -0.75, -5.75);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-1,-1);
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[17]);
   glNormal3d(-1,0,0);
   glTexCoord2f(1,0); glVertex3d(-2.4, -0.75, 1.75);
   glTexCoord2f(1,0.62); glVertex3d(-2.4, 0.75, 1.75);
   glTexCoord2f(0,0.62); glVertex3d(-2.4, 0.75, -0.67);
   glTexCoord2f(0,0); glVertex3d(-2.4, -0.75, -0.67);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);
   
   // Forward edge center
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,0,1);
   glTexCoord2f(0.05,0); glVertex3d(-2.3, -1, 2);
   glTexCoord2f(0.05,1); glVertex3d(-2.3, 1, 2);
   glTexCoord2f(0,1); glVertex3d(-2.4, 1, 2);
   glTexCoord2f(0,0); glVertex3d(-2.4, -1, 2);
   glEnd();
   
   // Aft edge center
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,0,-1);
   glTexCoord2f(0.05,0); glVertex3d(-2.4, -1, -6);
   glTexCoord2f(0.05,1); glVertex3d(-2.4, 1, -6);
   glTexCoord2f(0,1); glVertex3d(-2.3, 1, -6);
   glTexCoord2f(0,0); glVertex3d(-2.3, -1, -6);
   glEnd();
   
   // ---------------------------- Top-tilted panel starboard --------------------------
   // Inner
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(-1.8,2,1.25, -2.3,1,2, -1.8,2,-4.75);
   glTexCoord2f(0,0); glVertex3d(-2.3, 1, 2);
   glTexCoord2f(0.125,0.125); glVertex3d(-1.8, 2, 1.25);
   glTexCoord2f(0.875,0.125); glVertex3d(-1.8, 2, -4.75);
   glTexCoord2f(1,0); glVertex3d(-2.3, 1, -6);
   glEnd();
   
   // Outer
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(-1.9,2,1.25, -1.9,2,-4.75, -2.4,1,2);
   glTexCoord2f(0,0); glVertex3d(-2.4, 1, 2);
   glTexCoord2f(0.125,0.125); glVertex3d(-1.9, 2, 1.25);
   glTexCoord2f(0.875,0.125); glVertex3d(-1.9, 2, -4.75);
   glTexCoord2f(1,0); glVertex3d(-2.4, 1, -6);
   glEnd();
   
   // Forward edge top
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(-2.4,1,2, -2.3,1,2, -1.8,2,1.25);
   glTexCoord2f(0.1,0); glVertex3d(-2.3, 1, 2);
   glTexCoord2f(0.6,1); glVertex3d(-1.8, 2, 1.25);
   glTexCoord2f(0.5,1); glVertex3d(-1.9, 2, 1.25);
   glTexCoord2f(0,0); glVertex3d(-2.4, 1, 2);
   glEnd();
   
   // Aft edge top
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(-2.3,1,-6, -1.9,2,-4.75, -2.4,1,-6);
   glTexCoord2f(0.1,0); glVertex3d(-2.4, 1, -6);
   glTexCoord2f(0.6,1); glVertex3d(-1.9, 2, -4.75);
   glTexCoord2f(0.5,1); glVertex3d(-1.8, 2, -4.75);
   glTexCoord2f(0,0); glVertex3d(-2.3, 1, -6);
   glEnd();
   
   // Top edge
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,1,0);
   glTexCoord2f(0.017,1); glVertex3d(-1.8, 2, 1.25);
   glTexCoord2f(0.017,0); glVertex3d(-1.8, 2, -4.75);
   glTexCoord2f(0,0); glVertex3d(-1.9, 2, -4.75);
   glTexCoord2f(0,1); glVertex3d(-1.9, 2, 1.25);
   glEnd();
   
   // ------------------------- Bottom-tilted panel starboard -----------------------
   // Inner
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(-2.3,-1,2, -1.8,-2,1.25, -2.3,-1,-6);
   glTexCoord2f(0.125,0.125); glVertex3d(-1.8, -2, 1.25);
   glTexCoord2f(0,0); glVertex3d(-2.3, -1, 2);
   glTexCoord2f(1,0); glVertex3d(-2.3, -1, -6);
   glTexCoord2f(0.875,0.125); glVertex3d(-1.8, -2, -4.75);
   glEnd();
   
   // Outer
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(-2.4,-1,2, -2.4,-1,-6, -1.9,-2,1.25);
   glTexCoord2f(0.125,0.125); glVertex3d(-1.9, -2, 1.25);
   glTexCoord2f(0,0); glVertex3d(-2.4, -1, 2);
   glTexCoord2f(1,0); glVertex3d(-2.4, -1, -6);
   glTexCoord2f(0.875,0.125); glVertex3d(-1.9, -2, -4.75);
   glEnd();
   
   // Forward edge bottom
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(-1.9,-2,1.25, -1.8,-2,1.25, -2.3,-1,2);
   glTexCoord2f(0.6,1); glVertex3d(-1.8, -2, 1.25);
   glTexCoord2f(0.1,0); glVertex3d(-2.3, -1, 2);
   glTexCoord2f(0,0); glVertex3d(-2.4, -1, 2);
   glTexCoord2f(0.5,1); glVertex3d(-1.9, -2, 1.25);
   glEnd();
   
   // Aft edge bottom
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(-1.8,-2,-4.75, -2.4,-1,-6, -1.9,-2,-4.75);
   glTexCoord2f(0.5,1); glVertex3d(-1.9, -2, -4.75);
   glTexCoord2f(0,0); glVertex3d(-2.4, -1, -6);
   glTexCoord2f(0.1,0); glVertex3d(-2.3, -1, -6);
   glTexCoord2f(0.6,1); glVertex3d(-1.8, -2, -4.75);
   glEnd();
   
   // Bottom edge
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,-1,0);
   glTexCoord2f(0.017,0); glVertex3d(-1.8, -2, -4.75);
   glTexCoord2f(0.017,1); glVertex3d(-1.8, -2, 1.25);
   glTexCoord2f(0,1); glVertex3d(-1.9, -2, 1.25);
   glTexCoord2f(0,0); glVertex3d(-1.9, -2, -4.75);
   glEnd();
      
   // ------------------------------- Center panel port ------------------------------
   
   // Inner
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(-1,0,0);
   glTexCoord2f(1,0); glVertex3d(2.3, -1, 2);
   glTexCoord2f(1,0.25); glVertex3d(2.3, 1, 2);
   glTexCoord2f(0,0.25); glVertex3d(2.3, 1, -6);
   glTexCoord2f(0,0); glVertex3d(2.3, -1, -6);
   glEnd();
   
   // Outer
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(1,0,0);
   glTexCoord2f(1,0); glVertex3d(2.4, -1, 2);
   glTexCoord2f(1,0.25); glVertex3d(2.4, 1, 2);
   glTexCoord2f(0,0.25); glVertex3d(2.4, 1, -6);
   glTexCoord2f(0,0); glVertex3d(2.4, -1, -6);
   glEnd();

   // Center Port Panels
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[17]);
   glNormal3d(1,0,0);
   glTexCoord2f(1,0); glVertex3d(2.401, -0.75, -0.92);
   glTexCoord2f(1,0.25); glVertex3d(2.401, 0.75, -0.92);
   glTexCoord2f(0,0.25); glVertex3d(2.401, 0.75, -5.75);
   glTexCoord2f(0,0); glVertex3d(2.401, -0.75, -5.75);
   glEnd();

   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[17]);
   glNormal3d(1,0,0);
   glTexCoord2f(1,0); glVertex3d(2.401, -0.75, 1.75);
   glTexCoord2f(1,0.62); glVertex3d(2.401, 0.75, 1.75);
   glTexCoord2f(0,0.62); glVertex3d(2.401, 0.75, -0.67);
   glTexCoord2f(0,0); glVertex3d(2.401, -0.75, -0.67);
   glEnd();
   
   // Forward edge center
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,0,1);
   glTexCoord2f(0.05,0); glVertex3d(2.3, -1, 2);
   glTexCoord2f(0.05,1); glVertex3d(2.3, 1, 2);
   glTexCoord2f(0,1); glVertex3d(2.4, 1, 2);
   glTexCoord2f(0,0); glVertex3d(2.4, -1, 2);
   glEnd();
   
   // Aft edge center
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,0,-1);
   glTexCoord2f(0.05,0); glVertex3d(2.4, -1, -6);
   glTexCoord2f(0.05,1); glVertex3d(2.4, 1, -6);
   glTexCoord2f(0,1); glVertex3d(2.3, 1, -6);
   glTexCoord2f(0,0); glVertex3d(2.3, -1, -6);
   glEnd();
   
   // ----------------------------- Top-tilted panel port ----------------------------
   // Inner
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(1.8,2,1.25, 1.8,2,-4.75, 2.3,1,2);
   glTexCoord2f(0,0); glVertex3d(2.3, 1, 2);
   glTexCoord2f(0.125,0.125); glVertex3d(1.8, 2, 1.25);
   glTexCoord2f(0.875,0.125); glVertex3d(1.8, 2, -4.75);
   glTexCoord2f(1,0); glVertex3d(2.3, 1, -6);
   glEnd();
   
   // Outer
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(1.9,2,1.25, 2.4,1,2, 1.9,2,-4.75);
   glTexCoord2f(0,0); glVertex3d(2.4, 1, 2);
   glTexCoord2f(0.125,0.125); glVertex3d(1.9, 2, 1.25);
   glTexCoord2f(0.875,0.125); glVertex3d(1.9, 2, -4.75);
   glTexCoord2f(1,0); glVertex3d(2.4, 1, -6);
   glEnd();
   
   // Forward edge top
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(2.3,1,2, 2.4,1,2, 1.8,2,1.25);
   glTexCoord2f(0.1,0); glVertex3d(2.3, 1, 2);
   glTexCoord2f(0.6,1); glVertex3d(1.8, 2, 1.25);
   glTexCoord2f(0.5,1); glVertex3d(1.9, 2, 1.25);
   glTexCoord2f(0,0); glVertex3d(2.4, 1, 2);
   glEnd();
   
   // Aft edge top
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(2.3,1,-6, 1.9,2,-4.75, 2.4,1,-6);
   glTexCoord2f(0,0); glVertex3d(2.4, 1, -6);
   glTexCoord2f(0.5,1); glVertex3d(1.9, 2, -4.75);
   glTexCoord2f(0.6,1); glVertex3d(1.8, 2, -4.75);
   glTexCoord2f(0.1,0); glVertex3d(2.3, 1, -6);
   glEnd();
   
   // Top edge
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,1,0);
   glTexCoord2f(0.017,1); glVertex3d(1.8, 2, 1.25);
   glTexCoord2f(0.017,0); glVertex3d(1.8, 2, -4.75);
   glTexCoord2f(0,0); glVertex3d(1.9, 2, -4.75);
   glTexCoord2f(0,1); glVertex3d(1.9, 2, 1.25);
   glEnd();

   // --------------------------------- Bottom-tilted panel port ---------------------------
   // Inner
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(1.8,-2,1.25, 2.3,-1,2, 1.8,-2,-4.75);
   glTexCoord2f(0.125,0.125); glVertex3d(1.8, -2, 1.25);
   glTexCoord2f(0,0); glVertex3d(2.3, -1, 2);
   glTexCoord2f(1,0); glVertex3d(2.3, -1, -6);
   glTexCoord2f(0.875,0.125); glVertex3d(1.8, -2, -4.75);
   glEnd();

   //Outer
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(1.9,-2,1.25, 1.9,-2,-4.75, 2.4,-1,2);
   glTexCoord2f(0.125,0.125); glVertex3d(1.9, -2, 1.25);
   glTexCoord2f(0,0); glVertex3d(2.4, -1, 2);
   glTexCoord2f(1,0); glVertex3d(2.4, -1, -6);
   glTexCoord2f(0.875,0.125); glVertex3d(1.9, -2, -4.75);
   glEnd();
   
   // Forward edge bottom
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(1.8,-2,1.25, 1.9,-2,1.25, 2.3,-1,2);
   glTexCoord2f(0.6,1); glVertex3d(1.8, -2, 1.25);
   glTexCoord2f(0.1,0); glVertex3d(2.3, -1, 2);
   glTexCoord2f(0,0); glVertex3d(2.4, -1, 2);
   glTexCoord2f(0.5,1); glVertex3d(1.9, -2, 1.25);
   glEnd();
   
   // Aft edge bottom
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   normal(1.8,-2,-4.75, 2.3,-1,-6, 1.9,-2,-4.75);
   glTexCoord2f(0.5,1); glVertex3d(1.9, -2, -4.75);
   glTexCoord2f(0,0); glVertex3d(2.4, -1, -6);
   glTexCoord2f(0.1,0); glVertex3d(2.3, -1, -6);
   glTexCoord2f(0.6,1); glVertex3d(1.8, -2, -4.75);
   glEnd();
   
   // Bottom edge
   glBegin(GL_POLYGON);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,-1,0);
   glTexCoord2f(0.017,0); glVertex3d(1.8, -2, -4.75);
   glTexCoord2f(0.017,1); glVertex3d(1.8, -2, 1.25);
   glTexCoord2f(0,1); glVertex3d(1.9, -2, 1.25);
   glTexCoord2f(0,0); glVertex3d(1.9, -2, -4.75);
   glEnd();

   /* --------------------------------------- Rear elevator ------------------------------ */
   
   glPushMatrix();
   
   glTranslated(0, 0.15, 0);
   glRotated(-90, 1, 0, 0);
   
   
   // Top panel
   glBegin(GL_TRIANGLE_FAN);
   glColor3d(0.6, 0.6, 0.6);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glVertex3d(0, 0, 0);
   glNormal3d(0,0,1);
   for (th = 45; th <= 135; th += 5)
   {
      
      // x = sin(t)
      // y = cos(t)
      glTexCoord2f(0.5 + cos(th), 0.5 + sin(th)); glVertex3d((4.6 * Cos(45)) * Cos(th), (4.6 * Cos(45)) * Sin(th), 0);
      
   }
   
   glBegin(GL_QUADS);
   glNormal3d(0,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(1,0); glVertex3d(2.3, 0, 0);
   glTexCoord2f(1,0.5); glVertex3d(2.3, 2.3, 0);
   glTexCoord2f(0,0.5); glVertex3d(-2.3, 2.3, 0);
   glTexCoord2f(0,0); glVertex3d(-2.3, 0, 0);
   glEnd();
   
   // Bottom panel
   glBegin(GL_TRIANGLE_FAN);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,0,1);
   glVertex3d(0, 0, -0.3);
   for (th = 45; th <= 135; th += 5)
   {
      
      // x = sin(t)
      // y = cos(t)
      glTexCoord2f(0.5 + cos(th), 0.5 + sin(th)); glVertex3d((4.6 * Cos(45)) * Cos(th), (4.6 * Cos(45)) * Sin(th), -0.3);
      
   }
   glBegin(GL_QUADS);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glNormal3d(0,0,1);
   glTexCoord2f(1,0); glVertex3d(2.3, 0, -0.3);
   glTexCoord2f(1,0.5); glVertex3d(2.3, 2.3, -0.3);
   glTexCoord2f(0,0.5); glVertex3d(-2.3, 2.3, -0.3);
   glTexCoord2f(0,0); glVertex3d(-2.3, 0, -0.3);
   glEnd();
   
   // Arc strip (joins top and bottom panel on aft edge)
   glBegin(GL_QUAD_STRIP);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   
   for (th = 45; th <= 135; th += 5)
   {
      
      // x = sin(t)
      // y = cos(t)
      glNormal3d(Cos(45) * Cos(th), Cos(45) * Sin(th), 0);
      glTexCoord2f(0.5 + cos(th), 0.5 + sin(th)); glVertex3d((4.6 * Cos(45)) * Cos(th), (4.6 * Cos(45)) * Sin(th), 0);
      glTexCoord2f(0.5 + cos(th), 0.5 + sin(th) - 0.3); glVertex3d((4.6 * Cos(45)) * Cos(th), (4.6 * Cos(45)) * Sin(th), -0.3);
      
   }
   glEnd();
   
   // Engine compartment - top panel
   glBegin(GL_QUADS);
   glColor3d(0.35, 0.35, 0.35);
   glNormal3d(0,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(0.24,0); glVertex3d(0.3, 1, 0.15);
   glTexCoord2f(0.24,1); glVertex3d(0.3, 3.5, 0.15);
   glTexCoord2f(0,1); glVertex3d(-0.3, 3.5, 0.15);
   glTexCoord2f(0,0); glVertex3d(-0.3, 1, 0.15);
   glEnd();
   
   // Engine compartment - bottom panel
   glBegin(GL_QUADS);
   glNormal3d(0,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(0.24,0); glVertex3d(-0.3, 1, -0.45);
   glTexCoord2f(0.24,1); glVertex3d(-0.3, 3.5, -0.45);
   glTexCoord2f(0,1); glVertex3d(0.3, 3.5, -0.45);
   glTexCoord2f(0,0); glVertex3d(0.3, 1, -0.45);
   glEnd();
   
   // Engine compartment - starboard panel
   glBegin(GL_QUADS);
   glNormal3d(-1,0,0);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(0.24,0); glVertex3d(-0.3, 1, 0.15);
   glTexCoord2f(0.24,1); glVertex3d(-0.3, 3.5, 0.15);
   glTexCoord2f(0,1); glVertex3d(-0.3, 3.5, -0.45);
   glTexCoord2f(0,0); glVertex3d(-0.3, 1, -0.45);
   glEnd();
   
   // Engine compartment - port panel
   glBegin(GL_QUADS);
   glNormal3d(1,0,0);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(0.24,0); glVertex3d(0.3, 1, -0.45);
   glTexCoord2f(0.24,1); glVertex3d(0.3, 3.5, -0.45);
   glTexCoord2f(0,1); glVertex3d(0.3, 3.5, 0.15);
   glTexCoord2f(0,0); glVertex3d(0.3, 1, 0.15);
   glEnd();
   
   // Engine compartment - end cap
   glBegin(GL_QUADS);
   glNormal3d(0,1,0);
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glTexCoord2f(0,0); glVertex3d(-0.3, 3.5, -0.45);
   glTexCoord2f(0,1); glVertex3d(-0.3, 3.5, 0.15);
   glTexCoord2f(1,1); glVertex3d(0.3, 3.5, 0.15);
   glTexCoord2f(1,0); glVertex3d(0.3, 3.5, -0.45);
   glEnd();
   
    // Disable Textures
   glDisable(GL_TEXTURE_2D);
      
   glPopMatrix();
   /* End rear elevator */

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw an x-wing wing laser cannon
 *     
 */
static void wingLaser(double x, double y, double z){
   int th = 0;

   glPushMatrix();
   glTranslated(x, y, z);

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 0);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,-1);
      glTexCoord2f(0.1558 + 0.5 * Cos(th), 0.25 + 0.5 * Sin(th)); glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 0);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 0);
      glTexCoord2f(1,th*0.00274); glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 1);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 1);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,-1);
      glTexCoord2f(0.2182 + 0.7 * Cos(th), 0.35 + 0.7 * Sin(th)); glVertex3d(0.7 * Cos(th), 0.7 * Sin(th), 1);
      
   }
   glEnd();
   
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 8);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.2182 + 0.7 * Cos(th), 0.35 + 0.7 * Sin(th)); glVertex3d(0.7 * Cos(th), 0.7 * Sin(th), 8);
      
   }
   glEnd();
   
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(0.7 * Cos(th), 0.7 * Sin(th), 1);
      glTexCoord2f(1,th*0.00274); glVertex3d(0.7 * Cos(th), 0.7 * Sin(th), 8);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 16);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.1247 + 0.4 * Cos(th), 0.2 + 0.4 * Sin(th)); glVertex3d(0.4 * Cos(th), 0.4 * Sin(th), 16);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[7]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(0.4 * Cos(th), 0.4 * Sin(th), 8);
      glTexCoord2f(1,th*0.00274); glVertex3d(0.4 * Cos(th), 0.4 * Sin(th), 16);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 24);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.08 + 0.25 * Cos(th), 0.1875 + 0.25 * Sin(th)); glVertex3d(0.25 * Cos(th), 0.25 * Sin(th), 24);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(0.25 * Cos(th), 0.25 * Sin(th), 16);
      glTexCoord2f(1,th*0.00274); glVertex3d(0.25 * Cos(th), 0.25 * Sin(th), 24);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 24.5);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.256 + 0.8 * Cos(th), 0.6 + 0.8 * Sin(th)); glVertex3d(0.8 * Cos(th), 0.8 * Sin(th), 24.5);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(0.4 * Cos(th), 0.4 * Sin(th), 24);
      glTexCoord2f(1,th*0.00274); glVertex3d(0.4 * Cos(th), 0.4 * Sin(th), 24.5);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 26);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(0.1 * Cos(th), 0.1 * Sin(th), 26);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(0.1 * Cos(th), 0.1 * Sin(th), 24.5);
      glTexCoord2f(1,th*0.00274); glVertex3d(0.1 * Cos(th), 0.1 * Sin(th), 26);
   }
   glEnd();

   glPopMatrix();
}

/*
 *  Draw an x-wing wing engine
 *     
 */
static void wingEngine(double x, double y, double z){
   int th = 0;
   glPushMatrix();
   glTranslated(x, y, z);

   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 0);
   for (th = 0; th <= 360; th += 1)
   {
      glNormal3d(0,0,-1);
      glTexCoord2f(0.25 + 0.1 * Cos(th), 0.5 + 0.1 * Sin(th)); glVertex3d(1.5 * Cos(th), 1.5 * Sin(th), 0);
      
   }
   glEnd();
   
   glBindTexture(GL_TEXTURE_2D,texture[15]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 8);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.4 + 0.5* Cos(th), 0.5 + 0.5 * Sin(th)); glVertex3d(1.5 * Cos(th), 1.5 * Sin(th), 8);
      
   }
   glEnd();
   
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 1)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.0016); glVertex3d(1.5 * Cos(th), 1.5 * Sin(th), 0);
      glTexCoord2f(1,th*0.0016); glVertex3d(1.5 * Cos(th), 1.5 * Sin(th), 8);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[15]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, -9);
   for (th = 0; th <= 360; th += 1)
   {
      glNormal3d(0,0,-1);
      glTexCoord2f(0.4 + 0.5* Cos(th), 0.5 + 0.5 * Sin(th)); glVertex3d(0.8 * Cos(th), 0.8 * Sin(th), -9);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 1)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.0016); glVertex3d(.8 * Cos(th), 0.8 * Sin(th), 0);
      glTexCoord2f(1,th*0.0016); glVertex3d(.8 * Cos(th), 0.8 * Sin(th), -9);
   }
   glEnd();
   glPopMatrix();
}

/*
 *  Draw the upper wing of the x-wing
 *     
 */
static void createWings(){
   // ------------------- Left Top Wing -----------------------------------
   // Bottom of wing
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(2,2);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_POLYGON);
   normal(-3,0.5,-17, -3,0.5,-7, -25,8,-14);
   glTexCoord2f(1,0); glVertex3d(-3,+0.5,-7);
   glTexCoord2f(1,0.9031); glVertex3d(-3,+0.5,-17);
   glTexCoord2f(0,0.6217); glVertex3d(-25,+8,-14);
   glTexCoord2f(0,0.2709); glVertex3d(-25,+8,-10);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   // First panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(-3,0.5,-17, -3,0.5,-7, -25,8,-14);
   glTexCoord2f(1,0); glVertex3d(-5.2,1.25,-16.2);
   glTexCoord2f(1,0.9031); glVertex3d(-9.6,2.75,-15.6);
   glTexCoord2f(0,0.6217); glVertex3d(-9.6,2.75,-8.4);
   glTexCoord2f(0,0.2709); glVertex3d(-5.2,1.25,-7.8);
   glEnd();

   // Middle panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(-3,0.5,-17, -3,0.5,-7, -25,8,-14);
   glTexCoord2f(1,0); glVertex3d(-11.8,3.5,-15.3);
   glTexCoord2f(1,0.9031); glVertex3d(-16.2,5,-14.7);
   glTexCoord2f(0,0.6217); glVertex3d(-16.2,5,-9.3);
   glTexCoord2f(0,0.2709); glVertex3d(-11.8,3.5,-8.7);
   glEnd();

   // Tip panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(-3,0.5,-17, -3,0.5,-7, -25,8,-14);
   glTexCoord2f(1,0); glVertex3d(-18.4,5.75,-14.4);
   glTexCoord2f(1,0.9031); glVertex3d(-22.8,7.25,-13.8);
   glTexCoord2f(0,0.6217); glVertex3d(-22.8,7.25,-10.2);
   glTexCoord2f(0,0.2709); glVertex3d(-18.4,5.75,-9.6);
   glEnd();

   // Top of wing
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(2,2);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_POLYGON);
   normal(-3,1.5,-17, -25,9,-14, -3,1.5,-7);
   glTexCoord2f(1,0); glVertex3d(-3,+1.5,-7);
   glTexCoord2f(1,0.9031); glVertex3d(-3,+1.5,-17);
   glTexCoord2f(0,0.6217); glVertex3d(-25,+9,-14);
   glTexCoord2f(0,0.2709); glVertex3d(-25,+9,-10);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   // Red panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_POLYGON);
   normal(-3,1.5,-17, -25,9,-14, -3,1.5,-7);
   glTexCoord2f(1,0); glVertex3d(-20.6,7.5,-12.7);
   glTexCoord2f(1,0.9031); glVertex3d(-16.2,6,-12.4);
   glTexCoord2f(0,0.6217); glVertex3d(-16.2,6,-8.8);
   glTexCoord2f(0,0.2709); glVertex3d(-20.6,7.5,-9.7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_POLYGON);
   normal(-3,1.5,-17, -25,9,-14, -3,1.5,-7);
   glTexCoord2f(1,0); glVertex3d(-14,5.25,-12.25);
   glTexCoord2f(1,0.9031); glVertex3d(-16.2,6,-12.4);
   glTexCoord2f(0,0.6217); glVertex3d(-16.2,6,-11.7);
   glTexCoord2f(0,0.2709); glVertex3d(-14,5.25,-11.5);
   glEnd();

   // Wing front side
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(-25,8,-10, -25,9,-10, -3,0.5,-7);
   glTexCoord2f(0,0.1); glVertex3d(-3,+0.5,-7);
   glTexCoord2f(1,0.1); glVertex3d(-25,+8,-10);
   glTexCoord2f(1,0.1729); glVertex3d(-25,+9,-10);
   glTexCoord2f(0,0.1729); glVertex3d(-3,+1.5,-7);
   glEnd();

   // Wing back side
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(-25,8,-14, -3,0.5,-17, -25,9,-14);
   glTexCoord2f(1,0.1); glVertex3d(-3,+0.5,-17);
   glTexCoord2f(0,0.1); glVertex3d(-25,+8,-14);
   glTexCoord2f(0,0.1729); glVertex3d(-25,+9,-14);
   glTexCoord2f(1,0.1729); glVertex3d(-3,+1.5,-17);
   glEnd();

   // Wing end cap
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(-25,8,-14, -25,9,-14, -25,8,-10);
   glTexCoord2f(0,0.5011); glVertex3d(-25,+9,-14);
   glTexCoord2f(0,0.1); glVertex3d(-25,+8,-14);
   glTexCoord2f(1,0.1); glVertex3d(-25,+8,-10);
   glTexCoord2f(1,0.5011); glVertex3d(-25,+9,-10);
   glEnd();

   // Upper Left Engine and Laser
   wingEngine(-5.4, 3.75, -13);
   wingLaser(-24, 9, -16);

   // ------------------- Left Bottom Wing -----------------------------------
   // Top of wing
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(2,2);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_POLYGON);
   normal(-3,0.5,-17, -25,-7,-14, -3,0.5,-7);
   glTexCoord2f(1,0); glVertex3d(-3,+0.5,-7);
   glTexCoord2f(1,0.9031); glVertex3d(-3,+0.5,-17);
   glTexCoord2f(0,0.6217); glVertex3d(-25,-7,-14);
   glTexCoord2f(0,0.2709); glVertex3d(-25,-7,-10);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   // First panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(-3,0.5,-17, -25,-7,-14, -3,0.5,-7);
   glTexCoord2f(1,0); glVertex3d(-5.2,-0.25,-16.2);
   glTexCoord2f(1,0.9031); glVertex3d(-9.6,-1.75,-15.6);
   glTexCoord2f(0,0.6217); glVertex3d(-9.6,-1.75,-8.4);
   glTexCoord2f(0,0.2709); glVertex3d(-5.2,-0.25,-7.8);
   glEnd();

   // Middle panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(-3,0.5,-17, -25,-7,-14, -3,0.5,-7);
   glTexCoord2f(1,0); glVertex3d(-11.8,-2.5,-15.3);
   glTexCoord2f(1,0.9031); glVertex3d(-16.2,-4,-14.7);
   glTexCoord2f(0,0.6217); glVertex3d(-16.2,-4,-9.3);
   glTexCoord2f(0,0.2709); glVertex3d(-11.8,-2.5,-8.7);
   glEnd();

   // Tip panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(-3,0.5,-17, -25,-7,-14, -3,0.5,-7);
   glTexCoord2f(1,0); glVertex3d(-18.4,-4.75,-14.4);
   glTexCoord2f(1,0.9031); glVertex3d(-22.8,-6.25,-13.8);
   glTexCoord2f(0,0.6217); glVertex3d(-22.8,-6.25,-10.2);
   glTexCoord2f(0,0.2709); glVertex3d(-18.4,-4.75,-9.6);
   glEnd();

   // Bottom of wing
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_POLYGON);
   normal(-3,-0.5,-17, -3,-0.5,-7, -25,-8,-14);
   glTexCoord2f(1,0); glVertex3d(-3,-0.5,-7);
   glTexCoord2f(1,0.9031); glVertex3d(-3,-0.5,-17);
   glTexCoord2f(0,0.6217); glVertex3d(-25,-8,-14);
   glTexCoord2f(0,0.2709); glVertex3d(-25,-8,-10);
   glEnd();

   // Wing front side
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(-25,-7,-10, -25,-8,-10, -3,0.5,-7);
   glTexCoord2f(0,0.1); glVertex3d(-3,+0.5,-7);
   glTexCoord2f(1,0.1); glVertex3d(-25,-7,-10);
   glTexCoord2f(1,0.1729); glVertex3d(-25,-8,-10);
   glTexCoord2f(0,0.1729); glVertex3d(-3,-0.5,-7);
   glEnd();

   // Wing back side
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(-25,-7,-14, -3,0.5,-17, -25,-8,-14);
   glTexCoord2f(1,0.1); glVertex3d(-3,+0.5,-17);
   glTexCoord2f(0,0.1); glVertex3d(-25,-7,-14);
   glTexCoord2f(0,0.1729); glVertex3d(-25,-8,-14);
   glTexCoord2f(1,0.1729); glVertex3d(-3,-0.5,-17);
   glEnd();

   // Wing end cap
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(-25,-7,-14, -25,-8,-14, -25,-7,-10);
   glTexCoord2f(0,0.5011); glVertex3d(-25,-8,-14);
   glTexCoord2f(0,0.1); glVertex3d(-25,-7,-14);
   glTexCoord2f(1,0.1); glVertex3d(-25,-7,-10);
   glTexCoord2f(1,0.5011); glVertex3d(-25,-8,-10);
   glEnd();

   // Bottom Left Engine and Laser
   wingEngine(-5.4, -2.3, -13);
   wingLaser(-24, -8, -16);

   // ------------------- Right Top Wing -----------------------------------
   // Bottom of wing
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(2,2);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_POLYGON);
   normal(3,0.5,-17, 25,8,-14, 3,0.5,-7);
   glTexCoord2f(1,0); glVertex3d(+3,+0.5,-7);
   glTexCoord2f(1,0.9031); glVertex3d(+3,+0.5,-17);
   glTexCoord2f(0,0.6217); glVertex3d(+25,+8,-14);
   glTexCoord2f(0,0.2709); glVertex3d(+25,+8,-10);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   // First panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(3,0.5,-17, 25,8,-14, 3,0.5,-7);
   glTexCoord2f(1,0); glVertex3d(5.2,1.25,-16.2);
   glTexCoord2f(1,0.9031); glVertex3d(9.6,2.75,-15.6);
   glTexCoord2f(0,0.6217); glVertex3d(9.6,2.75,-8.4);
   glTexCoord2f(0,0.2709); glVertex3d(5.2,1.25,-7.8);
   glEnd();

   // Middle panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(3,0.5,-17, 25,8,-14, 3,0.5,-7);
   glTexCoord2f(1,0); glVertex3d(11.8,3.5,-15.3);
   glTexCoord2f(1,0.9031); glVertex3d(16.2,5,-14.7);
   glTexCoord2f(0,0.6217); glVertex3d(16.2,5,-9.3);
   glTexCoord2f(0,0.2709); glVertex3d(11.8,3.5,-8.7);
   glEnd();

   // Tip panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(3,0.5,-17, 25,8,-14, 3,0.5,-7);
   glTexCoord2f(1,0); glVertex3d(18.4,5.75,-14.4);
   glTexCoord2f(1,0.9031); glVertex3d(22.8,7.25,-13.8);
   glTexCoord2f(0,0.6217); glVertex3d(22.8,7.25,-10.2);
   glTexCoord2f(0,0.2709); glVertex3d(18.4,5.75,-9.6);
   glEnd();

   // Top of wing
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(2,2);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_POLYGON);
   normal(3,1.5,-17, 3,1.5,-7, 25,9,-14);
   glTexCoord2f(1,0); glVertex3d(+3,+1.5,-7);
   glTexCoord2f(1,0.9031); glVertex3d(+3,+1.5,-17);
   glTexCoord2f(0,0.6217); glVertex3d(+25,+9,-14);
   glTexCoord2f(0,0.2709); glVertex3d(+25,+9,-10);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   // Red panels on wing
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_POLYGON);
   normal(3,1.5,-17, 3,1.5,-7, 25,9,-14);
   glTexCoord2f(1,0); glVertex3d(20.6,7.5,-12.7);
   glTexCoord2f(1,0.9031); glVertex3d(16.2,6,-12.4);
   glTexCoord2f(0,0.6217); glVertex3d(16.2,6,-8.8);
   glTexCoord2f(0,0.2709); glVertex3d(20.6,7.5,-9.7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_POLYGON);
   normal(3,1.5,-17, 3,1.5,-7, 25,9,-14);
   glTexCoord2f(1,0); glVertex3d(14,5.25,-12.25);
   glTexCoord2f(1,0.9031); glVertex3d(16.2,6,-12.4);
   glTexCoord2f(0,0.6217); glVertex3d(16.2,6,-11.7);
   glTexCoord2f(0,0.2709); glVertex3d(14,5.25,-11.5);
   glEnd();

   // Front wing side
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(25,8,-10, 25,9,-10, 3,0.5,-7);
   glTexCoord2f(0,0.1); glVertex3d(+3,+0.5,-7);
   glTexCoord2f(1,0.1); glVertex3d(+25,+8,-10);
   glTexCoord2f(1,0.1729); glVertex3d(+25,+9,-10);
   glTexCoord2f(0,0.1729); glVertex3d(+3,+1.5,-7);
   glEnd();

   // Back wing side
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(+25,8,-14, +3,0.5,-17, +25,9,-14);
   glTexCoord2f(1,0.1); glVertex3d(+3,+0.5,-17);
   glTexCoord2f(0,0.1); glVertex3d(+25,+8,-14);
   glTexCoord2f(0,0.1729); glVertex3d(+25,+9,-14);
   glTexCoord2f(1,0.1729); glVertex3d(+3,+1.5,-17);
   glEnd();

   // Wing end cap
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(25,8,-14, 25,9,-14, 25,8,-10);
   glTexCoord2f(0,0.5011); glVertex3d(+25,+9,-14);
   glTexCoord2f(0,0.1); glVertex3d(+25,+8,-14);
   glTexCoord2f(1,0.1); glVertex3d(+25,+8,-10);
   glTexCoord2f(1,0.5011); glVertex3d(+25,+9,-10);
   glEnd();

   // Upper Right Engine and Laser
   wingEngine(5.4, 3.75, -13);
   wingLaser(24, 9, -16);

   // ------------------- Right Bottom Wing -----------------------------------
   // Top of wing
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(2,2);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_POLYGON);
   normal(3,0.5,-17, 3,0.5,-7, 25,-7,-14);
   glTexCoord2f(1,0); glVertex3d(+3,+0.5,-7);
   glTexCoord2f(1,0.9031); glVertex3d(+3,+0.5,-17);
   glTexCoord2f(0,0.6217); glVertex3d(+25,-7,-14);
   glTexCoord2f(0,0.2709); glVertex3d(+25,-7,-10);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   // First panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(3,0.5,-17, 3,0.5,-7, 25,-7,-14);
   glTexCoord2f(1,0); glVertex3d(5.2,-0.25,-16.2);
   glTexCoord2f(1,0.9031); glVertex3d(9.6,-1.75,-15.6);
   glTexCoord2f(0,0.6217); glVertex3d(9.6,-1.75,-8.4);
   glTexCoord2f(0,0.2709); glVertex3d(5.2,-0.25,-7.8);
   glEnd();

   // Middle panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(3,0.5,-17, 3,0.5,-7, 25,-7,-14);
   glTexCoord2f(1,0); glVertex3d(11.8,-2.5,-15.3);
   glTexCoord2f(1,0.9031); glVertex3d(16.2,-4,-14.7);
   glTexCoord2f(0,0.6217); glVertex3d(16.2,-4,-9.3);
   glTexCoord2f(0,0.2709); glVertex3d(11.8,-2.5,-8.7);
   glEnd();

   // Tip panel on wing
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_POLYGON);
   normal(3,0.5,-17, 3,0.5,-7, 25,-7,-14);
   glTexCoord2f(1,0); glVertex3d(18.4,-4.75,-14.4);
   glTexCoord2f(1,0.9031); glVertex3d(22.8,-6.25,-13.8);
   glTexCoord2f(0,0.6217); glVertex3d(22.8,-6.25,-10.2);
   glTexCoord2f(0,0.2709); glVertex3d(18.4,-4.75,-9.6);
   glEnd();


   // Bottom of wing
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_POLYGON);
   normal(3,-0.5,-17, 25,-8,-14, 3,-0.5,-7);
   glTexCoord2f(1,0); glVertex3d(+3,-0.5,-7);
   glTexCoord2f(1,0.9031); glVertex3d(+3,-0.5,-17);
   glTexCoord2f(0,0.6217); glVertex3d(+25,-8,-14);
   glTexCoord2f(0,0.2709); glVertex3d(+25,-8,-10);
   glEnd();

   // Wing front side
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(25,-7,-10, 3,0.5,-7, 25,-8,-10);
   glTexCoord2f(0,0.1); glVertex3d(+3,+0.5,-7);
   glTexCoord2f(1,0.1); glVertex3d(+25,-7,-10);
   glTexCoord2f(1,0.1729); glVertex3d(+25,-8,-10);
   glTexCoord2f(0,0.1729); glVertex3d(+3,-0.5,-7);
   glEnd();

   // Wing back side
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(25,-7,-14, 25,-8,-14, 3,0.5,-17);
   glTexCoord2f(1,0.1); glVertex3d(+3,+0.5,-17);
   glTexCoord2f(0,0.1); glVertex3d(+25,-7,-14);
   glTexCoord2f(0,0.1729); glVertex3d(+25,-8,-14);
   glTexCoord2f(1,0.1729); glVertex3d(+3,-0.5,-17);
   glEnd();

   // Wing end cap
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   normal(25,-7,-14, 25,-7,-10, 25,-8,-14);
   glTexCoord2f(0,0.5011); glVertex3d(+25,-8,-14);
   glTexCoord2f(0,0.1); glVertex3d(+25,-7,-14);
   glTexCoord2f(1,0.1); glVertex3d(+25,-7,-10);
   glTexCoord2f(1,0.5011); glVertex3d(+25,-8,-10);
   glEnd();

   // Bottom Right Engine and Laser
   wingEngine(5.4, -2.3, -13);
   wingLaser(24, -8, -16);
}

/*
 *  Draw an x-wing
 *     
 */
static void xWing(double x, double y, double z,
                 double s,
                 double rx, double ry, double rz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   
   //  Offset
   glTranslated(x,y,z);
   glScaled(s,s,s);
   glRotated(th,rx,ry,rz);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);

   // Nose left top panel
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-5,0,-3, -2,0,30, -1,1.5,30);
   glTexCoord2f(-1,-1); glVertex3d(-5,+0,-3);
   glTexCoord2f(-1,1); glVertex3d(-2,+0,+30);
   glTexCoord2f(0,1); glVertex3d(-1,+1.5,+30);
   glTexCoord2f(1,1); glVertex3d(-2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-5,0,-3, -2,2.5,7, -4,2,-3);
   glTexCoord2f(-1,-1); glVertex3d(-5,+0,-3);
   glTexCoord2f(-1,1); glVertex3d(-4,+2,-3);
   glTexCoord2f(1,1); glVertex3d(-2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-2,2.5,7, -3,5,0, -3,5,-2);
   glTexCoord2f(1,1); glVertex3d(-3,5,0);
   glTexCoord2f(1,0); glVertex3d(-3,5,-2);
   glTexCoord2f(0,0); glVertex3d(-2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-2,2.5,7, -3,5,-2, -3.8,2,-2);
   glTexCoord2f(-1,1); glVertex3d(-3,5,-2);
   glTexCoord2f(-1,1); glVertex3d(-3.8,2,-2);
   glTexCoord2f(1,1); glVertex3d(-2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-2,2.5,7, -3.8,2,-2, -4.7,0.25,-2);
   glTexCoord2f(-1,1); glVertex3d(-3.8,2,-2);
   glTexCoord2f(-1,1); glVertex3d(-4.7,0.25,-2);
   glTexCoord2f(1,1); glVertex3d(-2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-3.8,2,-2, -3,5,-2, -4,2,-3);
   glTexCoord2f(-1,1); glVertex3d(-3.8,2,-2);
   glTexCoord2f(-1,1); glVertex3d(-4,2,-3);
   glTexCoord2f(1,1); glVertex3d(-3,4,-3);
   glTexCoord2f(1,1); glVertex3d(-3,5,-2);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-3.8,2,-2, -4,2,-3, -5,0,-3);
   glTexCoord2f(-1,1); glVertex3d(-3.8,2,-2);
   glTexCoord2f(-1,1); glVertex3d(-4,2,-3);
   glTexCoord2f(1,1); glVertex3d(-5,0,-3);
   glTexCoord2f(1,1); glVertex3d(-4.7,0.25,-2);
   glEnd();

   // Nose right top panel
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(5,0,-3, 1,1.5,30, 2,0,30);
   glTexCoord2f(-1,-1); glVertex3d(5,+0,-3);
   glTexCoord2f(-1,1); glVertex3d(2,+0,+30);
   glTexCoord2f(0,1); glVertex3d(1,+1.5,+30);
   glTexCoord2f(1,1); glVertex3d(2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(2,2.5,7, 5,0,-3, 4,2,-3);
   glTexCoord2f(-1,-1); glVertex3d(5,+0,-3);
   glTexCoord2f(-1,1); glVertex3d(4,+2,-3);
   glTexCoord2f(1,1); glVertex3d(2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(2,2.5,7, 3,5,-2, 3,5,0);
   glTexCoord2f(1,1); glVertex3d(3,5,0);
   glTexCoord2f(1,0); glVertex3d(3,5,-2);
   glTexCoord2f(0,0); glVertex3d(2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(2,2.5,7, 3.8,2,-2, 3,5,-2);
   glTexCoord2f(-1,1); glVertex3d(3,5,-2);
   glTexCoord2f(-1,1); glVertex3d(3.8,2,-2);
   glTexCoord2f(1,1); glVertex3d(2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(2,2.5,7, 4.7,0.25,-2, 3.8,2,-2);
   glTexCoord2f(-1,1); glVertex3d(3.8,2,-2);
   glTexCoord2f(-1,1); glVertex3d(4.7,0.25,-2);
   glTexCoord2f(1,1); glVertex3d(2,+2.5,+7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(3.8,2,-2, 4,2,-3, 3,5,-2);
   glTexCoord2f(-1,1); glVertex3d(3.8,2,-2);
   glTexCoord2f(-1,1); glVertex3d(4,2,-3);
   glTexCoord2f(1,1); glVertex3d(3,4,-3);
   glTexCoord2f(1,1); glVertex3d(3,5,-2);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(3.8,2,-2, 5,0,-3, 4,2,-3);
   glTexCoord2f(-1,1); glVertex3d(3.8,2,-2);
   glTexCoord2f(-1,1); glVertex3d(4,2,-3);
   glTexCoord2f(1,1); glVertex3d(5,0,-3);
   glTexCoord2f(1,1); glVertex3d(4.7,0.25,-2);
   glEnd();

   // Cockpit window panel
   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_POLYGON);
   normal(3,5,0, -3,5,0, -2,2.5,7);
   glTexCoord2f(0,1); glVertex3d(+3,+5,+0);
   glTexCoord2f(1,0); glVertex3d(-3,+5,+0);
   glTexCoord2f(-1,0); glVertex3d(-2,+2.5,+7);
   glTexCoord2f(0,-1); glVertex3d(+2,+2.5,+7);
   glEnd();

   // Cockpit top panel
   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_POLYGON);
   normal(3,5,0, -3,5,-2, -3,5,0);
   glTexCoord2f(1,0); glVertex3d(+3,+5,+0);
   glTexCoord2f(0,0); glVertex3d(-3,+5,+0);
   glTexCoord2f(0,1); glVertex3d(-3,+5,-2);
   glTexCoord2f(1,1); glVertex3d(+3,+5,-2);
   glEnd();

   // Cockpit back panel
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(-3,5,-2, 3,5,-2, -3,4,-3);
   glTexCoord2f(1,0); glVertex3d(+3,+5,-2);
   glTexCoord2f(0,0); glVertex3d(-3,+5,-2);
   glTexCoord2f(0,1); glVertex3d(-3,+4,-3);
   glTexCoord2f(1,1); glVertex3d(+3,+4,-3);
   glEnd();

   // ---------------------------- Nose panels  ---------------------------
   // Nose top panel
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(2,2.5,7, -2,2.5,7, 1,1.5,30);
   glTexCoord2f(0,0); glVertex3d(-2,+2.5,+7);
   glTexCoord2f(0,1); glVertex3d(+2,+2.5,+7);
   glTexCoord2f(1,0); glVertex3d(+1,+1.5,+30);
   glTexCoord2f(1,1); glVertex3d(-1,+1.5,+30);
   glEnd();

   // Nose left bottom panel
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(-5,0,-3, -1,-1,30, -2,0,30);
   glTexCoord2f(-1,1); glVertex3d(-5,+0,-3);
   glTexCoord2f(-1,0); glVertex3d(-2,+0,+30);
   glTexCoord2f(1,0); glVertex3d(-1,-1,+30);
   glTexCoord2f(1,1); glVertex3d(-3,-3,-3);
   glEnd();

   // Nose right bottom panel
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(5,0,-3, 1,-1,30, 2,0,30);
   glTexCoord2f(-1,1); glVertex3d(+5,+0,-3);
   glTexCoord2f(-1,0); glVertex3d(+2,+0,+30);
   glTexCoord2f(1,0); glVertex3d(+1,-1,+30);
   glTexCoord2f(1,1); glVertex3d(+3,-3,-3);
   glEnd();

   // Nose bottom panel
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(3,-3,-3, -3,-3,-3, 1,-1,30);
   glTexCoord2f(-1,1); glVertex3d(-3,-3,-3);
   glTexCoord2f(-1,0); glVertex3d(+3,-3,-3);
   glTexCoord2f(1,0); glVertex3d(+1,-1,+30);
   glTexCoord2f(1,1); glVertex3d(-1,-1,+30);
   glEnd();

   // Nose tip panel
   glBindTexture(GL_TEXTURE_2D,texture[5]);
   glBegin(GL_POLYGON);
   glNormal3d(0,0,1);
   glTexCoord2f(0,0.75); glVertex3d(+1,-1,+30);
   glTexCoord2f(1,0.25); glVertex3d(+2,+0,+30);
   glTexCoord2f(0.677,0.75); glVertex3d(+1,+1.5,+30);
   glTexCoord2f(0.25,0.75); glVertex3d(-1,+1.5,+30);
   glTexCoord2f(0,0.25); glVertex3d(-2,+0,+30);
   glTexCoord2f(0.25,0); glVertex3d(-1,-1,+30);
   glEnd();

   // --------------- Body top and bottom panels -----------------------
   // Body top panel
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(2,2);
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   glNormal3d(0,1,0);
   glTexCoord2f(1,0.631); glVertex3d(-3,+4,-3);  
   glTexCoord2f(1,0.0901); glVertex3d(+3,+4,-3);
   glTexCoord2f(0.9375,0); glVertex3d(+4,+4,-4);
   glTexCoord2f(0,0.7212); glVertex3d(+4,+4,-19);
   glTexCoord2f(0,0); glVertex3d(-4,+4,-19);
   glTexCoord2f(0.9375,0.7212); glVertex3d(-4,+4,-4);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);

   // Body top panel center
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   glBegin(GL_POLYGON);
   glNormal3d(0,1,0);
   glTexCoord2f(1,0.631); glVertex3d(-2,+4,-2);  
   glTexCoord2f(1,0.0901); glVertex3d(+2,+4,-2);
   glTexCoord2f(0.9375,0); glVertex3d(+3,+4,-3);
   glTexCoord2f(0,0.7212); glVertex3d(+3,+4,-18);
   glTexCoord2f(0,0); glVertex3d(-3,+4,-18);
   glTexCoord2f(0.9375,0.7212); glVertex3d(-3,+4,-3);
   glEnd();
   
   // Body bottom panel
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   glNormal3d(0,-1,0);
   glTexCoord2f(1,0.631); glVertex3d(-3,-3,-3);  
   glTexCoord2f(1,0.0901); glVertex3d(+3,-3,-3);
   glTexCoord2f(0.9375,0); glVertex3d(+3,-3,-4);
   glTexCoord2f(0,0.7212); glVertex3d(+3,-3,-19);
   glTexCoord2f(0,0); glVertex3d(-3,-3,-19);
   glTexCoord2f(0.9375,0.7212); glVertex3d(-3,-3,-4);
   glEnd();

   // --------------- Body left panels - behind cockpit to wings -------
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(-4,4,-4, -5,0,-4, -3,4,-3);
   glTexCoord2f(0,1); glVertex3d(-3,+4,-3);
   glTexCoord2f(1,1); glVertex3d(-4,+4,-4);
   glTexCoord2f(1,0); glVertex3d(-5,+0,-4);
   glTexCoord2f(0,0); glVertex3d(-5,+0,-3);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(-5,0,-4, -4,4,-4, -5,0,-7);
   glTexCoord2f(0,1); glVertex3d(-4,+4,-4);
   glTexCoord2f(0,0); glVertex3d(-5,+0,-4);
   glTexCoord2f(1,0); glVertex3d(-5,+0,-7);
   glTexCoord2f(1,1); glVertex3d(-4,+4,-7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(-5,0,-7, -3,-3,-7, -5,0,-3);
   glTexCoord2f(1,1); glVertex3d(-5,+0,-3);
   glTexCoord2f(0,1); glVertex3d(-5,+0,-7);
   glTexCoord2f(1,0); glVertex3d(-3,-3,-7);
   glTexCoord2f(0,0); glVertex3d(-3,-3,-3);
   glEnd();

   // --------------- Body right panels - behind cockpit to wings -------
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(+4,4,-4, +3,4,-3, +5,0,-4);
   glTexCoord2f(1,1); glVertex3d(+3,+4,-3);
   glTexCoord2f(0,1); glVertex3d(+4,+4,-4);
   glTexCoord2f(0,0); glVertex3d(+5,+0,-4);
   glTexCoord2f(1,0); glVertex3d(+5,+0,-3);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(+5,0,-4, +5,0,-7, +4,4,-4);
   glTexCoord2f(0,1); glVertex3d(+4,+4,-4);
   glTexCoord2f(1,0); glVertex3d(+5,+0,-4);
   glTexCoord2f(0,0); glVertex3d(+5,+0,-7);
   glTexCoord2f(1,1); glVertex3d(+4,+4,-7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(+5,0,-7, +5,0,-3, +3,-3,-7);
   glTexCoord2f(1,1); glVertex3d(+5,+0,-3);
   glTexCoord2f(0,1); glVertex3d(+5,+0,-7);
   glTexCoord2f(0,0); glVertex3d(+3,-3,-7);
   glTexCoord2f(1,0); glVertex3d(+3,-3,-3);
   glEnd();

   // ---------------- Body inner panels - to hide internals ------------
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   glNormal3d(0,0,1);
   glTexCoord2f(0.9,1); glVertex3d(+4,+4,-7);
   glTexCoord2f(1,0.428); glVertex3d(+5,+0,-7);
   glTexCoord2f(0.8,0); glVertex3d(+3,-3,-7);
   glTexCoord2f(0.2,0); glVertex3d(-3,-3,-7);
   glTexCoord2f(0,0.428); glVertex3d(-5,+0,-7);
   glTexCoord2f(0.1,1); glVertex3d(-4,+4,-7);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(3,-3,-19, 4,4,-17, 3,-3,-17);
   glTexCoord2f(1,1); glVertex3d(+4,+4,-17);
   glTexCoord2f(1,0); glVertex3d(+3,-3,-17);
   glTexCoord2f(0,0); glVertex3d(+3,-3,-19);
   glTexCoord2f(0,1); glVertex3d(+4,+4,-19);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_POLYGON);
   normal(-3,-3,-19, -3,-3,-17, -4,4,-17);
   glTexCoord2f(0,1); glVertex3d(-4,+4,-17);
   glTexCoord2f(0,0); glVertex3d(-3,-3,-17);
   glTexCoord2f(1,0); glVertex3d(-3,-3,-19);
   glTexCoord2f(1,1); glVertex3d(-4,+4,-19);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[7]);
   glBegin(GL_POLYGON);
   normal(-3,4,-3, -3,4,-17, -3,-3,-17);
   glTexCoord2f(0,1); glVertex3d(-3,+4,-3);
   glTexCoord2f(1,1); glVertex3d(-3,+4,-17);
   glTexCoord2f(1,0); glVertex3d(-3,-3,-17);
   glTexCoord2f(0,0); glVertex3d(-3,-3,-3);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[7]);
   glBegin(GL_POLYGON);
   normal(+3,4,-3, +3,-3,-17, +3,4,-17);
   glTexCoord2f(1,1); glVertex3d(+3,+4,-3);
   glTexCoord2f(0,1); glVertex3d(+3,+4,-17);
   glTexCoord2f(0,0); glVertex3d(+3,-3,-17);
   glTexCoord2f(1,0); glVertex3d(+3,-3,-3);
   glEnd();

   // ------------------- Body back panels --------------------------------
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   glBegin(GL_POLYGON);
   glNormal3d(0,0,-1);
   glTexCoord2f(0,1); glVertex3d(-4,+4,-19);
   glTexCoord2f(0,0); glVertex3d(-3,-3,-19);
   glTexCoord2f(1,0); glVertex3d(+3,-3,-19);
   glTexCoord2f(1,1); glVertex3d(+4,+4,-19);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_POLYGON);
   glNormal3d(0,0,-1);
   glTexCoord2f(0,0); glVertex3d(-4,+4,-17);
   glTexCoord2f(0,1); glVertex3d(-3,-3,-17);
   glTexCoord2f(1,1); glVertex3d(+3,-3,-17);
   glTexCoord2f(1,0); glVertex3d(+4,+4,-17);
   glEnd();

   // Build wings
   createWings();

   // Disable Textures
   glDisable(GL_TEXTURE_2D);

   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a Tie-Fighter Wing
 *     
 */
static void tFighterWing(double x, double y, double z,
                 double rx, double ry, double rz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,rx,ry,rz);

   // Wing panel
   glBindTexture(GL_TEXTURE_2D,texture[9]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   glNormal3d(1,0,0);
   glTexCoord2f(0.1491,0); glVertex3d(13,0,-5.9560);
   glTexCoord2f(0,0.4931); glVertex3d(13,19.7230,0);
   glTexCoord2f(0.1491,1); glVertex3d(13,40,-5.9560);
   glTexCoord2f(0.7233,1); glVertex3d(13,40,-28.8919);
   glTexCoord2f(0.8807,0.4931); glVertex3d(13,19.730,-35.18);
   glTexCoord2f(0.7233,0); glVertex3d(13,0,-28.8919);
   glEnd();

   // Start the body connecting bridge to wing
   glPushMatrix();
   glTranslated(0,19.7230,-17.59);

   // Create connecting bridge
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,Sin(th),Cos(th));
      glTexCoord2f(0,th*0.0123+.05); glVertex3d(6, 3 * Sin(th), 3 * Cos(th));
      glTexCoord2f(1,th*0.0123+.05); glVertex3d(13, 2 * Sin(th), 2 * Cos(th));
   }
   glEnd();

   // End the body connecting bridge to the wing
   glPopMatrix();

   // End the wings
   glPopMatrix();
}

/*
 *  Draw the Tie-Fighter Cockpit
 *     
 */
static void tFighterCockpit(double size)
{
	
   glEnable(GL_TEXTURE_2D);
	
   //  Save transformation
   glPushMatrix();
   
   //  Offset
   glScaled(size, size, size);

   // Declare local variables
   int ph, th;
	
	/* Front windscreen */
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3d(0, 0, 1);
	
	for (th = 0; th <= 360; th += 45)
	{
		// x = sin(t)
		// y = cos(t)
		glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 1);
		
	}
	glEnd();
	
	// Octa-windows, outside
   glColor3d(0, 0, 0);
   glBegin(GL_QUADS);
   normal(0.2 * Cos(3),0.2 * Sin(3),1.001, 0.47 * Cos(3),0.47 * Sin(3),1.001, 0.47 * Cos(42),0.47 * Sin(42),1.001);
   glVertex3d(0.2 * Cos(3), 0.2 * Sin(3), 1.001);
   glVertex3d(0.47 * Cos(3), 0.47 * Sin(3), 1.001);
   glVertex3d(0.47 * Cos(42), 0.47 * Sin(42), 1.001);
   glVertex3d(0.2 * Cos(42), 0.2 * Sin(42), 1.001);
   glEnd();
   
   glBegin(GL_QUADS);
   normal(0.2 * Cos(48),0.2 * Sin(48),1.001, 0.47 * Cos(48),0.47 * Sin(48),1.001, 0.47 * Cos(87),0.47 * Sin(87),1.001);
   glVertex3d(0.2 * Cos(48), 0.2 * Sin(48), 1.001);
   glVertex3d(0.47 * Cos(48), 0.47 * Sin(48), 1.001);
   glVertex3d(0.47 * Cos(87), 0.47 * Sin(87), 1.001);
   glVertex3d(0.2 * Cos(87), 0.2 * Sin(87), 1.001);
   glEnd();
   
   glBegin(GL_QUADS);
   normal(0.2 * Cos(93),0.2 * Sin(93),1.001, 0.47 * Cos(93),0.47 * Sin(93),1.001, 0.47 * Cos(132),0.47 * Sin(132),1.001);
   glVertex3d(0.2 * Cos(93), 0.2 * Sin(93), 1.001);
   glVertex3d(0.47 * Cos(93), 0.47 * Sin(93), 1.001);
   glVertex3d(0.47 * Cos(132), 0.47 * Sin(132), 1.001);
   glVertex3d(0.2 * Cos(132), 0.2 * Sin(132), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(138),0.2 * Sin(138),1.001, 0.47 * Cos(138),0.47 * Sin(138),1.001, 0.47 * Cos(177),0.47 * Sin(177),1.001);
   glVertex3d(0.2 * Cos(138), 0.2 * Sin(138), 1.001);
   glVertex3d(0.47 * Cos(138), 0.47 * Sin(138), 1.001);
   glVertex3d(0.47 * Cos(177), 0.47 * Sin(177), 1.001);
   glVertex3d(0.2 * Cos(177), 0.2 * Sin(177), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(183),0.2 * Sin(183),1.001, 0.47 * Cos(183),0.47 * Sin(183),1.001, 0.47 * Cos(222),0.47 * Sin(222),1.001);
   glVertex3d(0.2 * Cos(183), 0.2 * Sin(183), 1.001);
   glVertex3d(0.47 * Cos(183), 0.47 * Sin(183), 1.001);
   glVertex3d(0.47 * Cos(222), 0.47 * Sin(222), 1.001);
   glVertex3d(0.2 * Cos(222), 0.2 * Sin(222), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(228),0.2 * Sin(228),1.001, 0.47 * Cos(228),0.47 * Sin(228),1.001, 0.47 * Cos(267),0.47 * Sin(267),1.001);
   glVertex3d(0.2 * Cos(228), 0.2 * Sin(228), 1.001);
   glVertex3d(0.47 * Cos(228), 0.47 * Sin(228), 1.001);
   glVertex3d(0.47 * Cos(267), 0.47 * Sin(267), 1.001);
   glVertex3d(0.2 * Cos(267), 0.2 * Sin(267), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(273),0.2 * Sin(273),1.001, 0.47 * Cos(273),0.47 * Sin(273),1.001, 0.47 * Cos(312),0.47 * Sin(312),1.001);
   glVertex3d(0.2 * Cos(273), 0.2 * Sin(273), 1.001);
   glVertex3d(0.47 * Cos(273), 0.47 * Sin(273), 1.001);
   glVertex3d(0.47 * Cos(312), 0.47 * Sin(312), 1.001);
   glVertex3d(0.2 * Cos(312), 0.2 * Sin(312), 1.001);
   glEnd();

   glBegin(GL_QUADS);
   normal(0.2 * Cos(318),0.2 * Sin(318),1.001, 0.47 * Cos(318),0.47 * Sin(318),1.001, 0.47 * Cos(357),0.47 * Sin(357),1.001);
   glVertex3d(0.2 * Cos(318), 0.2 * Sin(318), 1.001);
   glVertex3d(0.47 * Cos(318), 0.47 * Sin(318), 1.001);
   glVertex3d(0.47 * Cos(357), 0.47 * Sin(357), 1.001);
   glVertex3d(0.2 * Cos(357), 0.2 * Sin(357), 1.001);
   glEnd();
   
   // Center window
   glBegin(GL_POLYGON);
   glColor3d(0, 0, 0);
   glNormal3d(0,0,1);
   glVertex3d(0.17 * Cos(0), 0.17 * Sin(0), 1.001);
   glVertex3d(0.17 * Cos(45), 0.17 * Sin(45), 1.001);
   glVertex3d(0.17 * Cos(90), 0.17 * Sin(90), 1.001);
   glVertex3d(0.17 * Cos(135), 0.17 * Sin(135), 1.001);
   glVertex3d(0.17 * Cos(180), 0.17 * Sin(180), 1.001);
   glVertex3d(0.17 * Cos(225), 0.17 * Sin(225), 1.001);
   glVertex3d(0.17 * Cos(270), 0.17 * Sin(270), 1.001);
   glVertex3d(0.17 * Cos(315), 0.17 * Sin(315), 1.001);
   glVertex3d(0.17 * Cos(315), 0.17 * Sin(315), 1.001);
   glVertex3d(0.17 * Cos(360), 0.17 * Sin(360), 1.001);
   glEnd();
	
	// Front windscreen border trim (joins windscreen to fuselage)
	glBegin(GL_QUAD_STRIP);
	
	double color = 0.5;
	for (th = 0; th <= 360; th += 45, color += 0.015)
	{
		glColor3d(color, color, color);
		
		// x = sin(t)
		// y = cos(t)
		glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 1);
		glVertex3d(0.5 * Cos(th), 0.5 * Sin(th), 0.7);
		
	}
	glEnd();
	/* End front windscreen */
	
	glColor3d(1, 1, 1);

   glBindTexture(GL_TEXTURE_2D, cockpitTex);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
		  glTexCoord2d(th/360.0,ph/180.0+0.5);
		  Vertex(th,ph);
		  glTexCoord2d(th/360.0,(ph+inc)/180.0+0.5);
		  Vertex(th,ph+inc);
      }
      glEnd();
   }

   //  Restore transformation
   glPopMatrix();
	
   glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw a Tie-Fighter
 *     
 */
static void tFighter(double x, double y, double z,
                 double s,
                 double rx, double ry, double rz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   
   //  Offset
   glTranslated(x,y,z);
   glScaled(s,s,s);
   glRotated(th,rx,ry,rz);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);

   // Create Tie Fighter Wings and center on origin
   tFighterWing(0,-19.723,17.19, 0,0,0, 0);
   tFighterWing(0,-19.723,-17.19, 0,1,0, 180);
   tFighterCockpit(7);

   // Disable Textures
   glDisable(GL_TEXTURE_2D);

   //  Undo transofrmations
   glPopMatrix();
}


static void laserBeam(double x, double y, double z, double angle, double rx, double ry, double rz, char color) {

   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(angle, rx, ry, rz);



   glDisable(GL_LIGHTING);

   if (color == 'r')
      glBindTexture(GL_TEXTURE_2D, laser[0]);
else if (color == 'g')
      glBindTexture(GL_TEXTURE_2D, laser[1]);


   glPushMatrix();
   //glRotated(0, 0, 0, 1);

   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3d(-1, -25 + laserAnim, 0);
   glTexCoord2f(1, 0); glVertex3d(-1, 25 + laserAnim, 0);
   glTexCoord2f(1, 1); glVertex3d(1, 25 + laserAnim, 0);
   glTexCoord2f(0, 1); glVertex3d(1, -25 + laserAnim, 0);

   glTexCoord2f(0, 0); glVertex3d(0, -25 + laserAnim, 1);
   glTexCoord2f(1, 0); glVertex3d(0, 25 + laserAnim, 1);
   glTexCoord2f(1, 1); glVertex3d(0, 25 + laserAnim, -1);
   glTexCoord2f(0, 1); glVertex3d(0, -25 + laserAnim, -1);
   glEnd();

   glPopMatrix();


   glEnable(GL_LIGHTING);
   glPopMatrix();
}
/*
 *  Draw a trench turrent top
 *     
 */
static void turretTop(double rt)
{
   //  Save transformation
   glPushMatrix();
   
   //  Offset
   glTranslated(0,0.1,0);
   glRotated(rt,0,1,0);


   // Top panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   glNormal3d(0,1,0);
   glTexCoord2f(0,1); glVertex3d(-2.5,12,-2.5);
   glTexCoord2f(0,0); glVertex3d(-2.5,12,2.5);
   glTexCoord2f(1,0); glVertex3d(2.5,12,2.5);
   glTexCoord2f(1,1); glVertex3d(2.5,12,-2.5);
   glEnd();

   // Bottom panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   glNormal3d(0,-1,0);
   glTexCoord2f(0,1); glVertex3d(-3,8,-3);
   glTexCoord2f(0,0); glVertex3d(-3,8,3);
   glTexCoord2f(1,0); glVertex3d(3,8,3);
   glTexCoord2f(1,1); glVertex3d(3,8,-3);
   glEnd();

   // Front panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-3,8,3, 3,8,3, -2.5,12,2.5);
   glTexCoord2f(0,0); glVertex3d(-3,8,3);
   glTexCoord2f(1,0); glVertex3d(3,8,3);
   glTexCoord2f(0.92,0.67); glVertex3d(2.5,12,2.5);
   glTexCoord2f(0.08,0.67); glVertex3d(-2.5,12,2.5);
   glEnd();

   // Right panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(3,8,3, 3,8,-3, 2.5,12,2.5);
   glTexCoord2f(0,0); glVertex3d(3,8,3);
   glTexCoord2f(1,0); glVertex3d(3,8,-3);
   glTexCoord2f(0.92,0.67); glVertex3d(2.5,12,-2.5);
   glTexCoord2f(0.08,0.67); glVertex3d(2.5,12,2.5);
   glEnd();

   // Left panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-3,8,-3, -3,8,3, -2.5,12,-2.5);
   glTexCoord2f(0,0); glVertex3d(-3,8,-3);
   glTexCoord2f(1,0); glVertex3d(-3,8,3);
   glTexCoord2f(0.92,0.67); glVertex3d(-2.5,12,2.5);
   glTexCoord2f(0.08,0.67); glVertex3d(-2.5,12,-2.5);
   glEnd();

   // Back panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(3,8,-3, -3,8,-3, 2.5,12,-2.5);
   glTexCoord2f(0,0); glVertex3d(3,8,-3);
   glTexCoord2f(1,0); glVertex3d(-3,8,-3);
   glTexCoord2f(0.92,0.67); glVertex3d(-2.5,12,-2.5);
   glTexCoord2f(0.08,0.67); glVertex3d(2.5,12,-2.5);
   glEnd();

   // Push matrix to built turret guns
   glPushMatrix();

   // Adjust gun
   glTranslated(0,10.5,3);
   glRotated(-20,1,0,0);
   int th;

   // Build left gun
   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(-1, 0, -1);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,-1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(-1 + 0.6 * Cos(th), 0.6 * Sin(th), -1);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(-1 + 0.6 * Cos(th), 0.6 * Sin(th), -1);
      glTexCoord2f(1,th*0.00274); glVertex3d(-1 + 0.6 * Cos(th), 0.6 * Sin(th), 0);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(-1, 0, 0);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,-1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(-1 + 0.8 * Cos(th), 0.8 * Sin(th), 0);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(-1, 0, 0.3);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(-1 + 0.8 * Cos(th), 0.8 * Sin(th), 0.3);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(-1 + 0.8 * Cos(th), 0.8 * Sin(th), 0);
      glTexCoord2f(1,th*0.00274); glVertex3d(-1 + 0.8 * Cos(th), 0.8 * Sin(th), 0.3);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(-1, 0, 2);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(-1 + 0.5 * Cos(th), 0.5 * Sin(th), 2);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(-1 + 0.5 * Cos(th), 0.5 * Sin(th), 0.3);
      glTexCoord2f(1,th*0.00274); glVertex3d(-1 + 0.5 * Cos(th), 0.5 * Sin(th), 2);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(-1, 0, 5);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(-1 + 0.3 * Cos(th), 0.3 * Sin(th), 5);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(-1 + 0.3 * Cos(th), 0.3 * Sin(th), 2);
      glTexCoord2f(1,th*0.00274); glVertex3d(-1 + 0.3 * Cos(th), 0.3 * Sin(th), 5);
   }
   glEnd();

   // Build right gun
   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(1, 0, -1);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,-1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(1 + 0.6 * Cos(th), 0.6 * Sin(th), -1);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(1 + 0.6 * Cos(th), 0.6 * Sin(th), -1);
      glTexCoord2f(1,th*0.00274); glVertex3d(1 + 0.6 * Cos(th), 0.6 * Sin(th), 0);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(1, 0, 0);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,-1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(1 + 0.8 * Cos(th), 0.8 * Sin(th), 0);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(1, 0, 0.3);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(1 + 0.8 * Cos(th), 0.8 * Sin(th), 0.3);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(1 + 0.8 * Cos(th), 0.8 * Sin(th), 0);
      glTexCoord2f(1,th*0.00274); glVertex3d(1 + 0.8 * Cos(th), 0.8 * Sin(th), 0.3);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(1, 0, 2);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(1 + 0.5 * Cos(th), 0.5 * Sin(th), 2);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(1 + 0.5 * Cos(th), 0.5 * Sin(th), 0.3);
      glTexCoord2f(1,th*0.00274); glVertex3d(1 + 0.5 * Cos(th), 0.5 * Sin(th), 2);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(1, 0, 5);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(0,0,1);
      glTexCoord2f(0.032 + 0.1 * Cos(th), 0.075 + 0.1 * Sin(th)); glVertex3d(1 + 0.3 * Cos(th), 0.3 * Sin(th), 5);
      
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUAD_STRIP);
   for (th = 0; th <= 360; th += 5)
   {
      glNormal3d(Cos(th),Sin(th),0);
      glTexCoord2f(0,th*0.00274); glVertex3d(1 + 0.3 * Cos(th), 0.3 * Sin(th), 2);
      glTexCoord2f(1,th*0.00274); glVertex3d(1 + 0.3 * Cos(th), 0.3 * Sin(th), 5);
   }
   glEnd();

   //  Undo transofrmations
   glPopMatrix();



   //  Undo transofrmations
   glPopMatrix();

}

/*
 *  Draw a trench turret base
 *     
 */
static void turretBase()
{
   // Top panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   glNormal3d(0,1,0);
   glTexCoord2f(0,1); glVertex3d(-3,8,-3);
   glTexCoord2f(0,0); glVertex3d(-3,8,3);
   glTexCoord2f(1,0); glVertex3d(3,8,3);
   glTexCoord2f(1,1); glVertex3d(3,8,-3);
   glEnd();

   // Bottom panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   glNormal3d(0,-1,0);
   glTexCoord2f(1,0); glVertex3d(-4,0,-4);
   glTexCoord2f(1,1); glVertex3d(-4,0,4);
   glTexCoord2f(0,1); glVertex3d(4,0,4);
   glTexCoord2f(0,0); glVertex3d(4,0,-4);
   glEnd();

   // Front panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-4,0,4, 4,0,4, -3,8,3);
   glTexCoord2f(0,0); glVertex3d(-4,0,4);
   glTexCoord2f(1,0); glVertex3d(4,0,4);
   glTexCoord2f(0.88,1); glVertex3d(3,8,3);
   glTexCoord2f(0.13,1); glVertex3d(-3,8,3);
   glEnd();

   // Right panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(4,0,4, 4,0,-4, 3,8,3);
   glTexCoord2f(0,0); glVertex3d(4,0,4);
   glTexCoord2f(1,0); glVertex3d(4,0,-4);
   glTexCoord2f(0.88,1); glVertex3d(3,8,-3);
   glTexCoord2f(0.13,1); glVertex3d(3,8,3);
   glEnd();

   // Left panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(-4,0,-4, -4,0,4, -3,8,-3);
   glTexCoord2f(0,0); glVertex3d(-4,0,-4);
   glTexCoord2f(1,0); glVertex3d(-4,0,4);
   glTexCoord2f(0.88,1); glVertex3d(-3,8,3);
   glTexCoord2f(0.13,1); glVertex3d(-3,8,-3);
   glEnd();

   // Back panel
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_POLYGON);
   glColor3f(0.75,0.75,0.75);
   normal(4,0,-4, -4,0,-4, 3,8,-3);
   glTexCoord2f(0,0); glVertex3d(4,0,-4);
   glTexCoord2f(1,0); glVertex3d(-4,0,-4);
   glTexCoord2f(0.88,1); glVertex3d(-3,8,-3);
   glTexCoord2f(0.13,1); glVertex3d(3,8,-3);
   glEnd();
}

/*
 *  Draw a trench turret
 *     
 */
static void tTurret(double x, double y, double z,
                 double s,
                 double rx, double ry, double rz,
                 double th, double rt, double topRot)
{
   //  Save transformation
   glPushMatrix();
   
   //  Offset
   glTranslated(x,y, z);
   glScaled(s,s,s);
   glRotated(th,rx,ry,rz);

   // Create turret base
   turretBase();

   // Create turret top
   turretTop(topRot);

   //  Undo transofrmations
   glPopMatrix();
}

static void scaffoldBridge(double x, double y, double z) {
   glPushMatrix();
   glTranslated(x, y, z);

   double i; 
   int draw = rand() % 2;


   glBindTexture(GL_TEXTURE_2D, texture[11]);



   // Main horizontal cross members
   // Backface
   // Top
   glNormal3d(0, 0, -1);
   glBegin(GL_POLYGON);
   glTexCoord2f(0, 0); glVertex3d(-30, 39, 0);
   glTexCoord2f(3 * 1, 0); glVertex3d(31, 39, 0);
   glTexCoord2f(3 * 1, 0.5); glVertex3d(31, 40, 0);
   glTexCoord2f(0, 0.5); glVertex3d(-30, 40, 0);
   glEnd();

   // Bottom
   glBegin(GL_POLYGON);
   glTexCoord2f(0, 0); glVertex3d(-30, 33, 0);
   glTexCoord2f(3 * 1, 0); glVertex3d(31, 33, 0);
   glTexCoord2f(3 * 1, 0.5); glVertex3d(31, 34, 0);
   glTexCoord2f(0, 0.5); glVertex3d(-30, 34, 0);
   glEnd();

   for (i = -30.5; i <= 29.5; i += 7.5) {
      // Vertical bars
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3d(i, 34, 0);
      glTexCoord2f(0.1, 0); glVertex3d(i + 1, 34, 0);
      glTexCoord2f(0.1, 1); glVertex3d(i + 1, 39, 0);
      glTexCoord2f(0, 1); glVertex3d(i, 39, 0);
      glEnd();

      // Diagonal bars
      glBegin(GL_POLYGON);
      glTexCoord2f(0, 0.1); glVertex3d(i + 1, 34 + sqrt(2)/2, 0);
      glTexCoord2f(0, 0); glVertex3d(i + 1, 34, 0);
      glTexCoord2f(0.1, 0); glVertex3d(i + 1 + sqrt(2)/2, 34, 0);
      glTexCoord2f(0.7 * 1, 0.7 * 0.6); glVertex3d(i + 7.5, 39 - sqrt(2)/2, 0);
      glTexCoord2f(0.7 * 1, 0.7 * 0.7); glVertex3d(i + 7.5, 39, 0);
      glTexCoord2f(0.7 * 0.7, 0.7 * 0.7); glVertex3d(i + 7.5 - sqrt(2)/2, 39, 0);
      glEnd();
   }

   // Main horizontal cross members
   // Frontface
   // Top
   glNormal3d(0, 0, -1);
   glBegin(GL_POLYGON);
   glTexCoord2f(0, 0); glVertex3d(-30, 39, 7);
   glTexCoord2f(3 * 1, 0); glVertex3d(31, 39, 7);
   glTexCoord2f(3 * 1, 0.5); glVertex3d(31, 40, 7);
   glTexCoord2f(0, 0.5); glVertex3d(-30, 40, 7);
   glEnd();

   // Bottom
   glBegin(GL_POLYGON);
   glTexCoord2f(0, 0); glVertex3d(-30, 33, 7);
   glTexCoord2f(3 * 1, 0); glVertex3d(31, 33, 7);
   glTexCoord2f(3 * 1, 0.5); glVertex3d(31, 34, 7);
   glTexCoord2f(0, 0.5); glVertex3d(-30, 34, 7);
   glEnd();

   for (i = -30.5; i <= 29.5; i += 7.5) {
      // Vertical bars
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3d(i, 34, 7);
      glTexCoord2f(0.1, 0); glVertex3d(i + 1, 34, 7);
      glTexCoord2f(0.1, 1); glVertex3d(i + 1, 39, 7);
      glTexCoord2f(0, 1); glVertex3d(i, 39, 7);
      glEnd();

      // Diagonal bars
      glBegin(GL_POLYGON);
      glTexCoord2f(0, 0.1); glVertex3d(i + 1, 34 + sqrt(2)/2, 7);
      glTexCoord2f(0, 0); glVertex3d(i + 1, 34, 7);
      glTexCoord2f(0.1, 0); glVertex3d(i + 1 + sqrt(2)/2, 34, 7);
      glTexCoord2f(0.7 * 1, 0.7 * 0.6); glVertex3d(i + 7.5, 39 - sqrt(2)/2, 7);
      glTexCoord2f(0.7 * 1, 0.7 * 0.7); glVertex3d(i + 7.5, 39, 7);
      glTexCoord2f(0.7 * 0.7, 0.7 * 0.7); glVertex3d(i + 7.5 - sqrt(2)/2, 39, 7);
      glEnd();
   }

   // Top closure
   glNormal3d(0, 1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3d(-30, 40, 7);
   glTexCoord2f(3 * 1, 0); glVertex3d(30, 40, 7);
   glTexCoord2f(3 * 1, 1); glVertex3d(30, 40, 0);
   glTexCoord2f(0, 1); glVertex3d(-30, 40, 0);
   glEnd();

   // Bottom closure
   glNormal3d(0, -1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3d(-30, 33, 7);
   glTexCoord2f(3 * 1, 0); glVertex3d(30, 33, 7);
   glTexCoord2f(3 * 1, 1); glVertex3d(30, 33, 0);
   glTexCoord2f(0, 1); glVertex3d(-30, 33, 0);
   glEnd();

   glPopMatrix();
}


static void trench(double x, double y, double z) {

   glEnable(GL_TEXTURE_2D);

   // Save matrix and adjust position
   glPushMatrix();
   glTranslated(x, y, z);

   int i;


   // Starboard side lasers
   // 1
   laserBeam(-100, 0, 200, -45, 0, 0, 1, 'g');
   // 2
   laserBeam(-300, 0, 400, -45, 0, 0, 1, 'g');
   // 3
   laserBeam(-500, 0, 600, -45, 0, 0, 1, 'g');
   // 4
   laserBeam(-700, 0, 100, -45, 0, 0, 1, 'g');
   // 5
   laserBeam(-800, 0, 300, -45, 0, 0, 1, 'g');
   // 6
   laserBeam(-100, 0, 500, -60, 0, 0, 1, 'g');
   // 7
   laserBeam(-300, 0, 700, -60, 0, 0, 1, 'g');
   // 8
   laserBeam(-500, 0, 800, -70, 0, 0, 1, 'g');
   // 9
   laserBeam(-700, 0, 700, -50, 0, 0, 1, 'g');
   // 10
   laserBeam(-600, 0, 800, -60, 0, 0, 1, 'g');

   // Port side lasers
   // 1
   laserBeam(800, 0, 400, 40, 0, 0, 1, 'g');
   // 2
   laserBeam(450, 0, 600, 45, 0, 0, 1, 'g');
   // 3
   laserBeam(650, 0, 800, 45, 0, 0, 1, 'g');
   // 4
   laserBeam(540, 0, 700, 45, 0, 0, 1, 'g');
   // 5
   laserBeam(680, 0, 300, 30, 0, 0, 1, 'g');
   // 6
   laserBeam(120, 0, 500, 60, 0, 0, 1, 'g');
   // 7
   laserBeam(390, 0, 700, 60, 0, 0, 1, 'g');
   // 8
   laserBeam(540, 0, 800, 70, 0, 0, 1, 'g');
   // 9
   laserBeam(720, 0, 100, 50, 0, 0, 1, 'g');
   // 10
   laserBeam(980, 0, 1200, 60, 0, 0, 1, 'g');
   
   // Forward lasers
   // 1
   laserBeam(0, 30, 400, -85, 1, 0, 0, 'g');
   // 2
   laserBeam(0, 30, 600, -76, 1, 0, 0, 'g');
   // 3
   laserBeam(0, 30, 800, -85, 1, 0, 0, 'g');
   // 4
   laserBeam(0, 30, 1000, -89, 1, 0, 0, 'g');
   // 5
   laserBeam(0, 30, 300, -78, 1, 0, 0, 'g');

   // Red lasers counter
   // 1
   laserBeam(200, 1000, 800, 140, 0, 0, 1, 'r');
   // 2
   laserBeam(-450, 800, 800, -120, 0, 0, 1, 'r');
   // 3
   laserBeam(250, 600, 800, 130, 0, 0, 1, 'r');
   // 4
   laserBeam(-540, 1200, 800, -130, 0, 0, 1, 'r');
   // 5
   laserBeam(350, 400, 800, 125, 0, 0, 1, 'r');
   // 6
   laserBeam(-120, 700, 800, -140, 0, 0, 1, 'r');
   // 7
   laserBeam(390, 300, 800, 130, 0, 0, 1, 'r');
   // 8
   laserBeam(-540, 1400, 800, -134, 0, 0, 1, 'r');
   // 9
   laserBeam(500, 850, 800, 134, 0, 0, 1, 'r');
   // 10
   laserBeam(-400, 425, 800, -119, 0, 0, 1, 'r');


   // Repeat factor of trench texture
   double rep = 14;

   // Builds trench in panels for animation
   for (i = 10000; i >= -10000; i-=1000) {

      glBindTexture(GL_TEXTURE_2D, texture[7]);
      // Floor
      glBegin(GL_QUADS);
      glNormal3d(0, 1, 0);
      glTexCoord2f(0, 0); glVertex3d(-30, 0, i + trenchAnim);
      glTexCoord2f(1, 0); glVertex3d(30, 0, i + trenchAnim);
      glTexCoord2f(1, 4); glVertex3d(30, 0, i - 1000 + trenchAnim);
      glTexCoord2f(0, 4); glVertex3d(-30, 0, i - 1000 + trenchAnim);
      glEnd();

      glBindTexture(GL_TEXTURE_2D, trenchTex[0]);

      // Port side
      glBegin(GL_QUADS);
      glNormal3d(-1, 0, 0);
      glTexCoord2f(0, 0); glVertex3d(30, 0, i - 1000 + trenchAnim);
      glTexCoord2f(rep * 1, 0); glVertex3d(30, 0, i + trenchAnim);
      glTexCoord2f(rep * 1, 1); glVertex3d(30, 40, i + trenchAnim);
      glTexCoord2f(0, 1); glVertex3d(30, 40, i - 1000 + trenchAnim);
      glEnd();

      // Starboard side
      glBegin(GL_QUADS);
      glNormal3d(1, 0, 0);
      glTexCoord2f(0, 0); glVertex3d(-30, 0, i + trenchAnim);
      glTexCoord2f(rep * 1, 0); glVertex3d(-30, 0, i - 1000 + trenchAnim);
      glTexCoord2f(rep * 1, 1); glVertex3d(-30, 40, i - 1000 + trenchAnim);
      glTexCoord2f(0, 1); glVertex3d(-30, 40, i + trenchAnim);
      glEnd();


      scaffoldBridge(0, 0, i + trenchAnim);

      // Create turret
      tTurret(-40, 40, i + 500 +trenchAnim, 1, 0,0,0, 0, 0, 90);
      tTurret(-40, 40, i + 1000 +trenchAnim, 1, 0,0,0, 0, 0, 90);
      tTurret(40, 40, i + 500 +trenchAnim, 1, 0,0,0, 0, 0, -90);
      tTurret(40, 40, i + 1000 +trenchAnim, 1, 0,0,0, 0, 0, -90);

   }


   

   glPopMatrix();

   glDisable(GL_TEXTURE_2D);

}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();

      double Ex = 0.0;
      double Ey = 0.0;
      double Ez = 0.0;

   if (camera == 0) {

      Ex = -0.01 * dim*Sin(th)*Cos(ph);
      Ey = 0.01 * dim        *Sin(ph);
      Ez = 0.01 * dim*Cos(th)*Cos(ph);

   } else if (camera == 1) {

      Ex = 0;
      Ey = 12.155372;
      Ez = -68.936543;

   } else if (camera == 2) {

      Ex = 0.0;
      Ey =  18.9;
      Ez =  70.5;

   } 


   // Call gluLookAt
   gluLookAt(Ex,Ey,Ez, 0,0,0 , 0,Cos(ph),0);
   


   // Draw skybox
   skybox(3.5*dim);

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Translate intensity to color vectors
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};

   //  Light position
   float Position[]  = {0,100,-100,1.0};

   //  Draw light position as ball (still no lighting here)
   glColor3f(1,1,1);
   ball(Position[0],Position[1],Position[2] , 0.1);

   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);

   //  Enable lighting
   glEnable(GL_LIGHTING);

   //  Location of viewer for specular calculations
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);

   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   //  Enable light 0
   glEnable(GL_LIGHT0);

   //  Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);

   // Create Trench
   trench(0, -10, 0);

   // Create XWING
   xWing(5 * Cos(zh) , 3 *  Sin(zh), 50, 0.3, 0,0,1, 20 * Cos(zh));

   vader(10 * Cos(zh), 4 * Sin(zh), -20, 6/5, 20 * Cos(zh), 0, 0, 1);

   // Create Tie-Fighter
   tFighter(-10,-2,-40, 0.2, 0,0,0, 0);
   tFighter(10,0,-40, 0.2, 0,0,0, 0);
   
   //  Done - disable lighting
   glDisable(GL_LIGHTING);
   
   //  Render the scene and make it visible
   //ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  Idle function
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);

   
   rotateZ = fmod(60 * t/1000, 360.0);


   // Trench animation
   if (!pause) {
      if (trenchAnim < -2000){
         trenchAnim = 0;
      }

      if (xwing_d == 1){
         if (xwing_x == 15){
            xwing_d = -1;
            xwing_r = 15;
         }
         else{
            xwing_x++;
         }
      }

      if (xwing_d == -1){
         if (xwing_x == -15){
            xwing_d = 1;
            xwing_r = -15;
         }
         else{
            xwing_x--;
         }
      }

      trenchAnim -= 3;
   }
   
   laserAnim = fmod(500*t, 819);

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void timer(int value) {

}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 100;
   
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 100;
   
   //  Smooth color model
   else if (key == GLUT_KEY_F1)
      smooth = 1-smooth;
   
   //  Local Viewer
   else if (key == GLUT_KEY_F2)
      local = 1-local;
   else if (key == GLUT_KEY_F3)
      distance = (distance==1) ? 5 : 1;
   
   //  Toggle ball increment
   else if (key == GLUT_KEY_F8)
      inc = (inc==10)?3:10;
   
   //  Flip sign
   else if (key == GLUT_KEY_F9)
      one = -one;
   
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   
   //  Update projection
   Project(view?fov:0,asp,dim);
   
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   
   //  Reset view angle
   else if (ch == 'r')
      th = ph = 0;
   
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   else if (ch == '0') {
      camera = 0;
      move = 1;
   }
   else if (ch == '1') {
      camera = 1;
      move = 1;
   } 
   else if (ch == '2') {
      camera = 2;
      move = 1;
   }
   else if (ch == ' ') {
      pause = 1 - pause;

      if (move && pause) {
         move = 0;
      } else if (!pause && !move) {
         move = 1;
      }
   }


   //  Translate shininess power to value (-1 => 0)
   shinyvec[0] = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project(view?fov:0,asp,dim);
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(view?fov:0,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{

   // Random number seed for trench attribute generation
   srand(time(NULL));

   //  Initialize GLUT
   glutInit(&argc,argv);
   
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(800,800);
   glutCreateWindow("Chris Jordan & Jeremy Granger: Star Wars IV - Death Star Trench Scene");
   
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   
   // Set keyboard functions
   glutSpecialFunc(special);
   glutKeyboardFunc(key);

   // Set idle functions
   glutIdleFunc(idle);

   //  Load textures
   texture[0] = LoadTexBMP("img0.bmp");
   texture[1] = LoadTexBMP("img1.bmp");
   texture[2] = LoadTexBMP("img2.bmp");
   texture[3] = LoadTexBMP("img3.bmp");
   texture[4] = LoadTexBMP("img4.bmp");
   texture[5] = LoadTexBMP("img5.bmp");
   texture[6] = LoadTexBMP("img6.bmp");
   texture[7] = LoadTexBMP("img7.bmp");
   texture[8] = LoadTexBMP("img8.bmp");
   texture[9] = LoadTexBMP("tie-fighter-wing-panel.bmp");
   texture[10] = LoadTexBMP("img10.bmp");
   texture[11] = LoadTexBMP("Vinyl_Gray_2.bmp");
   texture[12] = LoadTexBMP("red_material.bmp");
   texture[13] = LoadTexBMP("Vinyl_Gray_1.bmp");
   texture[14] = LoadTexBMP("Carpet_Gray.bmp");
   texture[15] = LoadTexBMP("jet-engine.bmp");
   texture[16] = LoadTexBMP("Metal_sq.bmp");
   texture[17] = LoadTexBMP("vader_tie_panel.bmp");
	
	cockpitTex = LoadTexBMP("TIECockpit.bmp");

   space[0] = LoadTexBMP("neg-z.bmp");
   space[1] = LoadTexBMP("neg-x.bmp");
   space[2] = LoadTexBMP("pos-z.bmp");
   space[3] = LoadTexBMP("pos-x.bmp");
   space[4] = LoadTexBMP("pos-y.bmp");
   space[5] = LoadTexBMP("neg-y.bmp");

   trenchTex[0] = LoadTexBMP("trench_face.bmp");

   laser[0] = LoadTexBMP("laserRed.bmp");
   laser[1] = LoadTexBMP("laserGreen.bmp");

   // Set clock
   ot = clock();

   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
