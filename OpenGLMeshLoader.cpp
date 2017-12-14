#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <cmath>
#include <glut.h>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

int level = 1;
float levelOneAnim= 0.0;
float angle = 0;
int WIDTH = 1280;
int HEIGHT = 720;
float backgroundAnim = 0.0;
float spaceShipLocationX = 0.0;
float spaceShipLocationY = 0.0;
char title[] = "3D Model Loader Sample";
float chickenForward = 0.0;
int score = 0;
bool first_person = false;
bool change = false;
bool win = false;
bool lose =  false;
// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio =  (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(15, 10, 45);
Vector At(15, 10, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

//bullet obj and array
int cur_bullet = 0;

struct Bullet{
	float x, y, z;
	bool visible;
};

struct Chicken{
	float x, y, z;
	bool visible;
	Model_3DS model_chicken;
};

Bullet bullet[200];
Chicken chicken[12];

Bullet create_bullet(float x, float y, float z){
	Bullet b;
	b.x = x; b.y = y; b.z = z;
	b.visible = true;
	return b;
}

void hide_bullet(Bullet* b){
	(*b).visible = false;
}

void create_chicken(){
	int xTra = 12;
	int yTra = 6;
	for(int i = 0; i<12; i++){
		chicken[i].x= xTra;
		chicken[i].y= yTra;
		chicken[i].z= 3;
		chicken[i].visible = true;
		if(level ==1){
			chicken[i].model_chicken.Load("Models/chicken/Chicken N161109.3DS"); 
		} else{ chicken[i].model_chicken.visible=true;}
		if((i+1)%4 != 0){
			xTra=xTra+3;
		} else{
			xTra=12;
			yTra=yTra+3;
		}
	}
}


void draw_chicken(){
	for(int i=0; i<12; i++){
		if(chicken[i].visible == true){
			glPushMatrix();
			if(level == 1){
				if((i+1)%4==0){
					if(chicken[i].x > 6){
						chicken[i].x = chicken[i].x - 0.0055;
					}
				} else if((i+1)%4==1){
					if(chicken[i].x < 23){
						chicken[i].x = chicken[i].x + 0.0055;
					}
				}
			} else if(level == 2){
				if((i+1)%4==0 || (i+1)%4==3){ 
						chicken[i].y = 10 + (float)(sin(angle)*5);
  				} else {
						chicken[i].y = 10 - (float)(sin(angle)*5);
					}
			} else if(level == 3){
				if((i+1)%4==0||(i+1)%4==1){
						chicken[i].x = 10 + (sinf(angle)*5);
				} else {
						chicken[i].x = 10 - (sinf(angle)*5);
					}
			}
			glTranslated(chicken[i].x, chicken[i].y, chicken[i].z);
			glRotated(90,1,0,0);
			glScaled(0.02,0.02,0.02);
			chicken[i].model_chicken.Draw();
			glPopMatrix();
		} else {
			chicken[i].model_chicken.visible =false;
		}
	}
}

void check_collision(){
	if(chickenForward >=20){
		chickenForward = chickenForward +10;
		lose = true;
		return;
	}
	for(int i=0; i<12; i++){
		for(int j=0; j<200; j++){
			if(bullet[j].visible == false || chicken[i].visible == false)
				continue;
			if(chicken[i].z - 0.2 <= bullet[j].z && chicken[i].z + 0.2 >= bullet[j].z){
				bool condition = chicken[i].x+0.8>=bullet[j].x && chicken[i].x-0.8<=bullet[j].x 
					&& chicken[i].y+1.2>=bullet[j].y && chicken[i].y-0.8<=bullet[j].y;
				if(condition){
					chicken[i].visible=false;
					bullet[j].visible=false;
					score = score + (level * 2);
				}
			}
		}
	}
}
// Textures
GLuint tex_space;
GLuint tex_space_left;
GLuint tex_space_right;
GLuint tex_space_top;
GLuint tex_space_bottom;
GLuint tex_win;
GLuint tex_lose;



void levelUpdate(){
	for(int i = 0; i<12; i++){
		if(chicken[i].visible == true){
			return;
		}
	}
	level=level +1;
	if(level == 2){
		loadBMP(&tex_space, "textures/level2.bmp", true);
		loadBMP(&tex_space_left, "textures/level2.bmp", true);
		loadBMP(&tex_space_right, "textures/level2.bmp", true);
		loadBMP(&tex_space_bottom, "textures/level2.bmp", true);
		loadBMP(&tex_space_top, "textures/level2.bmp", true);
	}
	if(level ==3){
		loadBMP(&tex_space, "textures/level3.bmp", true);
		loadBMP(&tex_space_left, "textures/level3.bmp", true);
		loadBMP(&tex_space_right, "textures/level3.bmp", true);
		loadBMP(&tex_space_bottom, "textures/level3.bmp", true);
		loadBMP(&tex_space_top, "textures/level3.bmp", true);
	}
	if(level == 4){
	win = true;
	return;
	}
	chickenForward = 0;
	create_chicken();
}
// Model Variables
Model_3DS model_ship;


void print(int x, int y,int z, char *string)
{
//set the position of the text in the window using the x and y coordinates
glRasterPos2f(x,y);
//get the length of the string to display
int len = (int) strlen(string);

//loop to display character by character
for (int i = 0; i < len; i++) 
{
glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,string[i]);
}
};
//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	//glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);


	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat l0Diffuse[] = { 0.5f, 0.0f, 0.0f, 1.0f };
	GLfloat l0Ambient[] = { 0.1f, 0.0f, 0.0f, 1.0f };
	GLfloat l0Position[] = { -10.0f, 10.0f, 30.0f, true };
	GLfloat l0Direction[] = { 1.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT1,GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, l0Position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 90.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l0Direction);

	GLfloat l2Diffuse[] = { 0.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l2Ambient[] = { 0.0f, 0.1f, 0.0f, 1.0f };
	GLfloat l2Position[] = { 30.0f, 10.0f, 30.0f, true };
	//GLfloat l2Specular[]={0.0,1.0,0.0,1.0};
	GLfloat l2Direction[] = { -1.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, l2Diffuse);
	glLightfv(GL_LIGHT2,GL_AMBIENT, l2Ambient);
	glLightfv(GL_LIGHT2, GL_POSITION, l2Position);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l2Direction);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 90.0);

	GLfloat l3Diffuse[] = { 0.5f, 0.5f, 0.0f, 1.0f };
	GLfloat l3Ambient[] = { 0.1f, 0.1f, 0.0f, 1.0f };
	GLfloat l3Position[] = { 15.0f, 30.0f, 30.0f, true };
	GLfloat l3Specular[]={1.0,1.0,0.0,1.0};
	GLfloat l3Direction[] = { 0.0, -1.0, 0.0 };

	GLfloat m3Diffuse[] = { 0.99, 1.00, 0.54, 1.0 };
	GLfloat m3Ambient[] = { 0.11, 0.06, 0.11, 1.0 };
	//GLfloat m3Specular[]={0.33,0.33,0.52,1.0};

	glLightfv(GL_LIGHT3, GL_DIFFUSE, l3Diffuse);
	glLightfv(GL_LIGHT3,GL_AMBIENT, l3Ambient);
	glLightfv(GL_LIGHT3, GL_POSITION, l3Position);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l3Direction);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 30.0);

	//glMaterialfv(GL_FRONT,GL_AMBIENT,m3Ambient);
	//glMaterialfv(GL_FRONT,GL_DIFFUSE,m3Diffuse);
	//glMaterialfv(GL_FRONT,GL_SPECULAR,m3Specular);
	
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //		
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

void drawWall(GLuint tex) {
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glScaled(20.0, 1, 20.0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0-backgroundAnim); glVertex3f(-0.5, 0, -0.5);
	glTexCoord2f(0, 1-backgroundAnim); glVertex3f(-0.5, 0, 0.5); 
	glTexCoord2f(1, 1-backgroundAnim); glVertex3f(0.5, 0, 0.5); 
	glTexCoord2f(1, 0-backgroundAnim); glVertex3f(0.5, 0, -0.5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void drawWallFace(GLuint tex) {
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glScaled(20.0, 1, 20.0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-0.5, 0, -0.5);
	glTexCoord2f(0, 1); glVertex3f(-0.5, 0, 0.5); 
	glTexCoord2f(1, 1); glVertex3f(0.5, 0, 0.5); 
	glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void drawSpace(){
	//draw wall
	//face blue
	glPushMatrix();
	glTranslated(15.0, 10.0, 0.0);
	glScaled(1.5,1,1);
	glRotated(-90, 1.0, 0.0, 0.0);
	drawWallFace(tex_space);
	glPopMatrix();

	
	//left red
	glPushMatrix();
	glTranslated(0.0, 10.0, 10.0);
	glRotated(90, 0, 0, 1);
	glScaled(1, 1, 1.5);
	drawWall(tex_space_left);
	glPopMatrix();

	//right green
	glPushMatrix();
	glTranslated(30.0, 10.0, 10.0);
	glScaled(1, 1, 1.5);
	glRotated(90, 0, 0, 1);
	drawWall(tex_space_right);
	glPopMatrix();
	
	//bottom yellow
	glPushMatrix();
	glTranslated(15.0, 0, 10.0);
	glScaled(1.5, 1, 1.5);
	drawWall(tex_space_bottom);
	glPopMatrix();

	//top black
	glPushMatrix();
	glTranslated(15.0, 20.0, 10.0);
	glScaled(1.5, 1, 1.5);
	drawWall(tex_space_top);
	glPopMatrix();

}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat lmodel_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = {0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	string str = to_string(static_cast<long long>(score));
	const char* a = str.c_str();
	char* final_string = new char[str.length()+1];
	strcpy(final_string, a);
	char res[100];
	strcpy(res, "score: ");
	strcat(res, final_string);
	glPushMatrix();
	print(1,18,1, res);
	drawSpace();
    glPopMatrix();

    //win lose
	if(win || lose){
	glPushMatrix();
	glTranslated(15,10,0.5);
	glScaled(30,20,30);
	glRotated(-90,1,0,0);
	if(win){
		drawWallFace(tex_win);}
	else{ drawWallFace(tex_lose);}
	glPopMatrix();
	}
	glPushMatrix();
	glTranslated(17.45+spaceShipLocationX,5+spaceShipLocationY,31);
	glScaled(0.3,0.3,0.3);
	model_ship.Draw();
	glPopMatrix();

	//handle  bullets
	for(int i =0; i < 200; i++){
		if(bullet[i].visible == true){
			bullet[i].z -=  0.5;
		glPushMatrix();
		glTranslated(bullet[i].x, bullet[i].y, bullet[i].z-0.7);
		glutSolidSphere(0.15, 20, 20);
		glPopMatrix();
			if(bullet[i].z < -1)
				bullet[i].visible = false;
		}
	}

	//handle chicken
	glPushMatrix();
	glTranslated(0.0,0.0,chickenForward);
	draw_chicken();
	glPopMatrix();

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void spaceShipNavigation(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		if(spaceShipLocationY < 10){
			spaceShipLocationY = spaceShipLocationY + 0.2;
		}
		break;
	case GLUT_KEY_DOWN:
		if(spaceShipLocationY > 0){
			spaceShipLocationY = spaceShipLocationY - 0.2;
		}
		break;
	case GLUT_KEY_LEFT:
		if(spaceShipLocationX > -10){
			spaceShipLocationX = spaceShipLocationX - 0.2;
		}
		break;
	case GLUT_KEY_RIGHT:
		if(spaceShipLocationX < 10){
			spaceShipLocationX = spaceShipLocationX + 0.2;
		}
		break;
	}

	glutPostRedisplay();
}

//=================================
// Keyboard function.
//=================================
int delay_shoot = 0;
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 's':
		first_person = !first_person;
		break;
	case 'k':
		cur_bullet = cur_bullet % 200;
		bullet[cur_bullet] = create_bullet(15+spaceShipLocationX, 5+spaceShipLocationY, 30);
		cur_bullet++;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_ship.Load("Models/ship/models/Tree1.3DS");
	create_chicken();
	loadBMP(&tex_lose, "textures/lose.bmp",true);
	loadBMP(&tex_win, "textures/win.bmp",true);
	loadBMP(&tex_space, "textures/level1.bmp", true);
	loadBMP(&tex_space_left, "textures/level1.bmp", true);
	loadBMP(&tex_space_right, "textures/level1.bmp", true);
	loadBMP(&tex_space_bottom, "textures/level1.bmp", true);
	loadBMP(&tex_space_top, "textures/level1.bmp", true);
}

void Timer(int value){
	levelUpdate();
	backgroundAnim = backgroundAnim + 0.0020;
	chickenForward = chickenForward + 0.02;
	angle = angle + 0.01;
	check_collision();
	if(first_person){
		Eye.x=15+spaceShipLocationX; 
		Eye.y=5+spaceShipLocationY;
		Eye.z=31;
		At.x=15+spaceShipLocationX;
		At.y=5+spaceShipLocationY;
		At.z=0;
	} else {
		Eye.x=15; 
		Eye.y=10;
		Eye.z=45;	
		At.x=15;
		At.y=10;
		At.z=0;
	}

	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	glutPostRedisplay();
	glutTimerFunc(50, Timer, 0);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);
	glutTimerFunc(0, Timer, 0);
	
	glutSpecialFunc(spaceShipNavigation);
	glutKeyboardFunc(myKeyboard);
	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
		glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);
	//glEnable(GL_LIGHT3);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}