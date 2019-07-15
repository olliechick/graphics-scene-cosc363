//  ========================================================================
//  Assignment 1 for COSC363, 2018S1, University of Canterbury.
//  Some content adapted from labs 1-4.
//
//  By Ollie Chick (och26).
//  ========================================================================

#include <math.h>
#include <iostream>
#include <GL/freeglut.h>
#include "loadBMP.h"
#include "loadTGA.h"
using namespace std;

#define GL_CLAMP_TO_EDGE 0x812F   //To get rid of seams between textures

#define PI 3.14159265

#define FLOOR_HEIGHT -3
#define TICK 30

#define BUILDING_SIZE 100
#define DOOR_HEIGHT 2*BUILDING_SIZE/5
#define DOOR_WIDTH BUILDING_SIZE/5
#define DOOR_LEFT_SIDE BUILDING_SIZE/2 - DOOR_WIDTH/2
#define DOOR_RIGHT_SIDE BUILDING_SIZE/2 + DOOR_WIDTH/2

#define CONE_HEIGHT 14
#define CONE_WIDTH 4
#define NUMBER_OF_TEXTURES 11
#define NUMBER_OF_CAMERA_MODES 1

GLuint txId[NUMBER_OF_TEXTURES];
GLUquadricObj*	q;
float theta = 0;

float cam_angle = 0;
float eye_x=BUILDING_SIZE/2, eye_y = BUILDING_SIZE/5, eye_z=BUILDING_SIZE;  //Camera parameters
float look_x = eye_x + 100*sin(cam_angle);
float look_y = eye_y;
float look_z = eye_z - 100*cos(cam_angle);
int camera_mode = 0;

//--------------------------------------------------------------------------------
void loadTexture()	 
{
	glGenTextures(NUMBER_OF_TEXTURES, txId);   //Get 2 texture IDs 
	
	// *** left ***
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	loadTGA("side_1.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** front ***
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	loadTGA("side_2.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** right ***
	glBindTexture(GL_TEXTURE_2D, txId[2]);
	loadTGA("side_0.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** back***
	glBindTexture(GL_TEXTURE_2D, txId[3]);
	loadTGA("side_3.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** top ***
	glBindTexture(GL_TEXTURE_2D, txId[4]);
	loadTGA("side_4.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** down ***
	glBindTexture(GL_TEXTURE_2D, txId[5]);
	loadTGA("side_5.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// *** brick ***
	glBindTexture(GL_TEXTURE_2D, txId[6]);
	loadTGA("brick.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	// *** rocket ***
	glBindTexture(GL_TEXTURE_2D, txId[7]);  //Use this texture name for the following OpenGL texture
	loadBMP("rocket.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// *** icecream ***
	glBindTexture(GL_TEXTURE_2D, txId[8]);  //Use this texture name for the following OpenGL texture
	loadBMP("icecream_sym.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
	

//--- A marble, offset by 10 on the x axis --------------------------------
void marble() {
    glPushMatrix();
        glTranslatef(10, 0, 0);
        glutSolidSphere(3, 50, 50);
    glPopMatrix();
}

//---- Normal ------------------------------------------------------------

void normal(float x1, float y1, float z1, 
            float x2, float y2, float z2,
		      float x3, float y3, float z3 )
{
	  float nx, ny, nz;
	  nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
	  ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
	  nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);

      glNormal3f(nx, ny, nz);
}
//-- Ground Plane --------------------------------------------------------
void ground_plane() {
    float white[4] = {1., 1., 1., 1.};
    float black[4] = {0};
    glColor4f(0.7, 0.7, 0.7, 1.0);  //The floor is gray in colour
    glNormal3f(0.0, 1.0, 0.0);

    glMaterialfv(GL_FRONT, GL_SPECULAR, black); //disable specular

    //The floor is made up of several tiny squares on a 200x200 grid. Each square has a unit size.
    glBegin(GL_QUADS);
    for (int i = -200; i < 200; i++) {
        for (int j = -200; j < 200; j++) {
            glVertex3f(i, FLOOR_HEIGHT, j);
            glVertex3f(i, FLOOR_HEIGHT, j + 1);
            glVertex3f(i + 1, FLOOR_HEIGHT, j + 1);
            glVertex3f(i + 1, FLOOR_HEIGHT, j);
        }
    }
    glEnd();

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);

}
//========================================================================================

void skybox(){
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1.0);
	
	int SkyboxXSpan = 2000;
	int SkyboxYSpan = 1000;
	int SkyboxZSpan = 2000;
	
	// Vertices - see skybox diagram
	float A[] = {eye_x-SkyboxXSpan/2, eye_y+SkyboxYSpan/2, eye_z+SkyboxZSpan/2};
	float B[] = {eye_x-SkyboxXSpan/2, eye_y+SkyboxYSpan/2, eye_z-SkyboxZSpan/2};
	float C[] = {eye_x+SkyboxXSpan/2, eye_y+SkyboxYSpan/2, eye_z-SkyboxZSpan/2};
	float D[] = {eye_x+SkyboxXSpan/2, eye_y+SkyboxYSpan/2, eye_z+SkyboxZSpan/2};
	float E[] = {eye_x-SkyboxXSpan/2, eye_y-SkyboxYSpan/2, eye_z+SkyboxZSpan/2};
	float F[] = {eye_x-SkyboxXSpan/2, eye_y-SkyboxYSpan/2, eye_z-SkyboxZSpan/2};
	float G[] = {eye_x+SkyboxXSpan/2, eye_y-SkyboxYSpan/2, eye_z-SkyboxZSpan/2};
	float H[] = {eye_x+SkyboxXSpan/2, eye_y-SkyboxYSpan/2, eye_z+SkyboxZSpan/2};

	////////////////////// LEFT WALL ///////////////////////
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(E[0], E[1], E[2]);
		glTexCoord2f(1, 0);
		glVertex3f(F[0], F[1], F[2]);
		glTexCoord2f(1, 1);
		glVertex3f(B[0], B[1], B[2]);
		glTexCoord2f(0, 1);
		glVertex3f(A[0], A[1], A[2]);
	glEnd();

	////////////////////// FRONT WALL ///////////////////////
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(F[0], F[1], F[2]);
		glTexCoord2f(1, 0);
		glVertex3f(G[0], G[1], G[2]);
		glTexCoord2f(1, 1);
		glVertex3f(C[0], C[1], C[2]);
		glTexCoord2f(0, 1);
		glVertex3f(B[0], B[1], B[2]);
	glEnd();

	////////////////////// RIGHT WALL ///////////////////////
	glBindTexture(GL_TEXTURE_2D, txId[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(G[0], G[1], G[2]);
		glTexCoord2f(1, 0);
		glVertex3f(H[0], H[1], H[2]);
		glTexCoord2f(1, 1);
		glVertex3f(D[0], D[1], D[2]);
		glTexCoord2f(0, 1);
		glVertex3f(C[0], C[1], C[2]);
	glEnd();


	////////////////////// REAR WALL ////////////////////////
	glBindTexture(GL_TEXTURE_2D, txId[3]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(H[0], H[1], H[2]);
		glTexCoord2f(1, 0);
		glVertex3f(E[0], E[1], E[2]);
		glTexCoord2f(1, 1);
		glVertex3f(A[0], A[1], A[2]);
		glTexCoord2f(0, 1);
		glVertex3f(D[0], D[1], D[2]);
	glEnd();

	/////////////////////// TOP //////////////////////////
	glBindTexture(GL_TEXTURE_2D, txId[4]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(B[0], B[1], B[2]);
		glTexCoord2f(1, 0);
		glVertex3f(C[0], C[1], C[2]);
		glTexCoord2f(1, 1);
		glVertex3f(D[0], D[1], D[2]);
		glTexCoord2f(0, 1);
		glVertex3f(A[0], A[1], A[2]);
	glEnd();

	/////////////////////// FLOOR //////////////////////////
	glBindTexture(GL_TEXTURE_2D, txId[5]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(E[0], E[1], E[2]);
		glTexCoord2f(1, 0);
		glVertex3f(H[0], H[1], H[2]);
		glTexCoord2f(1, 1);
		glVertex3f(G[0], G[1], G[2]);
		glTexCoord2f(0, 1);
		glVertex3f(F[0], F[1], F[2]);
	glEnd();
  
	glDisable(GL_TEXTURE_2D);
}


//--- A bowl -----------------------------------------------------------
void bowl() {
    
    const int N = 51;  // Total number of vertices on the base curve
    float vx[N], vy[N];
    float x = 0;
    for (int i = 0; i < N; i++) {
		vx[i] = x;
		vy[i] = 2*x*x/17; // parabola: (2/17)x^2
        x += 0.1;
	}
    float vz[N] = {0};

	glColor4f (1.0, 0.75, 0.5, 1.0);
	float wx[N], wy[N], wz[N]; 
	float angStep = 10.0*3.1415926/180.0;  //Rotate in 10 deg steps (converted to radians)
	
    for (int j = 0; j < 36; j++) {
        glBegin(GL_TRIANGLE_STRIP);
                for (int i = 0; i < N; i++) {
                    wx[i] = vx[i]*cos(angStep) + vz[i]*sin(angStep);
                    wy[i] = vy[i];
                    wz[i] = - vx[i]*sin(angStep) + vz[i]*cos(angStep);
                    if(i > 0) normal( wx[i-1], wy[i-1], wz[i-1],
                                    vx[i-1], vy[i-1], vz[i-1],
                                    vx[i], vy[i], vz[i] );
                    glVertex3f(vx[i], vy[i], vz[i]);
                    if(i > 0) normal( wx[i-1], wy[i-1], wz[i-1],
                                    vx[i], vy[i], vz[i],
                                    wx[i], wy[i], wz[i] );
                    glVertex3f(wx[i], wy[i], wz[i]);
                }
                for (int i = 0; i < N; i++) {
                    vx[i] = wx[i];
                    vy[i] = wy[i];
                    vz[i] = wz[i];
                }
        glEnd();
    }

	glFlush();
}

//--- A scoop of lime icecream --------------------------------------------
void scoop() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[8]);
	glPushMatrix();
		glTranslatef(0, CONE_HEIGHT/2, 0);
		glScalef(CONE_WIDTH, CONE_WIDTH, CONE_WIDTH);
	    gluSphere ( q, 1, 50, 50 );
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);
		
		/*
	glPushMatrix();
		glutSolidSphere(1, 50, 50);
    glPopMatrix();*/
}

//--- An empty icecream cone --------------------------------------------
void cone() {
    glPushMatrix();
		glColor3ub(248,184,96);
        // cut off cone just below scoop intersection
        // see https://www.wolframalpha.com/input/?i=x%5E2+%2B+(y-h%2F2)%5E2+%3D+(w)%5E2+and+y%3D(h%2Fw)x+-+w%2F2
        // where w = CONE_WIDTH and h = CONE_HEIGHT
        // For this, I assume w = 4 and h = 14
        // thus https://www.wolframalpha.com/input/?i=x%5E2+%2B+(y-7)%5E2+%3D+4%5E2+and+y%3D(14%2F4)x+-+7
        float new_width = 180/53;
        float new_height = 637/53; //259/53 + (CH, which is 7) = 630/53 -> 637/53 so it looked right
		glTranslatef(0, CONE_HEIGHT/2, 0);
		glRotatef(90, 1, 0, 0);
        glTranslatef(0, 0, CONE_HEIGHT-new_height);
		glutSolidCone(new_width, new_height, 50, 50);
    glPopMatrix();
}
	
//--- A rocket --------------------------------------------
void rocket() {
	glTranslatef(0, 5, 0);
	glRotatef(90.0, 1., 0, 0.);
	glColor4f(1, 1, 1, 1.0); //white
	glPushMatrix(); //body
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, txId[7]);
		gluCylinder(q, 2, 2, 14, 50, 50);
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPushMatrix(); //base
		glColor4ub(152, 12, 19, 1.0); //dark red
		glTranslatef(0.0, 0.0, 14.0);
		gluDisk(q, 0, 2, 50, 50);
	glPopMatrix();
	glPushMatrix(); //nose
		glColor4f(.4, .4, .4, 1.0); //dark grey
		glRotatef(180.0, 0, 1, 0.);
		glutSolidCone(2, 4, 50, 50);
	glPopMatrix();
}

//--- A pyramid that fits inside a 2x2x2 block, centered on the origin. -------
void pyramid()
{
    float p1[3] = {1, -1, 1};
    float p2[3] = {-1, -1, 1};
    float p3[3] = {-1, -1, -1};
    float p4[3] = {1, -1, -1};
    float p5[3] = {0, 1, 0};
    
    glBegin(GL_TRIANGLES);
        glVertex3f(p5[0], p5[1], p5[2]);
        glVertex3f(p3[0], p3[1], p3[2]);
        glVertex3f(p4[0], p4[1], p4[2]);

        glVertex3f(p5[0], p5[1], p5[2]);
        glVertex3f(p4[0], p4[1], p4[2]);
        glVertex3f(p1[0], p1[1], p1[2]);
        
        glVertex3f(p5[0], p5[1], p5[2]);
        glVertex3f(p1[0], p1[1], p1[2]);
        glVertex3f(p2[0], p2[1], p2[2]);
        
        glVertex3f(p5[0], p5[1], p5[2]);
        glVertex3f(p2[0], p2[1], p2[2]);
        glVertex3f(p3[0], p3[1], p3[2]);
    glEnd();

    glBegin(GL_QUADS);
        glVertex3f(p1[0], p1[1], p1[2]);
        glVertex3f(p2[0], p2[1], p2[2]);
        glVertex3f(p3[0], p3[1], p3[2]);
        glVertex3f(p4[0], p4[1], p4[2]);
    glEnd();
}

//--- A fish (without tail) --------------------------------------------
void fish() {

	glPushMatrix(); //body
		glColor4f(.8, .8, .5, 1.0);
		glScalef(10, 3, 1);
		glutSolidSphere(1, 50, 50);
	glPopMatrix();
	glPushMatrix(); //eye 1
		glColor4f(1, 1, 0, 1.0);
        glTranslatef(-8.0, 1.0, 0.5);
		glutSolidSphere(.5, 50, 50);
	glPopMatrix();
	glPushMatrix(); //eye 2
		glColor4f(1, 1, 0, 1.0);
        glTranslatef(-8.0, 1.0, -0.5);
		glutSolidSphere(.5, 50, 50);
	glPopMatrix();
}



//--- A tail for a fish (translated x+=8.5) --------------------------------
void tail() {
	glPushMatrix();
		glColor4f(.8, .8, .5, 1.0);
        glTranslatef(8.5, 0, 0);
		glScalef(2, 2, .2);
        glRotatef(90.0, 0, 0, 1);
		pyramid();
	glPopMatrix();
}
    

/** 
 * Builds a cubic brick building with an entryway, of width BUILDING_SIZE
 * and height BUILDING_SIZE - FLOOR_HEIGHT.
 * It also has a red pyramid roof, and red skirting boards.
 */
void building() {
    glPushMatrix();
		// Walls
		glEnable(GL_TEXTURE_2D);
		int numOfBricks = 10; //number of bricks in one line
		glColor4f(1, 1, 1, 1.0);
		glBindTexture(GL_TEXTURE_2D, txId[6]);
		
		glBegin(GL_QUADS);
			
			// Left wall
			glTexCoord2f(numOfBricks, 0);
			glVertex3f(0, 0, -BUILDING_SIZE);
			glTexCoord2f(0, 0);
			glVertex3f(0, 0, 0);
			glTexCoord2f(0, numOfBricks);
			glVertex3f(0, BUILDING_SIZE, 0);
			glTexCoord2f(numOfBricks, numOfBricks);
			glVertex3f(0, BUILDING_SIZE, -BUILDING_SIZE);

			// Back wall
			glTexCoord2f(0, 0);
			glVertex3f(0, 0, -BUILDING_SIZE);
			glTexCoord2f(0, numOfBricks);
			glVertex3f(0, BUILDING_SIZE, -BUILDING_SIZE);
			glTexCoord2f(numOfBricks, numOfBricks);
			glVertex3f(BUILDING_SIZE, BUILDING_SIZE, -BUILDING_SIZE);
			glTexCoord2f(numOfBricks, 0);
			glVertex3f(BUILDING_SIZE, 0, -BUILDING_SIZE);

			// Right wall
			glTexCoord2f(0, 0);
			glVertex3f(BUILDING_SIZE, 0, -BUILDING_SIZE);
			glTexCoord2f(numOfBricks, 0);
			glVertex3f(BUILDING_SIZE, 0, 0);
			glTexCoord2f(numOfBricks, numOfBricks);
			glVertex3f(BUILDING_SIZE, BUILDING_SIZE, 0);
			glTexCoord2f(0, numOfBricks);
			glVertex3f(BUILDING_SIZE, BUILDING_SIZE, -BUILDING_SIZE);
			
			// Front wall (with entrance)
			/*
			// Vertices to make up concave polygon:
			glVertex3f(0, 0, 0);
			glVertex3f(2*BUILDING_SIZE/5, 0, 0);
			glVertex3f(2*BUILDING_SIZE/5, 2*BUILDING_SIZE/5, 0);
			glVertex3f(3*BUILDING_SIZE/5, 2*BUILDING_SIZE/5, 0);
			glVertex3f(3*BUILDING_SIZE/5, 0, 0);
			glVertex3f(BUILDING_SIZE, 0, 0);
			glVertex3f(BUILDING_SIZE, BUILDING_SIZE, 0);
			glVertex3f(0, BUILDING_SIZE, 0);*/		
			
			// See diagram for codes
			// A1
			glTexCoord2f(0, 0);
			glVertex3f(0, 2*BUILDING_SIZE/5, 0);
			glTexCoord2f(numOfBricks, 0);
			glVertex3f(BUILDING_SIZE, 2*BUILDING_SIZE/5, 0);
			glTexCoord2f(numOfBricks, 3*numOfBricks/5);
			glVertex3f(BUILDING_SIZE, BUILDING_SIZE, 0);
			glTexCoord2f(0, 3*numOfBricks/5);
			glVertex3f(0, BUILDING_SIZE, 0);
			
			// A2
			glTexCoord2f(0, 0);
			glVertex3f(0, 0, 0);
			glTexCoord2f(2*numOfBricks/5, 0);
			glVertex3f(2*BUILDING_SIZE/5, 0, 0);
			glTexCoord2f(2*numOfBricks/5, 2*numOfBricks/5);
			glVertex3f(2*BUILDING_SIZE/5, 2*BUILDING_SIZE/5, 0);
			glTexCoord2f(0, 2*numOfBricks/5);
			glVertex3f(0, 2*BUILDING_SIZE/5, 0);
			
			// A3
			glTexCoord2f(0, 0);
			glVertex3f(3*BUILDING_SIZE/5, 0, 0);
			glTexCoord2f(2*numOfBricks/5, 0);
			glVertex3f(BUILDING_SIZE, 0, 0);
			glTexCoord2f(2*numOfBricks/5, 2*numOfBricks/5);
			glVertex3f(BUILDING_SIZE, 2*BUILDING_SIZE/5, 0);
			glTexCoord2f(0, 2*numOfBricks/5);
			glVertex3f(3*BUILDING_SIZE/5, 2*BUILDING_SIZE/5, 0);
			
		glEnd();
		glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    
    // Pyramid roof
    glPushMatrix();
		glColor4ub(200, 0, 0, 1.0);
        glTranslatef(50, 120, -50);
		glScalef(50, 20, 50);
		pyramid();
    glPopMatrix();
    
    // Skirting boards
    
    // Left wall
    glPushMatrix();
		glColor4ub(200, 0, 0, 1.0);
        glTranslatef(0, -1.5, -BUILDING_SIZE/2);
		glScalef(1, 3, 99);
		glutSolidCube(1);
    glPopMatrix();
    
    // Back wall
    glPushMatrix();
		glColor4ub(200, 0, 0, 1.0);
        glTranslatef(BUILDING_SIZE/2, -1.5, -BUILDING_SIZE);
		glScalef(101, 3, 1);
		glutSolidCube(1);
    glPopMatrix();
    
    // Right wall
    glPushMatrix();
		glColor4ub(200, 0, 0, 1.0);
        glTranslatef(BUILDING_SIZE, -1.5, -BUILDING_SIZE/2);
		glScalef(1, 3, 99);
		glutSolidCube(1);
    glPopMatrix();
    
    // Front wall
    // Left side
    glPushMatrix();
		glColor4ub(200, 0, 0, 1.0);
        glTranslatef(20, -1.5, 0);
		glScalef(41, 3, 1);
		glutSolidCube(1);
    glPopMatrix();
    // Right side
    glPushMatrix();
		glColor4ub(200, 0, 0, 1.0);
        glTranslatef(80, -1.5, 0);
		glScalef(41, 3, 1);
		glutSolidCube(1);
    glPopMatrix();
   
}


void animateBowl() {
    glPushMatrix();
        int speed = 7;
        int amplitude = 10; // note that the max height will be double this
        float height = amplitude * (sin(theta*speed*PI/180)) + amplitude + FLOOR_HEIGHT;
        int ceiling = 16;
        if (height > ceiling) height = ceiling;
        glTranslatef(80, height, -60);
        bowl();
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void initialise()  
{
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};

	q =  gluNewQuadric ( );
	loadTexture();  	

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 	 //Background
	glClearDepth(1.0f);  	
	glEnable(GL_DEPTH_TEST);   	
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 50);

//	Define light 0's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

//	Define light 1's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
	
	gluQuadricDrawStyle (q, GLU_FILL );
	gluQuadricNormals	(q, GLU_SMOOTH );

	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture (q, GL_TRUE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60., 1.0, 10.0, 5000.0);   //Perspective projection
}

//============================================================
void display()  
{
	//light0 position (sun-like, facing the entrance)
    float lgt0_pos[] = {BUILDING_SIZE/2, BUILDING_SIZE/2, BUILDING_SIZE, 1.0f};
    // light1 position (inside)
    float lgt1_pos[] = {BUILDING_SIZE/2, BUILDING_SIZE*6/5, -BUILDING_SIZE/2, 1};
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  	
	gluLookAt(eye_x, eye_y, eye_z,  look_x, look_y, look_z,   0, 1, 0);	
	
	//light positions
    glLightfv(GL_LIGHT0, GL_POSITION, lgt0_pos);   
    glLightfv(GL_LIGHT1, GL_POSITION, lgt1_pos); 
	
	glColor4f(1.0, 1.0, 1.0, 1.0);        //Base colour
    
	// Environment
	glPushMatrix();
		ground_plane();
	glPopMatrix();
	glPushMatrix();
		building();
	glPopMatrix();
	glPushMatrix();
		skybox();
	glPopMatrix();
    
    // Rocket animation	
	glPushMatrix();
		if (theta < 60) { // stage 1: blast-off - flying up
			glTranslatef(10, 6 + (theta*7/6), -60);
		} else if (theta < 120) { // stage 2: turning back
			glTranslatef(19, 76, -60);
			glRotatef(-(theta-60)*3, 0, 0, 1);
			glTranslatef(-9, 0, 0);
		} else if (theta < 180) { // stage 3: "flying" down
			glTranslatef(28, 76 - ((theta-120)*7/6), -60);
			glRotatef(180, 0, 0, 1);
		} else if (theta < 240) { // stage 4: fall onto ground
			glTranslatef(28, -1, -60);
			glRotatef((theta-180)*9/6, 0, 0, 1);
			glTranslatef(0, 7, 0);
			glRotatef(180, 0, 0, 1);
		} else if (theta < 300) { // stage 5: right it up, back onto launchpad
			glTranslatef(12, -3, -60);
			glRotatef((theta-240)*9/6, 0, 0, 1);
			glTranslatef(9, 2, 0);
			glRotatef(-90, 0, 0, 1);
		} else { // stage 6: on launchpad
			glTranslatef(10, 6, -60);
		}
		rocket();
	glPopMatrix();
	

    // Fish animation (character model)
    glPushMatrix();
        glTranslatef(50, 5, -70);
        glRotatef(theta*2, 0, 1, 0);
        glTranslatef(0, 0, -15);
        fish();
        glPushMatrix();
            glTranslatef(9, 0, 0);
            glRotatef(30*sin(theta/6), 0, 1, 0);
            glTranslatef(-7.5, 0, 0);
            tail();
        glPopMatrix();
    glPopMatrix();
    
    

    // Marbles
    int speed = 5; //speed (rev/cycle)
    int height = 3;
    glPushMatrix();
		glColor4f(1, 0, 0, 1.0); //red
        glTranslatef(80, sin(((theta*speed)+240)*PI/180)*height + height + 1, -20);
        glRotatef(theta*speed + 240, 0, 1, 0);  
        marble();
    glPopMatrix();
    glPushMatrix();
		glColor4f(1, 1, 0, 1.0); //yellow
        glTranslatef(80, sin(((theta*speed)+120)*PI/180)*height + height + 1, -20);
        glRotatef(theta*speed + 120, 0, 1, 0);
        marble();
    glPopMatrix();
    glPushMatrix();
		glColor4f(0, 0, 1, 1.0); //blue
        glTranslatef(80, sin((theta*speed)*PI/180)*height + height + 1, -20);
        glRotatef(theta*speed, 0, 1, 0);
        marble();
    glPopMatrix();

    // Track for marbles to run on
    glPushMatrix();
		glColor4f(0, 0, 0, 1.0); //black
        glTranslatef(80, 1, -20);
        glRotatef(16.7, 1, 0, 0);
        glRotatef(90, 1, 0, 0);
        gluDisk(q, sqrt(436)/2-1, sqrt(436)/2+1, 50, 50);
    glPopMatrix();

    // Bowl
    animateBowl();
    
    
	// Icecream animation
	if (theta < 60) { // stage 1: initial rotate
		glPushMatrix();
			glTranslatef(BUILDING_SIZE/5, CONE_HEIGHT/2, -BUILDING_SIZE/5);	
			glRotatef(-theta*3/2, 0, 0, 1);
			cone();
			scoop();
		glPopMatrix();
	} else if (theta < 120) { // stage 2: scoop rotate out and drop
		glPushMatrix();
			glTranslatef(BUILDING_SIZE/5, CONE_HEIGHT/2, -BUILDING_SIZE/5);	
			glRotatef(-90, 0, 0, 1);
			cone();
		glPopMatrix();
		glPushMatrix();
			if (theta < 100) { //stage 2a: scoop rotate out
				glTranslatef(BUILDING_SIZE/5 + CONE_HEIGHT/2, CONE_HEIGHT/2 - CONE_WIDTH, -BUILDING_SIZE/5);
				glRotatef(90 - ((theta-60)*9/4), 0, 0, 1);
				glTranslatef(CONE_WIDTH, -CONE_HEIGHT/2, 0); //move it so it will rotate well
				glTranslatef(0, CONE_HEIGHT/2, 0);
				glRotatef(180, 0, 0, 1);
				glTranslatef(0, -CONE_HEIGHT/2, 0);
				scoop();
			} else { //stage 2b: drop
				glTranslatef(BUILDING_SIZE/5 + CONE_HEIGHT/2 + CONE_WIDTH, 0, 0);
				glTranslatef(0, -CONE_WIDTH - (theta-100)/10, -BUILDING_SIZE/5);
				glTranslatef(0, CONE_HEIGHT/2, 0);
				glRotatef(180, 0, 0, 1);
				glTranslatef(0, -CONE_HEIGHT/2, 0);
				scoop();
			}
		glPopMatrix();
	} else if (theta < 300) { // stages 3-5: cone rotate 270 degrees, back to normal
		glPushMatrix();
			glTranslatef(BUILDING_SIZE/5, CONE_HEIGHT/2, -BUILDING_SIZE/5);	
			glRotatef(-90 - (theta-120)*3/2, 0, 0, 1);
			cone();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(BUILDING_SIZE/5 + CONE_HEIGHT/2 + CONE_WIDTH, 0, 0);
			glTranslatef(0, -CONE_HEIGHT/2 + CONE_WIDTH + FLOOR_HEIGHT, -BUILDING_SIZE/5);
			glTranslatef(0, CONE_HEIGHT/2, 0);
			glRotatef(180, 0, 0, 1);
			glTranslatef(0, -CONE_HEIGHT/2, 0);
			scoop();
		glPopMatrix();
	} else if (theta < 310) { //stage 6a: waiting for scoop to come back
		glPushMatrix();
			glTranslatef(BUILDING_SIZE/5 + CONE_HEIGHT/2 + CONE_WIDTH, 0, 0);
			glTranslatef(0, -CONE_HEIGHT/2 + CONE_WIDTH + FLOOR_HEIGHT, -BUILDING_SIZE/5);
			glTranslatef(0, CONE_HEIGHT/2, 0);
			glRotatef(180, 0, 0, 1);
			glTranslatef(0, -CONE_HEIGHT/2, 0);
			scoop();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(BUILDING_SIZE/5, CONE_HEIGHT/2, -BUILDING_SIZE/5);	
			cone();
		glPopMatrix();
	} else { //stage 6b: scoop back on icecream :)
		glPushMatrix();
			glTranslatef(BUILDING_SIZE/5, CONE_HEIGHT/2, -BUILDING_SIZE/5);	
			cone();
			scoop();
		glPopMatrix();
	}
    

	glutSwapBuffers();
}

//--------------------------------------------------------------------------------
void special(int key, int x, int y)
{
	if (key == GLUT_KEY_F5) {
		camera_mode++;
		// Wrap around to 0
		if (camera_mode >= NUMBER_OF_CAMERA_MODES) camera_mode = 0;
	}
		
	if (camera_mode == 0) {
		// General (default) view
		
		// Change direction
		if(key == GLUT_KEY_LEFT) cam_angle -= 0.1;  
		else if(key == GLUT_KEY_RIGHT) cam_angle += 0.1;
		
		// Move camera
		else if (key == GLUT_KEY_INSERT) {
			// Move left
			eye_x -= cos(cam_angle);
			eye_z += sin(cam_angle);
		} else if (key == GLUT_KEY_HOME) {
			// Move right
			eye_x += cos(cam_angle);
			eye_z -= sin(cam_angle);
		} else if(key == GLUT_KEY_DOWN) {
			//Move backward
			eye_x -= sin(cam_angle);
			eye_z += cos(cam_angle);
		} else if(key == GLUT_KEY_UP) {
			//Move forward
			eye_x += sin(cam_angle);
			eye_z -= cos(cam_angle);
		} else if (key == GLUT_KEY_PAGE_UP) {
			// Move up
			eye_y += 1;
		} else if (key == GLUT_KEY_PAGE_DOWN) {
			// Move down
			eye_y -= 1;
		}
		
		// Turn lights on/off
		else if (key == GLUT_KEY_F1 && !glIsEnabled(GL_LIGHT0)) glEnable(GL_LIGHT0);
		else if (key == GLUT_KEY_F1 && glIsEnabled(GL_LIGHT0)) glDisable(GL_LIGHT0);
		else if (key == GLUT_KEY_F2 && !glIsEnabled(GL_LIGHT1)) glEnable(GL_LIGHT1);
		else if (key == GLUT_KEY_F2 && glIsEnabled(GL_LIGHT1)) glDisable(GL_LIGHT1);
		
		look_x = eye_x + 100*sin(cam_angle);
		look_y = eye_y;
		look_z = eye_z - 100*cos(cam_angle);
	} /*else if (camera_mode == 1) {
		// First person view - fish
		eye_x = 50;
		eye_y = 5;
		eye_z = -70;
		look_x = eye_x + 100*sin(cam_angle);
		look_y = eye_y;
		look_z = eye_z - 100*cos(cam_angle);
		
		WHAT THE FISH DOES:
		
        glTranslatef(50, 5, -70);
        glRotatef(theta*2, 0, 1, 0);
        glTranslatef(0, 0, -15);
        fish();
        glPushMatrix();
            glTranslatef(9, 0, 0);
            glRotatef(30*sin(theta/6), 0, 1, 0);
            glTranslatef(-7.5, 0, 0);
            tail();
        glPopMatrix();
		
	}*/
		
	
	glutPostRedisplay();
}

//----------------------------------------------------------------
void timer(int value)
{
	theta++;
	if(theta > 360) theta = 0;
	glutTimerFunc(TICK, timer, value);
	glutPostRedisplay();
}

//----------------------------------------------------------------
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("och26 Assignment 1");
	initialise ();
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutTimerFunc(TICK, timer, 0);
	glutMainLoop();
	return 0;
}
