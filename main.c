#include<GL/glut.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define NUMBER_OF_DOTS 10
#define PI 3.1415926535

static int window_width, window_height;
static int animation_ongoing;
static int kretanje[]={0, 0};

static double x_coveka=2;
static double y_coveka=0.7;
static double z_coveka=0;

static double x_puta_prvog=2;
static double y_puta_prvog=-0.5;
static double z_puta_prvog=3;

static double x_puta_drugog=2;
static double y_puta_drugog=-0.5;
static double z_puta_drugog=10;

static int loptica=0;

typedef struct{
	double x;
	double y;
	double z;
} Prepreka;

Prepreka prepreke1[80];
Prepreka prepreke2[80];

static int prva_poz;
static int druga_poz;

static void draw_human(void);
static void kretanje_coveka(int value);
static void postavi_prepreke();
static void iscrtaj_loptice(int x, int y, int z);
static void iscrtaj_put_prvi();
static void iscrtaj_put_drugi();
static void kolizija();
static float udaljenost(Prepreka pr);
static void postavi_prepreke();

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

	iscrtaj_put_prvi();
	iscrtaj_put_drugi();
	draw_human();

	for(int i=0;i<prva_poz;i++){
		Prepreka pr=prepreke1[i];
		iscrtaj_loptice(pr.x, pr.y, pr.z);
	}

	for(int i=0;i<druga_poz;i++){
		Prepreka pr=prepreke2[i];
		iscrtaj_loptice(pr.x, pr.y, pr.z);
	}

	glutSwapBuffers();
}

void iscrtaj_put_prvi(){
	glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(x_puta_prvog, y_puta_prvog, z_puta_prvog);
		glScalef(4, 0.25, 42);
		glutSolidCube(1);
	glPopMatrix();
}

void iscrtaj_put_drugi(){
	glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(x_puta_drugog, y_puta_drugog, z_puta_drugog);
		glScalef(4, 0.25, 42);
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

	if(z_puta_prvog<=-6){
		z_puta_prvog=14;
		postavi_prepreke();
	}
	if(z_puta_drugog<=-8){
		z_puta_drugog=14;
	}

	z_puta_prvog-=0.1+loptica;
	z_puta_drugog-=0.1+loptica;

	for(int i=0;i<prva_poz;i++){
		prepreke1[i].z-=0.1+loptica;
	}
	for(int i=0;i<prva_poz;i++){
		prepreke2[i].z-=0.1+loptica;
	}

	kolizija();

	glutPostRedisplay();

	if(animation_ongoing){
		glutTimerFunc(40, kretanje_coveka, 0);
	}
}

static void postavi_prepreke(){
	int broj_prepreka=0;
	prva_poz=0;
	for(int i=7;i<42;i+=7){
		Prepreka pr;
            	pr.x=(int)rand()%3+1;
            	pr.y=0;
            	pr.z=z_puta_prvog+i-19.5;
            	prepreke1[prva_poz++]=pr;
	}
}

void iscrtaj_loptice(int x, int y, int z){
	glPushMatrix();
        	glColor3f(0, 1, 0);
        	glTranslatef(x, y+0.5, z);
        	glutSolidSphere(0.125, 30, 30);
    	glPopMatrix();
}

float udaljenost(Prepreka pr){
	float x=pow((pr.x-x_coveka), 2);
	float y=pow((pr.y-y_coveka), 2);
	float z=pow((pr.z-z_coveka), 2);

	return sqrt(x+y+z);
}

void kolizija(){
	if(z_puta_prvog<z_puta_drugog){
		for(int i=0;i<prva_poz;i++){
			if(udaljenost(prepreke1[i])<=1.5){
				loptica=0.2;
			}
		}
	}
	else{
		for(int i=0;i<druga_poz;i++){
			if(udaljenost(prepreke2[i])<=1.5){
				loptica=0.2;
			}
		}
	}
}
