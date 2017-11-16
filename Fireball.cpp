#include <stdio.h>
#include <stdlib.h>
#include <gl/GLUT.h>
#include <math.h>

// Id degli oggetti
const int LAVA = 0, WATER = 1, COIN = 2;
// N degli oggetti generati
const int NLAVA = 2, NWATER = 2, NCOINS = 5;

// Raggi/lati degli oggetti
float ballRay = 0.4f, coinRay = 0.2f;
float lavaEdge = 0.8f, waterEdge = 0.8f;

// Valori minimi e massimi della x della palla e degli oggetti
float xMin = -2.01f, xMax = 2.01f;
// Velocità con cui si muovono gli oggetti
float velocity = 0.005f;

// Valore di partenza della palla
float xBall = 0.0f;
// Velocità della palla
float xMovement = 0.1f;

// Valori iniziali delle x e delle z degli oggetti
float xLava[2] = { 0.0f, 0.0f };
float zLava[2] = { 0.0f, 0.0f };
float xWater[] = { 0.0f, 0.0f };
float zWater[] = { 0.0f, 0.0f };
float xCoin[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float zCoin[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

// Funzioni che disegnano gli elementi di gioco, richiamate nel drawRandomObject()
void drawGround() {
	glColor3f(0.5f, 0.2f, 0.2f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
}

void drawBall() {
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(xBall, ballRay, 0.0f);
	glutSolidSphere(ballRay, 20, 20);
	glPopMatrix();
}

void drawCoin() {
	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, coinRay, 0.0f);
	glutSolidSphere(coinRay, 20, 20);
}

void drawLava() {
	glColor3f(1.0f, 0.4f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(-lavaEdge/2, 0.02f, -lavaEdge/2);
	glVertex3f(-lavaEdge/2, 0.02f, lavaEdge/2);
	glVertex3f(lavaEdge/2, 0.02f, lavaEdge/2);
	glVertex3f(lavaEdge/2, 0.02f, -lavaEdge/2);
	glEnd();
}

void drawWater() {
	glColor3f(0.0f, 0.4f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(-waterEdge / 2, 0.01f, -waterEdge / 2);
	glVertex3f(-waterEdge / 2, 0.01f, waterEdge / 2);
	glVertex3f(waterEdge / 2, 0.01f, waterEdge / 2);
	glVertex3f(waterEdge / 2, 0.01f, -waterEdge / 2);
	glEnd();
}

// Funzione che genera un qualunque oggetto in modo casuale
void drawRandomObject(int OBJECT) {
	float *x, *z;
	int size = 0, edge = 0;
	switch (OBJECT) {
	case LAVA:
		size = NLAVA;
		edge = lavaEdge;
		x = xLava;
		z = zLava;
		break;
	case WATER:
		size = NWATER;
		edge = waterEdge;
		x = xWater;
		z = zWater;
		break;
	case COIN:
		size = NCOINS;
		edge = coinRay;
		x = xCoin;
		z = zCoin;
		break;
	}

	// Ripete il ciclo "size" volte, dove size è il numero di oggetti di quel tipo generati
	for (int i = 0; i < size; i++) {
		if (z[i] > edge * 5) {
			// Una volta superata la palla, l'oggetto viene rigenerato con una probabilità di 1/500
			// Serve a non generare gli oggetti tutti allo stesso tempo
			int okGenerate = rand() % 500;
			if (okGenerate == 1) {
				x[i] = xMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (xMax - xMin)));
				z[i] = -30.0f;
			}
		}
		else {
			// Altrimenti l'oggetto si muove incrementando la z
			z[i] = z[i] + velocity;
			glPushMatrix();
			switch (OBJECT) {
			case LAVA:
				glTranslatef(x[i], 0, z[i]);
				drawLava();
				break;
			case WATER:
				glTranslatef(x[i], 0, z[i]);
				drawWater();
				break;
			case COIN:
				glTranslatef(x[i], coinRay, z[i]);
				drawCoin();
				break;
			}
			glPopMatrix();
		}
	}
}

// Funzione di diplay
void renderScene(void) {

	glClearColor(0.8f, 0.5f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Set the camera
	gluLookAt(0.0f, 1.0f, 5.0f,//z,
		0.0f,/*x + lx,*/ 1.0f, 4.0f,//z + lz,
		0.0f, 1.0f, 0.0f);

	// Draw elements
	drawGround();
	drawBall();
	drawRandomObject(LAVA);
	drawRandomObject(WATER);
	drawRandomObject(COIN);

	glutSwapBuffers();
}

// Funzione dei tasti speciali
void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.1f;

	switch (key) {
	case GLUT_KEY_LEFT:
		if (xBall > xMin)	xBall -= xMovement;
		break;
	case GLUT_KEY_RIGHT:
		if (xBall < xMax)	xBall += xMovement;
		break;
	}
}

// Funzione per il ridimensionamento della finestra
void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you can't make a window of zero width)
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

// Funzione di inizializzazione
void init() {

}

int main(int argc, char **argv) {

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Fireball");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	//glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	init();

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
