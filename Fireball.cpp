#include <stdio.h>
#include <stdlib.h>
#include <gl/GLUT.h>
#include <math.h>
#include <string>
#include <string.h>
#include <sstream>

// Id degli oggetti
const int BALL = 0, LAVA = 1, WATER = 2, COIN = 3;
// N degli oggetti generati
const int NLAVA = 2, NWATER = 2, NCOINS = 5;

// Raggi/lati degli oggetti
float ballRay = 0.3f, coinRay = 0.15f;
float lavaEdge = 0.8f, waterEdge = 0.8f;

// Valori minimi e massimi della x della palla e degli oggetti
float xMin = -2.01f, xMax = 2.01f;
// Valori minimi e massimi del raggio della palla
float ballRayMin = 0.1f, ballRayMax = 2*ballRay;
// Velocità con cui si muovono gli oggetti
float speed = 0.01f, minSpeed = 0.001f, maxSpeed = 0.05f, speedIncr = 0.001f;
// Valore di partenza della palla
float xBall = 0.0f;
// Velocità della palla
float xMovement = 0.1f;
// Incremento/decremento palla
float ballIncr = 0.05f;

// Posizione iniziale oggetti
float startDistance = 100.0f;
// Valori iniziali delle x e delle z degli oggetti
float xLava[] = { 0.0f, 0.0f };
float zLava[] = { startDistance, startDistance };
float xWater[] = { 0.0f, 0.0f };
float zWater[] = { startDistance, startDistance };
float xCoin[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float zCoin[] = { startDistance, startDistance, startDistance, startDistance, startDistance };
bool lavaCollision[] = { false, false };
bool waterCollision[] = { false, false };

// Valori HUD
// Vite
float lifeRay = 0.1f, xLifeCenter = -1.7f, yLifeCenter = 2.7f, distanceBetweenLifes = 0.01f;
int maxLifes = 5, nLifes = 3;// maxLifes;
// Punteggio
int points = 0, lavaPoints = 50, waterPoints = 50, coinPoints = 100, multiplierCount = 10, multiplierIncr = 5;

char *IntToChar(int number)
{
	std::string s = std::to_string(number);
	char const *pchar = s.c_str();
	//char *s = pchar;
	//return pchar;
	return "";
}

void drawText(int x, int y, int z, char *string, int value)
{
	char const *myStr;
	if (string != "x") {
		glColor3f(1.0f, 1.0f, 1.0f);
		std::string s = std::to_string(value);
		char const *pchar = s.c_str();
		myStr = pchar;
		fprintf(stdout, "%s\n", myStr);
		//sprintf(string, "%d", value);
	}
	else {
		myStr = string;
	}
	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);
	//get the length of the string to display
	int len = (int)strlen(myStr);
	fprintf(stdout, "%d\n", len);

	//loop to display character by character
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, myStr[i]);
	}
};

// Funzioni che disegnano gli elementi di gioco, richiamate nel drawRandomObject()
void drawGround() {
	glColor3f(0.5f, 0.2f, 0.2f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	glColor3f(0.3f, 0.1f, 0.1f);
	glBegin(GL_QUADS);
	glVertex3f(xMin - 0.5f, 0.001f, -100.0f);
	glVertex3f(xMin - 0.5f, 0.001f,  100.0f);
	glVertex3f(xMax + 0.5f, 0.001f,  100.0f);
	glVertex3f(xMax + 0.5f, 0.001f, -100.0f);
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

void drawLife(float green) {
	glColor3f(0.0f, green, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(xLifeCenter, yLifeCenter);
	for(int ang = 0; ang <= 360; ang = ang + 15)
		glVertex2f(xLifeCenter + cos(ang)*lifeRay, yLifeCenter + sin(ang)*lifeRay);
	glEnd();
}

void drawHud() {
	// Disegno le vite
	glPushMatrix();
	for (int i = 0; i < maxLifes; i++) {
		if (i < nLifes) drawLife(1.0f);
		else drawLife(0.0f);
		glTranslatef(distanceBetweenLifes + 2*lifeRay, 0, 0);
	}
	glPopMatrix();
	// Scrivo i punti
	drawText(0, yLifeCenter, 0, "", points);
	// Scrivo il moltiplicatore
	drawText(0.9f, yLifeCenter, 0, "x", -1);
	drawText(0.92f, yLifeCenter, 0, "", multiplierCount%10);
}

// Calcola il modulo di un numero
float modulo(float n) {
	if (n < 0) n = n - 2*n;
	return n;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo LAVA
bool collision_with_lava(int OBJECT, int arrayIndex, float myedge, float x, float z) {
	for (int i = 0; i < NLAVA; i++) {
		if ( (OBJECT != LAVA) || (i != arrayIndex) ) {
			if ( (modulo(xLava[i]-x) < (lavaEdge+myedge)) && (modulo(zLava[i]-z) < (lavaEdge+myedge)) ) {
//				if (OBJECT == BALL) fprintf(stdout, "++: %f - Distance: x: %f<%f, z: %f<%f\n",
//					ballRay, modulo(xLava[i] - x), (lavaEdge + myedge), zLava[i], (lavaEdge + myedge));
				// Collisione
				if ( (OBJECT == BALL) && (!lavaCollision[i]) ) {
					lavaCollision[i] = true;
					fprintf(stdout, "lavacoll = true\n");
					if (nLifes < maxLifes) {
						nLifes++;
						ballRay += ballIncr;
						points += lavaPoints*(multiplierCount % 10);
					}
				}
				return true;
			}
			else {
				if ( (OBJECT == BALL) && (lavaCollision[i]) ) {
					lavaCollision[i] = false;
					fprintf(stdout, "lavacoll = false\n");
				}
			}
		}
	}
	return false;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo WATER
bool collision_with_water(int OBJECT, int arrayIndex, float myedge, float x, float z) {
	for (int i = 0; i < NWATER; i++) {
		if ((OBJECT != WATER) || (i != arrayIndex)) {
			if ((modulo(xWater[i] - x) < (waterEdge + myedge)) && (modulo(zWater[i] - z) < (waterEdge + myedge))) {
				// Collisione
				if ((OBJECT == BALL) && (!waterCollision[i])) {
					waterCollision[i] = true;
					if (nLifes > 0) {
						nLifes--;
						ballRay -= ballIncr;
						points -= waterPoints;
						multiplierCount = 10;
					}
				}
				return true;
			}
			else {
				if ((OBJECT == BALL) && (waterCollision[i])) {
					waterCollision[i] = false;
				}
			}
		}
	}
	return false;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo COIN
bool collision_with_coins(int OBJECT, int arrayIndex, float myedge, float x, float z) {
	bool collision = false;
	for (int i = 0; i < NCOINS; i++) {
		if ( (OBJECT != COIN) || (i != arrayIndex) ) {
			if ((modulo(xCoin[i] - x) < (coinRay + myedge)) && (modulo(zCoin[i] - z) < (coinRay + myedge))) {
				// Collisione
				points += coinPoints*(multiplierCount % 10);
				multiplierCount += multiplierIncr;
				return true;
			}
		}
	}
	return false;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo COIN
int collision_with_a_coin(float myedge, float x, float z) {
	for (int i = 0; i < NCOINS; i++) {
		if ((modulo(xCoin[i] - x) < (coinRay + myedge)) && (modulo(zCoin[i] - z) < (coinRay + myedge))) {
			// Collisione
			fprintf(stdout, "\a");
			//fprintf(stdout, "%d - z: %f\n", i, zCoin[i]);
			zCoin[i] = coinRay + 2 * ballRay + 0.1f;
			return i;
		}
	}
	return -1;
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

	int distanceFromBall = 20;
	// Ripete il ciclo "size" volte, dove size è il numero di oggetti di quel tipo generati
	for (int i = 0; i < size; i++) {
		if (z[i] > edge * distanceFromBall) {
			// Una volta superata la palla, l'oggetto viene rigenerato con una probabilità di 1/500
			// Serve a non generare gli oggetti tutti allo stesso tempo
			int okGenerate = rand() % 500;
			int attempt = 0;
			bool generate = false;
			if (okGenerate == 1) {
				do {
					attempt++;
					x[i] = xMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (xMax - xMin)));
					z[i] = -30.0f;
					
					if ( (!collision_with_lava(OBJECT, i, edge, x[i], z[i])) && (!collision_with_water(OBJECT, i, edge, x[i], z[i])) ) {
						generate = true;
					}

				} while ((attempt < 10) && (!generate));
				if (!generate) z[i] = edge * distanceFromBall;
			}
		}
		else {
			// Altrimenti l'oggetto si muove incrementando la z
			z[i] += speed;
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
				collision_with_a_coin(ballRay, xBall, 0.0);
				break;
			}
			glPopMatrix();
		}
		switch (OBJECT) {
		case LAVA:
			xLava[i] = x[i];
			zLava[i] = z[i];
			break;
		case WATER:
			xWater[i] = x[i];
			zWater[i] = z[i];
			break;
		case COIN:
			xCoin[i] = x[i];
			zCoin[i] = z[i];
			break;
		}
	}
	collision_with_lava(BALL, -1, 0, xBall, 0);
	collision_with_water(BALL, -1, 0, xBall, 0);
}

// Funzione di diplay
void renderScene(void) {

	glClearColor(0.8f, 0.5f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Set the camera
	gluLookAt(0.0f, 1.0f, 5.0f,
		0.0f, 1.0f, 4.0f,
		0.0f, 1.0f, 0.0f);

	// Draw elements
	drawGround();
	drawBall();
	drawRandomObject(LAVA);
	drawRandomObject(WATER);
	drawRandomObject(COIN);
	drawHud();

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
	case GLUT_KEY_UP:
		if (speed < maxSpeed)	speed += speedIncr;
		break;
	case GLUT_KEY_DOWN:
		if (speed > minSpeed)	speed -= speedIncr;
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
