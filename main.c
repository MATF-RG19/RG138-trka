#include<GL/glut.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

static int window_width, window_height;

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Trka");

	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);

	glClearColor(0, 0.5, 1, 0);
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
	switch(key){
		case 'q':
			exit(0);
			break;
		case 'r'://skrece desno
			
			break;
		case 'l'://skrece levo
			
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

static void on_display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(2, 2, -4, 2, 0, 2, 0, 1, 0);

	glBegin(GL_LINES);
		//postavlja se x-osa
		glColor3f(1,0,0);
		glVertex3f(100,0,0);
		glVertex3f(-100,0,0);
		
		//postavlja se y-osa
		glColor3f(0,1,0);
		glVertex3f(0,100,0);
		glVertex3f(0,-100,0);

		//postavlja se z-osa
		glColor3f(0, 0, 1);
		glVertex3f(0,0,100);
		glVertex3f(0,0,-100);
    	glEnd();

	glutSwapBuffers();
}

