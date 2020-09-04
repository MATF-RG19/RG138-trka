#include<GL/glut.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include"image.h"

#define NUMBER_OF_DOTS 10
#define PI 3.1415926535

/*Teksture*/
#define FILENAME0 "startgame.bmp"
#define FILENAME1 "gameover.bmp"

#define PREPREKA_NITRO 100
#define PREPREKA_KAMEN 200
#define PREPREKA_POEN 300
#define TRAJANJE_NITRO 200

/*Niz u koji smestamo teksture*/
static GLuint names[2];

static int score=0;
static int obrisi=-100;

/*Pozicije coveka*/
static float x_coveka=2;
static float y_coveka=0.25;
static float z_coveka=0;

/*Pozicije prve i druge ravni*/
static float x_puta_prvog=2;
static float y_puta_prvog=-0.5;
static float z_puta_prvog=10;

static float x_puta_drugog=2;
static float y_puta_drugog=-0.5;
static float z_puta_drugog=50;

/*Promenljive koje nam sluze za postavljanje prepreka*/
static int prva_poz=0;
static int druga_poz=0;

/*Promenljive za faze*/
static int prva_faza=1;
static int druga_faza=0;
static int treca_faza=0;
static int prva_faza_trajanje=900;
static int druga_faza_trajanje=500;

/*Promenljive za nitro*/
static float nitro=0;
static float nitro_levo=0;
static float nitro_desno=0;

static float nitro_boost=0.3;
static float nitro_boost_levo=0.05;
static float nitro_boost_desno=0.05;
static int nitro_trajanje=0;

static int window_width, window_height, animation_ongoing;
static int kretanje[]={0, 0};

/*Niz koji nam odredjuje vrstu_prepreke*/
static int vrsta_prepreke[]={PREPREKA_NITRO, PREPREKA_KAMEN, PREPREKA_POEN};

/*Struktura koja predstavlja prepreku*/
typedef struct{
	float x, y, z;
	int vrsta_prepreke;
	int doslo_do_kolizije;
} Prepreka;

/*Nizovi za prepreke za svaku ravan*/
Prepreka prepreke1[80];
Prepreka prepreke2[80];

/*Deklaracije funkcija koje koristimo*/

static void start_game();
static void game_over();

static void nacrtaj_coveka(void);
static void kretanje_coveka(int value);
static void postavi_prepreke1();
static void postavi_prepreke2();
static void iscrtaj_nitro(float x, float y, float z);
static void iscrtaj_kamen(float x, float y, float z);
static void iscrtaj_poen(float x, float y, float z);
static void iscrtaj_put_prvi();
static void iscrtaj_put_drugi();
static void kolizija();
static float udaljenost(Prepreka pr);
static int mutiraj_nitro();
static int mutiraj_poen();
static int u_nizu(float niz[], float prepreka);
static int izaberi_prepreku();
static int da_li_postaviti_prepreku();
static float generisi_broj();

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_release(unsigned char key, int x, int y);

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Trka");

	/* Objekat koji predstavlja teskturu ucitanu iz fajla. */
	Image* image;

	/* Ukljucuju se teksture. */
	glEnable(GL_TEXTURE_2D);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/*
     	* Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     	* fajla.
     	*/
	image = image_init(0, 0);

	/* Generisu se identifikatori tekstura. */
	glGenTextures(2, names);
	
	/* Kreira se prva tekstura. */
	image_read(image, FILENAME0);

	glBindTexture(GL_TEXTURE_2D, names[0]);
    	glTexParameteri(GL_TEXTURE_2D,
                    	GL_TEXTURE_WRAP_S, GL_CLAMP);
    	glTexParameteri(GL_TEXTURE_2D,
                    	GL_TEXTURE_WRAP_T, GL_CLAMP);
    	glTexParameteri(GL_TEXTURE_2D,
                    	GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D,
                    	GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 	image->width, image->height, 0,
                 	GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	image_read(image, FILENAME1);

	glBindTexture(GL_TEXTURE_2D, names[1]);
    	glTexParameteri(GL_TEXTURE_2D,
                    	GL_TEXTURE_WRAP_S, GL_CLAMP);
    	glTexParameteri(GL_TEXTURE_2D,
                    	GL_TEXTURE_WRAP_T, GL_CLAMP);
    	glTexParameteri(GL_TEXTURE_2D,
                    	GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D,
                    	GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 	image->width, image->height, 0,
                 	GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	/* Iskljucujemo aktivnu teksturu */
    	glBindTexture(GL_TEXTURE_2D, 0);

    	/* Unistava se objekat za citanje tekstura iz fajla. */
    	image_done(image);

	/*Sito za generisanje random brojeva*/
	srand(time(NULL));

	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutKeyboardUpFunc(on_release);
	glutDisplayFunc(start_game);

	glClearColor(0, 0.5, 1, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	animation_ongoing=0;

	glutFullScreen();

	glutMainLoop();

	return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:/*Izlazimo iz igre*/
			exit(0);
			break;
		case 'S':
		case 's':/*Pokrece se igra*/
			if(!animation_ongoing){
				glutDisplayFunc(on_display);
				glutTimerFunc(20, kretanje_coveka, 0);
				animation_ongoing=1;
			}
			break;
		case 'P':
		case 'p':/*Pauzira se*/
			animation_ongoing=0;
			break;
		case 'R':
		case 'r':/*Ponovo se pokrece*/
			animation_ongoing=0;
			score=0;
			obrisi=-100;
			x_coveka=2;
			y_coveka=0.25;
			z_coveka=0;
			x_puta_prvog=2;
			y_puta_prvog=-0.5;
			z_puta_prvog=10;
			x_puta_drugog=2;
			y_puta_drugog=-0.5;
			z_puta_drugog=50;
			prva_poz=0;
			druga_poz=0;
			prva_faza=1;
			druga_faza=0;
			treca_faza=0;
			prva_faza_trajanje=900;
			druga_faza_trajanje=500;
			nitro=0;
			nitro_levo=0;
			nitro_desno=0;
			nitro_boost=0.3;
			nitro_boost_levo=0.05;
			nitro_boost_desno=0.05;
			nitro_trajanje=0;
			glutDisplayFunc(on_display);
			glutPostRedisplay();
			break;
		case 'A':
		case 'a':/*Skrece levo*/
			kretanje[0]=1;
			glutPostRedisplay();
			break;
		case 'D':
		case 'd':/*Skrece desno*/
			kretanje[1]=1;
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
	gluLookAt(x_coveka, y_coveka+2, -6+z_coveka,
		x_coveka, y_coveka, 7+z_coveka,
		0, 1, 0);

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

	/*Iscrtavamo na ekranu score*/
	glPushMatrix();
		glColor3f(1, 0, 0);
		glRasterPos3f(2.5, 3.5, 5);

		char score_tekst[50]="SCORE: ";
		char score_vrednost[50];

		sprintf(score_vrednost, "%d ", score);
		strcat(score_tekst, score_vrednost);

		int len=(int)strlen(score_tekst);
		for(int i=0;i<len;i++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score_tekst[i]);
		}
	glPopMatrix();

	/*Iscrtavamo na ekranu nitro*/
	glPushMatrix();
		glColor3f(1, 0, 0);
		glRasterPos3f(2.5, 3, 5);

		char nitro_tekst[50]="NITRO: ";
		char nitro_vrednost[50];

		sprintf(nitro_vrednost, "%d ", nitro_trajanje);
		strcat(nitro_tekst, nitro_vrednost);

		int len1=(int)strlen(nitro_tekst);
		for(int i=0;i<len1;i++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, nitro_tekst[i]);
		}
	glPopMatrix();

	iscrtaj_put_prvi();
	iscrtaj_put_drugi();
	nacrtaj_coveka();

	/*Iscrtavamo prepreke u zavisnosti od ravni na kojoj je se nalazimo*/
	for(int i=0;i<prva_poz;i++){
		Prepreka pr=prepreke1[i];
		switch(pr.vrsta_prepreke){
			case PREPREKA_NITRO:
				if(pr.doslo_do_kolizije==1){
					iscrtaj_nitro(pr.x, pr.y+obrisi, pr.z);
				}
				else{
					iscrtaj_nitro(pr.x, pr.y, pr.z);
				}
				break;
			case PREPREKA_KAMEN:
				iscrtaj_kamen(pr.x, pr.y, pr.z);
				break;
			case PREPREKA_POEN:
				if(pr.doslo_do_kolizije==1){
					iscrtaj_poen(pr.x, pr.y+obrisi, pr.z);
				}
				else{
					iscrtaj_poen(pr.x, pr.y, pr.z);
				}
				break;
		}
	}

	for(int i=0;i<druga_poz;i++){
		Prepreka pr=prepreke2[i];
		switch(pr.vrsta_prepreke){
			case PREPREKA_NITRO:
				if(pr.doslo_do_kolizije==1){
					iscrtaj_nitro(pr.x, pr.y+obrisi, pr.z);
				}
				else{
					iscrtaj_nitro(pr.x, pr.y, pr.z);
				}
				break;
			case PREPREKA_KAMEN:
				iscrtaj_kamen(pr.x, pr.y, pr.z);
				break;
			case PREPREKA_POEN:
				if(pr.doslo_do_kolizije==1){
					iscrtaj_poen(pr.x, pr.y+obrisi, pr.z);
				}
				else{
					iscrtaj_poen(pr.x, pr.y, pr.z);
				}
				break;
		}
	}

	glutSwapBuffers();
}

static void iscrtaj_put_prvi(){
	glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(x_puta_prvog, y_puta_prvog, z_puta_prvog);
		glScalef(4, 0.25, 42);
		glutSolidCube(1);
	glPopMatrix();
}

static void iscrtaj_put_drugi(){
	glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(x_puta_drugog, y_puta_drugog, z_puta_drugog);
		glScalef(4, 0.25, 42);
		glutSolidCube(1);
	glPopMatrix();
}

static void nacrtaj_coveka(){
	glColor3f(0, 0, 0);
	glLineWidth(5);

	glPushMatrix();
		glTranslatef(x_coveka, y_coveka, z_coveka);

		/*Iscrtavamo glavu*/
		glBegin(GL_POLYGON);
	       		glVertex2f(0, 0);
			for(int i=0;i<360;i++){
				glVertex2f(cos(2*i*PI/NUMBER_OF_DOTS)*0.1,
						sin(2*i*PI/NUMBER_OF_DOTS)*0.1);
			}
	    	glEnd();

		/*Isrcatavamo telo*/
		glBegin(GL_LINES);
		       	glVertex2f(0, 0);
			glVertex2f(0, -0.5);
		glEnd();

		/*Isrcatavamo ruke*/
		glBegin(GL_LINES);
			glVertex2f(0, -0.2);
			glVertex2f(-0.2, -0.28);
		glEnd();

		glBegin(GL_LINES);
		    	glVertex2f(0, -0.2);
			glVertex2f(0.2, -0.28);
		glEnd();

		/*Isrcatavamo noge*/
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

	/*Menjamo kordinatu coveka, u zavisnosti da li se krece levo ili desno*/
	/*Postavljamo ogranicenje da ne bi izasla sa staze*/
	if(kretanje[0] && x_coveka<3.8){
		x_coveka+=0.07+nitro_levo;
	}
	if(kretanje[1] && x_coveka>0.2){
		x_coveka-=0.07+nitro_desno;
	}

	/*Menjamo vresnost nitra*/
	if(nitro_trajanje!=0){
		nitro_trajanje--;
	}
	else if(nitro_trajanje==0){
		nitro=0;
		nitro_levo=0;
		nitro_desno=0;
	}

	/*Pomeramo ravni*/
	z_puta_prvog-=0.25+nitro;
	z_puta_drugog-=0.25+nitro;
	printf("BRZINA:%f\n",0.25 + nitro);
	
	/*Menjamo faze*/
	if(prva_faza){
		prva_faza_trajanje--;
		if(prva_faza_trajanje==0){
			prva_faza=0;
			druga_faza=1;
		}
	}

	if(druga_faza){
		druga_faza_trajanje--;
		if(druga_faza_trajanje==0){
			druga_faza=0;
			treca_faza=1;
		}
	}

	if(!treca_faza){
        	printf("Trajanje 1.faza: %d Trajanje 2.faza: %d\n", prva_faza_trajanje, druga_faza_trajanje);
	}
	else{
		printf("Aktivna je treca faza!!!\n");
	}
	printf("Trajanje nitra:--%d--\n", nitro_trajanje);

	/*Pomeramo prepreke*/
	for(int i=0;i<prva_poz;i++){
		prepreke1[i].z-=0.25+nitro;
	}
	for(int i=0;i<druga_poz;i++){
		prepreke2[i].z-=0.25+nitro;
	}

	/*Naizmenicno postavljamo prvu i drugu ravan*/
	if(z_puta_prvog<=-20){
		z_puta_prvog=60;
		postavi_prepreke1();
	}
	if(z_puta_drugog<=-20){
		z_puta_drugog=60;
		postavi_prepreke2();
	}


	kolizija();

	glutPostRedisplay();

	if(animation_ongoing){
		glutTimerFunc(20, kretanje_coveka, 0);
	}
}

static void postavi_prepreke1(){
	int broj_prepreka=0;
	prva_poz=0;
	for(int i=7;i<42;i+=7){
		if(prva_faza){
			if(!da_li_postaviti_prepreku()){
				continue;
			}
		}

		if(druga_faza){
			if(!da_li_postaviti_prepreku()){
				continue;
			}
		}

		broj_prepreka=((int)rand()%3)+1;
		int trenutna_prepreka=0;

		float niz_pozicija[]={0, 0, 0};/*Ubaceno za sprecavanje da se 2 prepreke nadju na isptom mestu*/

		while(trenutna_prepreka<broj_prepreka){
			Prepreka pr;
            	
			int prepreka_broj=izaberi_prepreku();/*Biramo random prepreku*/
			pr.vrsta_prepreke=vrsta_prepreke[prepreka_broj];
			if(pr.vrsta_prepreke==PREPREKA_NITRO){
				if(mutiraj_nitro()){
					pr.vrsta_prepreke=PREPREKA_POEN;
				}
			}
			if(pr.vrsta_prepreke==PREPREKA_POEN){
				if(mutiraj_poen()){
					pr.vrsta_prepreke=PREPREKA_KAMEN;
				}
			}

			float tmp=0+generisi_broj();

			while(u_nizu(niz_pozicija, tmp)){/*Sprecavanje da se 2 prepreke nadju na isptom mestu*/
				tmp=0+generisi_broj();
			}

			pr.x=tmp;
		    	pr.y=0.125;
		    	pr.z=z_puta_prvog+i-19.5;
			pr.doslo_do_kolizije=0;
			prepreke1[prva_poz++]=pr;
			niz_pozicija[trenutna_prepreka]=tmp;
			trenutna_prepreka++;
		}
	}
}

static void postavi_prepreke2(){
	int broj_prepreka=0;
	druga_poz=0;
	for(int i=7;i<42;i+=7){
		if(prva_faza){
			if(!da_li_postaviti_prepreku()){
				continue;
			}
		}

		broj_prepreka=((int)rand()%3)+1;
		int trenutna_prepreka=0;

		float niz_pozicija[]={0, 0, 0};

		while(trenutna_prepreka<broj_prepreka){
			Prepreka pr;
            	
			int prepreka_broj=izaberi_prepreku();
			pr.vrsta_prepreke=vrsta_prepreke[prepreka_broj];
			if(pr.vrsta_prepreke==PREPREKA_NITRO){
				if(mutiraj_nitro()){
					pr.vrsta_prepreke=PREPREKA_POEN;
				}
			}
			if(pr.vrsta_prepreke==PREPREKA_POEN){
				if(mutiraj_poen()){
					pr.vrsta_prepreke=PREPREKA_KAMEN;
				}
			}

			float tmp=0+generisi_broj();

			while(u_nizu(niz_pozicija, tmp)){
				tmp=0+generisi_broj();
			}

			pr.x=tmp;
		    	pr.y=0.125;
		    	pr.z=z_puta_drugog+i-19.5;
			pr.doslo_do_kolizije=0;
			prepreke2[druga_poz++]=pr;
			niz_pozicija[trenutna_prepreka]=tmp;
			trenutna_prepreka++;
		}
	}
}

static void iscrtaj_kamen(float x, float y, float z){
	glPushMatrix();
		glColor3f(0.658824, 0.658824, 0.658824);
		glTranslatef(x, y-0.125, z);
		glutSolidCube(0.4);
	glPopMatrix();
}

static void iscrtaj_poen(float x, float y, float z){
	glPushMatrix();
        	glColor3f(1, 1, 0);
        	glTranslatef(x, y, z);
        	glutSolidSphere(0.125, 30, 30);
    	glPopMatrix();
}

static float udaljenost(Prepreka pr){
	float x=powf((pr.x-x_coveka), 2);
	float y=powf((pr.y-y_coveka), 2);
	float z=powf((pr.z-z_coveka), 2);

	return sqrtf(x+y+z);
}

static void kolizija(){
	if(z_puta_prvog<z_puta_drugog){/*Gledamo na kojoj smo ravni*/
		for(int i=0;i<prva_poz;i++){
			if(udaljenost(prepreke1[i])<=0.325){
				Prepreka pr=prepreke1[i];
				switch(pr.vrsta_prepreke){
					case PREPREKA_NITRO:/*Ako smo udarili u nitro, ubrzavamo*/
						prepreke1[i].doslo_do_kolizije=1;
						if(nitro_trajanje){
							nitro+=nitro_boost;
							nitro_levo+=nitro_boost_levo;
							nitro_desno+=nitro_boost_desno;
							nitro_trajanje+=TRAJANJE_NITRO;
							break;
						}
						nitro=nitro_boost;
						nitro_levo=nitro_boost_levo;
						nitro_desno=nitro_boost_desno;
						nitro_trajanje=TRAJANJE_NITRO;
						break;
					case PREPREKA_POEN:/*Ako smo udarili u poen, povecavamo score*/
						score+=5;
						prepreke1[i].doslo_do_kolizije=1;
						break;
					case PREPREKA_KAMEN:/*Igra je gotova*/
						animation_ongoing=0;
						glutDisplayFunc(game_over);
                    				glutPostRedisplay();
				}
			}
		}
	}
	else{
		for(int i=0;i<druga_poz;i++){
			if(udaljenost(prepreke2[i])<=0.325){
				Prepreka pr=prepreke2[i];
				switch(pr.vrsta_prepreke){
					case PREPREKA_NITRO:/*Ako smo udarili u nitro, ubrzavamo*/
						prepreke2[i].doslo_do_kolizije=1;
						if(nitro_trajanje){
							nitro+=nitro_boost;
							nitro_levo+=nitro_boost_levo;
							nitro_desno+=nitro_boost_desno;
							nitro_trajanje+=TRAJANJE_NITRO;
							break;
						}
						nitro=nitro_boost;
						nitro_levo=nitro_boost_levo;
						nitro_desno=nitro_boost_desno;
						nitro_trajanje=TRAJANJE_NITRO;
						break;
					case PREPREKA_POEN:/*Ako smo udarili u poen, povecavamo score*/
						score+=5;
						prepreke2[i].doslo_do_kolizije=1;
						break;
					case PREPREKA_KAMEN:/*Igra je gotova*/
						animation_ongoing=0;
						glutDisplayFunc(game_over);
                    				glutPostRedisplay();
				}
			}
		}
	}
}

static int da_li_postaviti_prepreku(){
	int da_ili_ne=(int)rand()%2;
	return da_ili_ne;
}

static float generisi_broj(){
	float niz[]={0.25, 1.1, 1.95, 2.8, 3.65};
	int broj=5;
	int index=(int) rand()%broj;
	float izbor=niz[index];

	return izbor;
}

static int izaberi_prepreku(){
	int br_prepreka=3;
	int br=(int)rand()%br_prepreka;
	return br;
}

static int u_nizu(float niz[], float prepreka){
	for(int i=0;i<3;i++){
		if(prepreka==niz[i]){
			return 1;
		}
	}
	return 0;
}

static int mutiraj_poen(){
	int posto=(int)rand()%101;
	if(posto>70){
		return 1;
	}
	return 0;
}

static int mutiraj_nitro(){
	int posto=(int)rand()%101;
	if(posto>15){
		return 1;
	}
	return 0;
}

static void iscrtaj_nitro(float x, float y, float z){
	glPushMatrix();
		glColor3f(0, 0, 1);
        	glTranslatef(x, y+0.1, z);
        	glScalef(0.2, 0.4, 0.1);
        	glutSolidCube(1);
    	glPopMatrix();
    
    	glPushMatrix();
        	glColor3f(0, 0, 1);
        	glTranslatef(x, y+0.3, z);
        	glScalef(0.1, 0.1, 0.1);
        	glutSolidCube(1);
    	glPopMatrix();
    
    	glPushMatrix();
        	glColor3f(0, 0, 1);
        	glTranslatef(x, y + 0.4, z);
        	glScalef(0.25, 0.1, 0.1);
        	glutSolidCube(1);
    	glPopMatrix();
}

static void start_game(){
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glClearColor(0, 0, 0, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 3.5, 0, 0, 0, 0, 1, 0, 0);

	glBindTexture(GL_TEXTURE_2D, names[0]);
	
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(-2, 0, -3.5);
			
		glTexCoord2f(0, 1);
		glVertex3f(2, 0, -3.5);
			
		glTexCoord2f(1, 1);
		glVertex3f(2, 0, 3.5);
			
		glTexCoord2f(1, 0);
		glVertex3f(-2, 0, 3.5);
	glEnd();
        
	glBindTexture(GL_TEXTURE_2D, 0);

	glutSwapBuffers();
}

static void game_over(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0, 0, 0, 0);

	glMatrixMode(GL_MODELVIEW);
    	glLoadIdentity();
    	gluLookAt(0, 3.5, 0, 0, 0, 0, 1, 0, 0);

	glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, names[1]);
	
		glBegin(GL_POLYGON);
			glTexCoord2f(0, 0);
			glVertex3f(-2, 0, -3.5);
			
			glTexCoord2f(0, 1);
			glVertex3f(2, 0, -3.5);
			
			glTexCoord2f(1, 1);
			glVertex3f(2, 0, 3.5);
			
			glTexCoord2f(1, 0);
			glVertex3f(-2, 0, 3.5);
		glEnd();
        
	glDisable(GL_TEXTURE_2D);

	
	glPushMatrix();
		glColor3f(0, 0, 0);
		glRasterPos3f(0.5, 0.5, 0.45);

		char score_tekst[50]=" ";
		char score_vrednost[50];

		sprintf(score_vrednost, " %d ", score);
		strcat(score_tekst, score_vrednost);

		int len=(int)strlen(score_tekst);
		for(int i=0;i<len;i++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score_tekst[i]);
		}
	glPopMatrix();
	
    	glutSwapBuffers();
}
