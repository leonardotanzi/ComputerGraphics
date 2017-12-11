#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <gl/GLUT.h>
#include <math.h>
#include <string>
#include <string.h>
#include <sstream>

#include "Sound.h"
#include "AssimpObj.h"
using namespace std;

//////////////////// VARIABILI E COSTANTI ////////////////////

// 13: codice ascii del tasto ENTER (INVIO) - 32: codice ascii della BARRA SPAZIATRICE
#define ENTER 13
#define SPACEBAR 32

// Valori Menu
#define RESUME 1
#define STOP 2

// Creo oggetto di tipo Sound, per l'esecuzione dell'audio di gioco
Sound sound;

// Creo oggetti di tipo AssimpObj, per importare i modelli di Blender
AssimpObj ball, tree;

// Interruttori di gioco, menu e gameover
bool gameOn = false, gameOver = false, menuOn = false;
char *gameOverString = "GAME OVER", *newGameString = "NUOVA PARTITA", *finalScoreString = "Punteggio: ", *resumeString = "Riprendi", *stopString = "Termina gioco";
const float xNewGame = 0.5f, yNewGame = 0.0f, xGameOver = -0.5f, yGameOver = 2.0f, xFinalScore = -0.55f, yFinalScore = 1.5f, changeColorInterval = 1000.0f;
float changeColorTime = 0.0f;
int menu;

// N degli oggetti generati
const int NLAVA = 2, NWATER = 2, NCOINS = 5, NTREES = 10;

// Raggi/lati degli oggetti
float ballRayInit = 0.3f*2, ballRay = ballRayInit, coinRay = 0.15f;
const float lavaEdge = 0.8f, waterEdge = 0.8f, treeEdge = 1.0f, yPool = 0.02f;

// Valori minimi e massimi della x della palla e degli oggetti
const float xMin = -2.01f, xMax = 2.01f;
// Salto della palla
const float yMin = 0.0f, yMax = 2*ballRay + coinRay + 0.5f;
float yIncr = 0.1f;
bool jumpOn = false;
// Valori minimi e massimi del raggio della palla
float ballRayMin = 0.1f, ballRayMax = 2*ballRayInit;
// Velocità con cui si muovono gli oggetti
const float speedInit = 0.5f, minSpeed = 0.001f, maxSpeed = 1.5f, speedIncr = 0.001f;
float speed = speedInit;
// Valore di partenza della palla
const float xBallInit = 0.0f;
float xBall = xBallInit, yBall = yMin;
// Velocità della palla
const float xMovement = 0.1f;
// Velocità di rotazione degli oggetti (palla e moneta)
const float angleInit = 0.0f, speedRot = 2.0f;
float ballAngle = angleInit, coinAngle = angleInit;
// Incremento/decremento palla
const float ballIncr = 0.05f;
// Gestione del colore della lava (in particolare, della componente verde (G))
const float lavaColorInit = 0.0f;
float lavaColor = lavaColorInit, lavaColorIncr = 0.01f;
// Gestione del colore dell'acqua (in particolare, delle componenti rosso e verde (RG))
const float waterColorInit = 0.0f;
float waterColor = waterColorInit, waterColorIncr = 0.01f;

// Posizione iniziale oggetti
float startDistance = 100.0f;
// Valori iniziali delle x e delle z degli oggetti
float xLava[] = { 0.0f, 0.0f };
float zLava[] = { startDistance, startDistance };
float xWater[] = { 0.0f, 0.0f };
float zWater[] = { startDistance, startDistance };
float xCoin[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float yCoin[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float zCoin[] = { startDistance, startDistance, startDistance, startDistance, startDistance };
float xTree[] = { startDistance, startDistance, startDistance, startDistance, startDistance };
float zTree[] = { startDistance, startDistance, startDistance, startDistance, startDistance };
bool lavaCollision[] = { false, false };
bool waterCollision[] = { false, false };

// Valori HUD
// Vite
const float lifeRay = 0.1f, xLifeCenter = -1.7f, yLifeCenter = 2.7f, distanceBetweenLifes = 0.01f;
const int maxLifes = 5, nLifesInit = 3;
float nLifes = nLifesInit;
// Punteggio
const float xPoints = 0.0f, yPoints = 2.5f, xMult = 1.5f, yMult = 2.5f;
int pointsInit = 0, lavaPoints = 50, waterPoints = 50, coinPoints = 100, multiplierCountInit = 10, multiplierIncr = 5;
int points = pointsInit, multiplierCount = multiplierCountInit;

//////////////////// FUNZIONI ////////////////////

// Calcola il modulo di un numero
float modulo(float n) {
	if (n < 0) n = n - 2 * n;
	return n;
}

// Scrive su schermo una stringa in posizione (x, y, 0) (value = eventuale int da convertire in stringa, se string == "")
void drawText(float x, float y, char *string, int value)
{
	char myStr[10];
	if (string == "") {
		sprintf_s(myStr, "%d", value);
		string = myStr;
	}
	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);
	//get the length of the string to display
	int len = (int)strlen(string);

	//loop to display character by character
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
};

void setMaterialLights(GLfloat* specular, GLfloat* shininess) {
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

// Funzioni che disegnano gli elementi di gioco, richiamate nel drawRandomObject()
void drawGround() {
	glColor3f(0.5f, 0.2f, 0.2f);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	glColor3f(0.3f, 0.1f, 0.1f);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(xMin - 0.5f, 0.001f, -100.0f);
	glVertex3f(xMin - 0.5f, 0.001f,  100.0f);
	glVertex3f(xMax + 0.5f, 0.001f,  100.0f);
	glVertex3f(xMax + 0.5f, 0.001f, -100.0f);
	glEnd();
}

void drawBall() {
	glColor3f(0.4f, 0.2f, 0.1f);
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat shininess[] = { 55.0 };
	setMaterialLights(specular, shininess);

	// Angolo di rotazione
	if (modulo(ballAngle) >= 360) ballAngle = 0;
	else ballAngle += speedRot;
	// Salto e calcolo della y
	if (jumpOn) {
		yBall += yIncr;
		if (yBall < 0.0f) yBall = 0.0f;
		if (yBall >= yMax || yBall <= yMin) yIncr -= 2 * yIncr;
		if (yBall <= yMin) jumpOn = false;
	}

	glPushMatrix();
	glTranslatef(xBall, yBall+ballRay, 0.0f);
	glRotatef(ballAngle + speedRot, -1, 0, 0);
	//glutSolidSphere(ballRay, 20, 20);
	glEnable(GL_TEXTURE_2D);
	const struct aiScene* myscene = ball.getScene();
	ball.recursive_render(myscene, myscene->mRootNode, ballRay);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glPopMatrix();
}

void drawCoin() {
	glColor3f(1.0f, 1.0f, 0.0f);
	GLfloat specular[] = { 1.0, 1.0, 0.0, 1.0 };
	GLfloat shininess[] = { 55.0 };
	setMaterialLights(specular, shininess);

	if (modulo(coinAngle) >= 360) coinAngle = 0;
	else coinAngle += speedRot;
	glTranslatef(0.0f, coinRay, 0.0f);
	glRotatef(coinAngle + speedRot, 0, 1, 0);
	glutSolidTorus(coinRay/2, coinRay, 20, 20);
}

void drawLava() {
	glColor3f(1.0f, lavaColor, 0.0f);
	GLfloat specular[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat shininess[] = { 55.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-lavaEdge / 2, yPool, -lavaEdge / 2);
	glVertex3f(-lavaEdge / 2, yPool, lavaEdge / 2);
	glVertex3f(lavaEdge / 2, yPool, lavaEdge / 2);
	glVertex3f(lavaEdge / 2, yPool, -lavaEdge / 2);
	glEnd();
}

void drawWater() {
	glColor3f(waterColor, waterColor, 1.0f);
	GLfloat specular[] = { 0.0, 1.0, 1.0, 1.0 };
	GLfloat shininess[] = { 55.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-waterEdge / 2, yPool, -waterEdge / 2);
	glVertex3f(-waterEdge / 2, yPool, waterEdge / 2);
	glVertex3f(waterEdge / 2, yPool, waterEdge / 2);
	glVertex3f(waterEdge / 2, yPool, -waterEdge / 2);
	glEnd();
}

void drawTree() {
	glColor3f(0.4f, 0.2f, 0.1f);
	glEnable(GL_TEXTURE_2D);
	const struct aiScene* myscene = ball.getScene();
	tree.recursive_render(myscene, myscene->mRootNode, treeEdge);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	/*
    glBegin(GL_QUADS);
	glVertex3f(-treeEdge / 8, -treeEdge, 0.0f);
	glVertex3f(-treeEdge / 8, treeEdge, 0.0f);
	glVertex3f(treeEdge / 8, treeEdge, 0.0f);
	glVertex3f(treeEdge / 8, -treeEdge, 0.0f);
	glEnd();
	*/
}

void drawLife(float green) {
	glColor3f(0.0f, green, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, 1);
	glVertex2f(xLifeCenter, yLifeCenter);
	for(float ang = 0; ang <= 360; ang = ang + 15.0f)
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
	glColor3f(0.0f, 0.0f, 0.0f);
	// Scrivo i punti
	drawText(xPoints, yPoints, "", points);
	// Scrivo il moltiplicatore
	char tmp[10], *multStr;
	sprintf_s(tmp, "x%d", multiplierCount / 10);
	multStr = tmp;
	drawText(xMult, yMult, multStr, -1);
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo LAVA
bool collision_with_lava(int OBJECT, int arrayIndex, float myedge, float x, float y, float z) {
	for (int i = 0; i < NLAVA; i++) {
		if ( (OBJECT != LAVA) || (i != arrayIndex) ) {
			if ( (modulo(xLava[i]-x) < (lavaEdge+myedge)) && (modulo(zLava[i]-z) < (lavaEdge+myedge)) && (y <= myedge) ) {
				// Collisione
				if ( (OBJECT == BALL) && (!lavaCollision[i]) ) {
					sound.Play(SOUND_LAVA);
					lavaCollision[i] = true;
					if (nLifes < maxLifes) {
						nLifes++;
						ballRay += ballIncr;
					}
					points += lavaPoints*(multiplierCount / 10);
				}
				return true;
			}
			else {
				if ( (OBJECT == BALL) && (lavaCollision[i]) ) {
					lavaCollision[i] = false;
				}
			}
		}
	}
	return false;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo WATER
bool collision_with_water(int OBJECT, int arrayIndex, float myedge, float x, float y, float z) {
	for (int i = 0; i < NWATER; i++) {
		if ((OBJECT != WATER) || (i != arrayIndex)) {
			if ((modulo(xWater[i] - x) < (waterEdge + myedge)) && (modulo(zWater[i] - z) < (waterEdge + myedge)) && (y <= myedge)) {
				// Collisione
				if ((OBJECT == BALL) && (!waterCollision[i])) {
					sound.Play(SOUND_WATER);
					waterCollision[i] = true;
					if (nLifes > 0) {
						nLifes--;
						ballRay -= ballIncr;
						points -= waterPoints;
						multiplierCount = 10;
						if (nLifes == 0) {
							sound.StopAll();
							sound.Play(SOUND_GAMEOVER);
							glutDestroyMenu(menu);
						}
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
				return true;
			}
		}
	}
	return false;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo COIN
int collision_with_a_coin(float myedge, float x, float y, float z) {
	for (int i = 0; i < NCOINS; i++) {
		if ((modulo(xCoin[i] - x) < (coinRay + myedge)) && (modulo(zCoin[i] - z) < (coinRay + myedge)) && (modulo(yCoin[i] - y) < (coinRay + 2*myedge)) ) {
			// Collisione
			sound.Play(SOUND_COIN);
			points += coinPoints*(multiplierCount / 10);
			multiplierCount += multiplierIncr;
			zCoin[i] = 100.0f;// coinRay + 2 * ballRay + 0.1f;
			return i;
		}
	}
	return -1;
}

// Funzione che genera un qualunque oggetto in modo casuale
void drawRandomObject(int OBJECT) {
	float *x, *y, *z;
	int size = 0, edge = 0;
	switch (OBJECT) {
	case LAVA:
		size = NLAVA;
		edge = lavaEdge;
		x = xLava;
		z = zLava;
		lavaColor += lavaColorIncr;
		if ( (lavaColor >= 0.7f) || (lavaColor <= 0.0f) )
			lavaColorIncr = lavaColorIncr - 2*lavaColorIncr;
		break;
	case WATER:
		size = NWATER;
		edge = waterEdge;
		x = xWater;
		z = zWater;
		waterColor += waterColorIncr;
		if ((waterColor >= 0.7f) || (waterColor <= 0.0f))
			waterColorIncr = waterColorIncr - 2 * waterColorIncr;
		break;
	case COIN:
		size = NCOINS;
		edge = coinRay;
		x = xCoin;
		y = yCoin;
		z = zCoin;
		break;
	case TREE:
		size = NTREES;
		edge = treeEdge;
		x = xTree;
		z = zTree;
		break;
	}

	int distanceFromBall = 10;
	// Ripete il ciclo "size" volte, dove size è il numero di oggetti di quel tipo generati
	for (int i = 0; i < size; i++) {
		if (z[i] > ballRay * distanceFromBall) {
			// Una volta superata la palla, l'oggetto viene rigenerato con una probabilità di 1/500
			// Serve a non generare gli oggetti tutti allo stesso tempo
			int okGenerate = rand() % 100;
			int attempt = 0;
			bool generate = false;
			if (okGenerate == 1) {
				do {
					attempt++;
					x[i] = xMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (xMax - xMin)));
					// Incrementa maggiormente la x se l'oggetto è un albero
					if (OBJECT == TREE) x[i] = modulo(x[i]) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f)));
					if (OBJECT == COIN) y[i] = yMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (yMax - yMin)));
					z[i] = -30.0f;
					
					if ( (!collision_with_lava(OBJECT, i, edge, x[i], 0, z[i])) && (!collision_with_water(OBJECT, i, edge, x[i], 0, z[i])) ) {
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
				glTranslatef(x[i], y[i] + coinRay, z[i]);
				drawCoin();
				collision_with_a_coin(ballRay, xBall, yBall, 0.0);
				break;
			case TREE:
				glTranslatef(xMin - x[i], 0, z[i]);
				drawTree();
				glPopMatrix();
				glPushMatrix();
				glTranslatef(xMax + x[i], 0, z[i]);
				drawTree();
			}
			glPopMatrix();
		}
	}
	
	collision_with_lava(BALL, -1, 0, xBall, yBall, 0);
	collision_with_water(BALL, -1, 0, xBall, yBall, 0);
}

// Menu
// Opzioni Menu
void processMenu(int value) {
	if (gameOn) {
		menuOn = true;
		switch (value) {
		case RESUME:
			gameOn = true;
			menuOn = false;
			break;
		case STOP:
			gameOn = false;
			gameOver = true;
			menuOn = false;
			sound.StopAll();
			sound.Play(SOUND_GAMEOVER);
			glutDestroyMenu(menu);
			break;
		}
		glutPostRedisplay();
	}
}

// Creazione del menu
void createMenu() {
	menu = glutCreateMenu(processMenu);
	glutAddMenuEntry(resumeString, RESUME);
	glutAddMenuEntry(stopString, STOP);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Funzione di diplay
void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Set the camera
	gluLookAt(0.0f, 1.0f, 5.0f,
		0.0f, 1.0f, 4.0f,
		0.0f, 1.0f, 0.0f);
	/*gluLookAt(0.0f, 15.0f, 15.0f,
		0.0f, 1.0f, 4.0f,
		0.0f, 1.0f, 0.0f);*/

	glClear(GL_COLOR_BUFFER_BIT);
	// Se il gioco è in corso
	if (gameOn && !menuOn) {
		// Draw elements
		drawHud();
		drawGround();
		drawBall();
		drawRandomObject(LAVA);
		drawRandomObject(WATER);
		drawRandomObject(COIN);
		drawRandomObject(TREE);

		// Verifico se è gameover
		if (nLifes < 1) {
			gameOn = false;
			gameOver = true;
		}
	}
	// Se si è alla schermata iniziale
	else if (!gameOn && !menuOn) {
		glDisable(GL_LIGHTING);
		glClearColor(0.8f, 0.5f, 0.1f, 1.0f);
		if (gameOver) {
			glColor3f(1.0f, 0.8f, 0.8f);
			drawText(xGameOver, yGameOver, gameOverString, -1);
			char tmp[50], *pointsStr;
			sprintf_s(tmp, "%s%d", finalScoreString, points);
			pointsStr = tmp;
			glColor3f(0.0f, 0.0f, 0.0f);
			drawText(xFinalScore, yFinalScore, pointsStr, -1);
		}
		if (changeColorTime < changeColorInterval) {
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		else if (changeColorTime < 2 * changeColorInterval) {
			glColor3f(0.0f, 0.0f, 0.0f);
		}
		else {
			changeColorTime = 0.0f;
		}
		changeColorTime += 1.0f;

		drawText(xNewGame, yNewGame, newGameString, -1);
	}

	glutSwapBuffers();
}

// Funzione di inizializzazione
void init() {
	ballRay = ballRayInit;
	speed = speedInit;
	xBall = xBallInit;
	ballAngle = angleInit;
	coinAngle = angleInit;
	nLifes = nLifesInit;
	points = pointsInit;
	lavaColor = lavaColorInit;
	lavaColorIncr = modulo(lavaColorIncr);
	waterColor = waterColorInit;
	waterColorIncr = modulo(waterColorIncr);

	for (int i = 0; i < NLAVA; i++) {
		xLava[i] = 0.0f;
		zLava[i] = startDistance;
		lavaCollision[i] = false;
	}
	for (int i = 0; i < NWATER; i++) {
		xWater[i] = 0.0f;
		zWater[i] = startDistance;
		waterCollision[i] = false;
	}
	for (int i = 0; i < NCOINS; i++) {
		xCoin[i] = 0.0f;
		yCoin[i] = 0.0f;
		zCoin[i] = startDistance;
	}
	for (int i = 0; i < NTREES; i++) {
		xTree[i] = 0.0f;
		zTree[i] = startDistance;
	}
	
	// Illuminazione
	GLfloat ambient[] = { 0.0,0.0,0.0,1.0 };
	GLfloat diffuse[] = { 1.0,1.0,1.0,1.0 };
	GLfloat position[] = { 0.0,3.0,2.0,0.0 };
	GLfloat lmodel_amb[] = { 0.4f,0.4f,0.4f,1.0f };
	GLfloat local_view[] = { 0.0 };
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_amb);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glEnable(GL_NORMALIZE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glClearColor(0.8f, 0.5f, 0.1f, 0.0f);

	sound.StopAll();
	sound.Play(SOUND_AMBIENT);
	createMenu();
}

// Funzione dei tasti normali
void processNormalKeys(unsigned char key, int x, int y) {
	if (!menuOn)
	switch (key) {
	case ENTER:
		if (!gameOn) {
			sound.Play(SOUND_CHOOSE);
			gameOn = true;
			gameOver = false;
			changeColorTime = 0.0f;
			init();
		}
		break;
	case SPACEBAR:
		if (gameOn && !jumpOn) {
			sound.Play(SOUND_JUMP);
			jumpOn = true;
		}
		break;
	}
}

// Funzione dei tasti speciali
void processSpecialKeys(int key, int xx, int yy) {
	if (!menuOn) {
		switch (key) {
		// SINISTRA - DESTRA: sposta palla
		case GLUT_KEY_LEFT:
			if (xBall > xMin)	xBall -= xMovement;
			break;
		case GLUT_KEY_RIGHT:
			if (xBall < xMax)	xBall += xMovement;
			break;
		// SU - GIU': modifica velocità di gioco
		case GLUT_KEY_UP:
			if (speed < maxSpeed)	speed += speedIncr;
			break;
		case GLUT_KEY_DOWN:
			if (speed > minSpeed)	speed -= speedIncr;
			break;
		}
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


int main(int argc, char **argv) {

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Fireball");
	sound.Load();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	ball.InitGL(BALL);
	tree.InitGL(TREE);
	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
