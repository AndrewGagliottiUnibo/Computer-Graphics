#include "EventHandler.h"
#include "Values.h"

extern bool hitboxOn;
extern Elements *Scene;
static int score = 0;

void update(int value)
{
	glutPostRedisplay();
	glutTimerFunc(66, update, 0);
}

void resize(GLsizei w, GLsizei h)
{
	float AspectRatio_mondo = (float)(WIDTH) / (float)(HEIGHT);

	if (AspectRatio_mondo > w / h)
	{
		glViewport(0, 0, w, w / AspectRatio_mondo);
	}
	else
	{
		glViewport(0, 0, h * AspectRatio_mondo, h);
	}
}

void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		// Playsound( "bulletSound.wav", NULL, SND_SYNC);
		shoot(0);
		break;

	case 'a':
		updateShip('a');
		break;

	case 'd':
		updateShip('d');
		break;

	case 'z':
		updateShip('z');
		break;

	default:
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	if (state != GLUT_DOWN)
	{
		hitboxOn = false;
		return;
	}

	hitboxOn = true;
}

void updateShip(unsigned char key)
{
	switch (key)
	{
	case 'a':
		Scene->getMotherShip()->posx -= movementDelay;
		Scene->getBullet()->posx -= movementDelay;
		break;

	case 'd':
		Scene->getMotherShip()->posx += movementDelay;
		Scene->getBullet()->posx += movementDelay;
		break;

	case 'z':
		endGame();
		break;

	default:
		break;
	}

	glutTimerFunc(5, update, 0);
}

void updateBlob(int value)
{
	int choice = (rand() % 10);

	if (choice == 5 && Scene->getBlob()->posx < (WIDTH - blobDelay))
	{
		// cout << "destra" << endl;
		Scene->getBlob()->posx += movementDelay;
	}
	else if (choice == 2 && Scene->getBlob()->posx > blobDelay)
	{
		// cout << "sinistra" << endl;
		Scene->getBlob()->posx -= movementDelay;
	}

	glutTimerFunc(5, updateBlob, 0);
}

void shoot(int value)
{
	// Ordinata del proettile durante lo sparo
	Scene->getBullet()->posy += movementDelay;

	if (Scene->getBullet()->posy <= HEIGHT)
	{
		glutTimerFunc(5, shoot, 0);
	}
	else
	{
		Scene->getBullet()->posy = Scene->getMotherShip()->posy + bulletYDelay;
	}

	glutPostRedisplay();
}

bool checkCollision(Shape *blob, Shape *bullet)
{
	// collisioni su asse x
	bool collisionX = blob->corner_b.x <= bullet->corner_t.x &&
					  blob->corner_t.x >= bullet->corner_b.x;
	// collisioni su asse y
	bool collisionY = blob->corner_b.y <= bullet->corner_t.y &&
					  blob->corner_t.y >= bullet->corner_b.y;

	// Collision happens if there is collision both on x and y directions.
	return collisionX && collisionY;
}

void scoreUp(int value)
{
	if (checkCollision(Scene->getBlob(), Scene->getBullet()))
	{
		// cout << score << endl;
		score++;
		Scene->getBlob()->posx = blobDelay + (rand() % (WIDTH - blobDelay));
		Scene->getBlob()->posy = (blobBottomHeight + rand() % blobTopPhase);
	}

	glutTimerFunc(5, scoreUp, 0);
}

int getScore()
{
	return score;
}

void endGame()
{
	exit(0);
}
