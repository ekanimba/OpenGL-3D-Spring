#include <iostream>
#include <string>
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <ctime>
#include "BitmapReader.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926535897932384626434
#endif

#define CAM_DIST 4.0f 
#define FOVY_ANGLE 45.0f // fovy, perspective projection

#define topspring 0.95f
#define springradius 0.04f
#define MAX_X 1.32f
#define MAX_Y (topspring + 0.2f)
#define MAX_Z 1.3f

// Draw a cylinder between two points
void solidcylinder(float *p1, float *p2, float radius)
{
	static GLUquadricObj *obj = gluNewQuadric();
	float px = p2[0] - p1[0];
	float py = p2[1] - p1[1];
	float pz = p2[2] - p1[2];
//	float axex = -py;
//	float axey = px; // the rotation axe to put p2 in z direction (-py, px, 0)
	float n = px*px + py*py;
	float m = sqrt(n + pz*pz);
//	angle = asin(sqrt(n)/m)*180/M_PI; // from vectorial product
	glPushMatrix();
		glTranslatef(p1[0], p1[1], p1[2]);
		glRotatef(asin(sqrt(n)/m)*180/M_PI, -py, px, 0);
		gluCylinder(obj, radius, radius, m, 20, 1);
	glPopMatrix();
}

GLuint Texture, Texture2, Texture3; // wall, ceiling and floor texture identifier
vector<float> springpoint;
int numSpringPoints = 400;
float equilibriumlenght, amplitude;


GLfloat light0_position[4] = {-0.5f, 0.1f, 1.0f, 0.0f};
GLfloat light1_position[4] = {0.8f, -1.0f, 1.0f, 0.0f};
int	SCREEN_WIDTH = 580, SCREEN_HEIGHT = 500;


int framespersecond = 25;

void idlefunc()
{
	int end = clock() + CLOCKS_PER_SEC / framespersecond;
	static double delta_t = 0.3f;
	static double t = M_PI/2;
	static double l = equilibriumlenght + amplitude;
	double l2 = equilibriumlenght + amplitude*sin(t);
	double fy = l2/l;
	double fxz = sqrt(l/l2);
	l = l2;

	for (int i = 0; i < numSpringPoints; ++i)
	{
		springpoint[3*i + 0] *= fxz;
		springpoint[3*i + 1] *= fy;
		springpoint[3*i + 2] *= fxz;
	}

	t += delta_t;
	if (t > M_PI) t -= 2*M_PI;
	glutPostRedisplay();
	while(clock() < end); //wait 1/framespersecond seconds
}

void drawroom() {
	glColor3f(0.8f, 0.8f, 0.8f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture2);
	glBegin(GL_QUADS);
		glNormal3f( 0.0f,-1.0f, 0.0f ); // ceiling
		glTexCoord2f(0, 1);
		glVertex3f(-MAX_X,  MAX_Y, -MAX_Z );
		glTexCoord2f(1, 1);
		glVertex3f( MAX_X,  MAX_Y, -MAX_Z );
		glTexCoord2f(1, 0);
		glVertex3f( MAX_X,  MAX_Y,  MAX_Z );
		glTexCoord2f(0, 0);
		glVertex3f(-MAX_X,  MAX_Y,  MAX_Z );
	glEnd();
	glBindTexture(GL_TEXTURE_2D, Texture3);
	glBegin(GL_QUADS);
		glNormal3f( 0.0f, 1.0f, 0.0f ); // floor
		glTexCoord2f(0, 0);
		glVertex3f(-MAX_X, -MAX_Y,  MAX_Z );
		glTexCoord2f(1, 0);
		glVertex3f( MAX_X, -MAX_Y,  MAX_Z );
		glTexCoord2f(1, 1);
		glVertex3f( MAX_X, -MAX_Y, -MAX_Z );
		glTexCoord2f(0, 1);
		glVertex3f(-MAX_X, -MAX_Y, -MAX_Z );
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, Texture);
	glBegin(GL_QUADS);
		glNormal3f( 1.0f, 0.0f, 0.0f ); // left
		glTexCoord2f(1, 1);
		glVertex3f(-MAX_X, -MAX_Y,  MAX_Z );
		glTexCoord2f(0, 1);
		glVertex3f(-MAX_X, -MAX_Y, -MAX_Z );
		glTexCoord2f(0, 0);
		glVertex3f(-MAX_X,  MAX_Y, -MAX_Z );
		glTexCoord2f(1, 0);
		glVertex3f(-MAX_X,  MAX_Y,  MAX_Z );
		glNormal3f(-1.0f, 0.0f, 0.0f ); // right
		glTexCoord2f(0, 1);
		glVertex3f( MAX_X, -MAX_Y, -MAX_Z );
		glTexCoord2f(1, 1);
		glVertex3f( MAX_X, -MAX_Y,  MAX_Z );
		glTexCoord2f(1, 0);
		glVertex3f( MAX_X,  MAX_Y,  MAX_Z );
		glTexCoord2f(0, 0);
		glVertex3f( MAX_X,  MAX_Y, -MAX_Z );
		glNormal3f( 0.0f, 0.0f, 1.0f ); // back
		glTexCoord2f(0, 1);
		glVertex3f(-MAX_X, -MAX_Y, -MAX_Z );
		glTexCoord2f(1, 1);
		glVertex3f( MAX_X, -MAX_Y, -MAX_Z );
		glTexCoord2f(1, 0);
		glVertex3f( MAX_X,  MAX_Y, -MAX_Z );
		glTexCoord2f(0, 0);
		glVertex3f(-MAX_X,  MAX_Y, -MAX_Z );
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void MyDisplay()
{
	float v0[3] = {0.0f, MAX_Y, 0.0f}, v1[3] = {0.0f, MAX_Y + 0.1f, 0.0f};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -CAM_DIST); // the camera is moved away from center

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	drawroom();

	glColor3ub(180, 180, 120);
	glPushMatrix();
		glTranslatef(v1[0], v1[1], v1[2]);
		glRotatef(45, 0.0f, 1.0f, 0.0f);
		glutSolidCube(0.4f); // decorative
	glPopMatrix();

	v1[1] = topspring;
	glColor3ub(85, 85, 85); // spring color
	solidcylinder(v0, v1, springradius); // spring upper end
	glPushMatrix();
		glTranslatef(v1[0], v1[1], v1[2]);
		glutSolidSphere(springradius, 20, 20);
	glPopMatrix();
	v0[0] = springpoint[0];
	v0[1] = topspring - springpoint[1];
	v0[2] = springpoint[2];
	solidcylinder(v1, v0, springradius);
	for (int i = 0; i < numSpringPoints; ++i)
	{
		glPushMatrix();
		glTranslatef(springpoint[3*i], topspring - springpoint[3*i + 1], springpoint[3*i + 2]);
		glutSolidSphere(springradius, 20, 20);
		glPopMatrix();
	}
	v0[0] = springpoint[3*numSpringPoints-6];
	v0[1] = topspring - springpoint[3*numSpringPoints-5];
	v0[2] = springpoint[3*numSpringPoints-4];
	v1[1] = topspring - springpoint[3*numSpringPoints-2];
	solidcylinder(v0, v1, springradius);
	v0[0] = v0[2] = 0.0f;
	v0[1] = v1[1] - 0.15f;
	solidcylinder(v1, v0, springradius); // spring lower end
	glTranslatef(0.0f, v0[1] - 0.20f, 0.0f);
	glColor3ub(10, 10, 10); // ball color
	glutSolidSphere(0.25f, 30, 30);
	glutSwapBuffers();
}


GLuint LoadTexture(const char *filename)
{
	GLuint ID;
	BitmapReader textureData(filename);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureData.width, textureData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) textureData.img);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return ID;
}

void MyInit(void) {
	GLfloat light0_ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat light0_diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};

	GLfloat light1_ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
	GLfloat light1_diffuse[4] = {0.5f, 0.5f, 0.5f, 1.0f};

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_diffuse);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_diffuse);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT, GL_FILL);
	glClearColor(1.0, 1.0, 1.0, 0.0);//select clearing (background) color
}

void initspring()
{
	float t;
	int n = numSpringPoints - 2;
	float dt = 8.0f*M_PI/n;
	t = 3.0f + dt;
	for (int i = 0; i <= n; ++i)
	{
//		springpoint.push_back(cos(t)*(3 + cos(u)));
//		springpoint.push_back(sin(t)*(3 + cos(u)));
//		springpoint.push_back(0.6f*t + sin(u));
		springpoint.push_back(0.2f*cos(t));
		springpoint.push_back(0.045f*t);
		springpoint.push_back(0.2f*sin(t));
		t += dt;
	}
	t += 3.0f;
	springpoint.push_back(0.0f);
	springpoint.push_back(0.045f*t);
	springpoint.push_back(0.0f);
	equilibriumlenght = 0.035f*t;
	amplitude = 0.01f*t;
}

void Resize(int w, int h)
{
	SCREEN_WIDTH = w; SCREEN_HEIGHT = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY_ANGLE, (GLfloat)w/(GLfloat)h, 0.1f, 10.0f);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	initspring();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	glutCreateWindow("Spring and ball");

	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(Resize);
	glutIdleFunc(idlefunc);
	MyInit();

	Texture = LoadTexture("wall.bmp");
	Texture2 = LoadTexture("ceiling.bmp");
	Texture3 = LoadTexture("floor.bmp");

	glutMainLoop();
	return 0;
}