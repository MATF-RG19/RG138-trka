#include<GL/glut.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define NUMBER_OF_DOTS 10
#define PI 3.1415926535

static int window_width, window_height;
static int animation_ongoing;
static int kretanje[]={0, 0};

double x_coveka=2;
double y_coveka=0.7;
double z_coveka=0;

double x_puta=2;
double y_puta=-0.5;
double z_puta=3;

static void draw_road(void);
static void draw_human(void);
static void kretanje_coveka(int value);

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_release(unsigned char key, int x, int y);

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(700, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Trka");

	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutKeyboardUpFunc(on_release);
	glutDisplayFunc(on_display);

	glClearColor(0, 0.5, 1, 0);
	glEnable(GL_DEPTH_TEST);

	animation_ongoing=0;

	glutMainLoop();

	return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:
			exit(0);
			break;
		case 'S':
		case 's'://pokrece se
			if(!animation_ongoing){
				glutTimerFunc(40, kretanje_coveka, 0);
				animation_ongoing=1;
			}
			break;
		case 'R':
		case 'r'://pauzira se
			animation_ongoing=0;
			break;
		case 'D':
		case 'd'://skrece desno
			kretanje[1]=1;
			glutPostRedisplay();
			break;
		case 'A':
		case 'a'://skrece levo
			kretanje[0]=1;
			glutPostRedisplay();
			break;
	}
}

static void on_reshape(int width, int height){
	window_width=width;
	window_height=height;

	glViewport(0, 0, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)window_width/window_height, 1, 1500);
}

static void on_release(unsigned char key, int x, int y){
	switch(key){
		case 'A':
		case 'a':
			kretanje[0]-=1;
			break;
		case 'D':
		case 'd':
			kretanje[1]-=1;
			break;
	}
}

static void on_display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(2, 2, -4, 2, 0, 2, 0, 1, 0);

	/*iscrtavamo ose
	glBegin(GL_LINES);
		//postavlja se x-osa
		glColor3f(1, 1, 1);
		glVertex3f(100, 0, 0);
		glVertex3f(-100, 0, 0);
		
		//postavlja se y-osa
		glColor3f(0, 1, 0);
		glVertex3f(0, 100, 0);
		glVertex3f(0, -100, 0);

		//postavlja se z-osa
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 100);
		glVertex3f(0, 0, -100);
    	glEnd();*/

	draw_road();
	draw_human();

	glutSwapBuffers();
}

static void draw_road(){
	glPushMatrix();
        	glColor3f(1, 0, 0);
        	glTranslatef(x_puta, y_puta, z_puta);
        	glScalef(4.5, 0.5, 5);
        	glutSolidCube(1);
	glPopMatrix();
}

static void draw_human(){
	glColor3f(0, 0, 0);
	glLineWidth(5);

	glPushMatrix();
		glTranslatef(x_coveka, y_coveka, z_coveka);

		//iscrtava glavu
		glBegin(GL_POLYGON);
	       		glVertex2f(0, 0);
			for(int i=0;i<360;i++){
				glVertex2f(cos(2*i*PI/NUMBER_OF_DOTS)*0.1,
						sin(2*i*PI/NUMBER_OF_DOTS)*0.1);
			}
	    	glEnd();

		//iscrtava telo 
		glBegin(GL_LINES);
		       	glVertex2f(0, 0);
			glVertex2f(0, -0.5);
		glEnd();

		//iscrtava ruke
		glBegin(GL_LINES);
			glVertex2f(0, -0.2);
			glVertex2f(-0.2, -0.28);
		glEnd();

		glBegin(GL_LINES);
		    	glVertex2f(0, -0.2);
			glVertex2f(0.2, -0.28);
		glEnd();

		//iscrtava noge
		glBegin(GL_LINES);
		       	glVertex2f(0, -0.5);
			glVertex2f(-0.15, -0.62);
		glEnd();
			
		glBegin(GL_LINES);
		       	glVertex2f(0, -0.5);
			glVertex2f(0.15, -0.62);
		glEnd();

	glPopMatrix();
}

static void kretanje_coveka(int value){
	if(value){
		return;	
	}

	if(kretanje[0] && x_coveka<3.5){
		x_coveka+=0.1;
	}
	if(kretanje[1] && x_coveka>0.5){
		x_coveka-=0.1;
	}

	z_puta-=0.2;

	if(z_puta<-4){
		z_puta=14;
	}

	glutPostRedisplay();

	if(animation_ongoing){
		glutTimerFunc(40, kretanje_coveka, 0);
	}
}
