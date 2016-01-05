//
//  main.cpp
//  OpenGLHomeWork02
//
//  Created by 张焕 on 16/1/5.
//  Copyright © 2016年 Huan. All rights reserved.
//

#if defined(_WIN32)
#include "glut.h"
#include <windows.h>
#else
#include <GLUT/glut.h>
#endif

#include <math.h>
#include "RGBpixmap.h"
#include "SolarSystemGlobals.h"


GLfloat viewerAzimuth = INITIAL_VIEWER_AZIMUTH;
GLfloat viewerZenith  = INITIAL_VIEWER_ZENITH;


GLuint EarthTextureName = 3;
GLuint MoonTextureName  = 4;
GLuint SunTextureName   = 0;


GLfloat CurrentEarthRotation = 0.00;
GLfloat EarthDaysTranspired  = 0.00;
GLfloat EarthDayIncrement    = 0.04;


GLint currWindowSize[2]   = { 800, 600 };
GLint currViewportSize[2] = { 800, 600 };

GLfloat ViewerDistance = INITIAL_VIEWER_DISTANCE;


bool blendFlag = true;


void TimerFunction(int value);
void Display();

void MakeAllImages();
void MakeImage(const char bitmapFilename[], GLuint &textureName, bool hasAlpha);

void SetLights();
void UpdateLight();

void ResizeWindow(GLsizei w, GLsizei h);

void drawEarthAndMoon();
void drawSun();

void drawAllPlanets();
void drawGenericPlanet(GLfloat inclination, GLfloat orbitDuration,
                       GLfloat orbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius);

int main(int argc, char** argv)
{
    cout << argv[0] <<endl;
    glutInit (&argc, argv);//初始化
    

    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH );//设定显示模式
    glutInitWindowPosition( INIT_WINDOW_POSITION[0], INIT_WINDOW_POSITION[1] );//位置
    glutInitWindowSize( currWindowSize[0], currWindowSize[1] );//大小
    glutCreateWindow( "21551051张焕");//窗口名称
    

    glutReshapeFunc( ResizeWindow );
    glutDisplayFunc( Display );
    glutTimerFunc( 20, TimerFunction, 1 );
    glViewport(0, 0, currWindowSize[0], currWindowSize[1]);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_NORMALIZE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    SetLights();
    

    MakeAllImages();
    glutMainLoop();
    return 0;
}
	
void TimerFunction(int value)
{
    CurrentEarthRotation += 0.018;
    EarthDaysTranspired += EarthDayIncrement;
    if (EarthDaysTranspired == EARTH_ORBIT_DUR)
        EarthDaysTranspired = 0;
    glutPostRedisplay();
    glutTimerFunc(20, TimerFunction, 1);
}


void Display()
{
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LIGHT_MODEL_AMBIENT);
    glEnable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
   
    gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
              LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
              LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
              LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
              0.0, 1.0, 0.020);
    
    
    UpdateLight();
    drawEarthAndMoon();
    drawSun();
    
    if(blendFlag){
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_COLOR,GL_ONE);
    }
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glutSwapBuffers();
    glFlush();
}


void MakeAllImages()
{
    MakeImage(EARTH_BMP_FILENAME, EarthTextureName, false);
    MakeImage(MOON_BMP_FILENAME,  MoonTextureName,  false);
    MakeImage(SUN_BMP_FILENAME,		SunTextureName,		false);
    return;
}



void MakeImage(const char bitmapFilename[], GLuint &textureName, bool hasAlpha)
{
    RGBpixmap pix;
    pix.readBMPFile(bitmapFilename, hasAlpha);
    pix.setTexture(textureName);
    return;
}



void SetLights()
{
    glLightfv(GL_LIGHT0, GL_AMBIENT,  LIGHT_AMBIENT);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  LIGHT_DIFFUSE);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT_SPECULAR);
    glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_0_POSITION);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT,  LIGHT_AMBIENT);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  LIGHT_DIFFUSE);
    glLightfv(GL_LIGHT1, GL_SPECULAR, LIGHT_SPECULAR);
    glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_1_POSITION);
}



void UpdateLight()
{
    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_0_POSITION);
    glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_1_POSITION);

    glPopMatrix();
    
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}


void ResizeWindow(GLsizei w, GLsizei h)
{
    currWindowSize[0] = w;
    currWindowSize[1] = h;
    if (ASPECT_RATIO > w/h)
    {
        currViewportSize[0] = w;
        currViewportSize[1] = w / ASPECT_RATIO;
    }
    else
    {
        currViewportSize[1] = h;
        currViewportSize[0] = h * ASPECT_RATIO;
    }
    
    glViewport(0.5*(w-currViewportSize[0]), 0.5*(h-currViewportSize[1]), currViewportSize[0], currViewportSize[1]);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



void drawEarthAndMoon()
{
    glDisable(GL_LIGHT1);
    
    GLfloat MoonRevolution = EarthDaysTranspired / LUNAR_CYCLE;
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPushMatrix();
				glRotatef(EARTH_INCLINATION, 0.0, 0.0, 0.0);
				glRotatef( 360.0 * (EarthDaysTranspired/EARTH_ORBIT_DUR), 0.0, 1.0, 0.0);
				glTranslatef(EARTH_ORBIT_RADIUS, 0.0, 0.0 );
				glRotatef( 360.0 * CurrentEarthRotation, 0.0, 1.0, 0.0 );
				glRotatef( -90.0, 1.0, 0.0, 0.0 );
				glBindTexture(GL_TEXTURE_2D, EarthTextureName);
				gluSphere(quadro, EARTH_RADIUS, 48, 48);
    glPopMatrix();
    glRotatef(EARTH_INCLINATION, 0.0, 0.0, 0.0);
    glRotatef( 360.0 * (EarthDaysTranspired/EARTH_ORBIT_DUR), 0.0, 1.0, 0.0);
    glTranslatef(EARTH_ORBIT_RADIUS, 0.0, 0.0 );
    glRotatef( 360.0 * MoonRevolution, 0.0, 1.0, 0.0 );
    glTranslatef( MOON_ORBIT_RADIUS  , 0.0, 0.0 );
    glBindTexture(GL_TEXTURE_2D, MoonTextureName);
    gluSphere(quadro, MOON_RADIUS, 48, 48);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);
    
    glEnable(GL_LIGHT1);
}


void drawSun()
{
    glDisable(GL_LIGHT0);
    
    GLUquadricObj* quadro = gluNewQuadric();
    gluQuadricNormals(quadro, GLU_SMOOTH);
    gluQuadricTexture(quadro, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPushMatrix();
				//glRotatef( -90.0, 1.0, 0.0, 0.0 );
				glBindTexture(GL_TEXTURE_2D, SunTextureName);
				gluSphere(quadro, SUN_RADIUS, 48, 48);
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quadro);
    
    glEnable(GL_LIGHT0);
}
