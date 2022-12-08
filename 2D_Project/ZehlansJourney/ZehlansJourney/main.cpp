#include <iostream>
#include "Lib.h"
#include "Values.h"
#include "ShaderMaker.h"
#include "EventHandler.h"
#include "Shape.h"
#include "Elements.h"
#include "CreateGeometries.h"

// Global variables C++
unsigned int lsceltavs, lsceltafs, loc_time, loc_res;
static unsigned int programId;
extern bool hitboxOn = false;

// Global items variables
Elements *Scene = new Elements();

// Global variables GLM
mat4 Projection;
GLuint MatProj, MatModel;

// Shader initializer
void INIT_SHADER()
{
	GLenum ErrorCheckValue = glGetError();

	char *vertexShader = (char *)"vertexShaderC.glsl";
	char *fragmentShader = (char *)"fragmentShaderC.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

// Vertex Array Object initializer
void INIT_VAO()
{
	// A magic tool we're gonna use later
	srand(time(NULL));

	// MotherShip
	Scene->getMotherShip()->posx = WIDTH / shipPositionX;
	Scene->getMotherShip()->posy = HEIGHT / shipPositionY;
	Scene->getMotherShip()->scalex = shipScale;
	Scene->getMotherShip()->scaley = shipScale;
	buildShip(Scene->getMotherShip(), cTopShip, cBotShip);
	create_VAO_Vector(Scene->getMotherShip());

	// Blob
	Scene->getBlob()->scalex = blobScale;
	Scene->getBlob()->scaley = blobScale;
	Scene->getBlob()->nTriangles = blobTriangles;
	buildBlob(Scene->getBlob(), cBlobIn, cBlobOut);
	create_VAO_Vector(Scene->getBlob());

	// Bullet
	Scene->getBullet()->posx = Scene->getMotherShip()->posx + bulletXDelay;
	Scene->getBullet()->posy = Scene->getMotherShip()->posy + bulletYDelay;
	Scene->getBullet()->scalex = bulletScale;
	Scene->getBullet()->scaley = bulletScale;
	buildBullet(Scene->getBullet(), cBullet);
	create_VAO_Vector(Scene->getBullet());

	// Background stars
	for (int i = 0; i < maxStars; i++)
	{
		Scene->getStars(i)->nTriangles = starsTriangles;
		buildStars(Scene->getStars(i), cStarsCenter, cStarsEdges);
		create_VAO_Vector(Scene->getStars(i));
	}
	// Background
	Scene->getBackground()->posx = DEFAULT;
	Scene->getBackground()->posy = DEFAULT;
	Scene->getBackground()->scalex = float(WIDTH);
	Scene->getBackground()->scaley = float(HEIGHT);
	buildBackground(Scene->getBackground());
	create_VAO_Vector(Scene->getBackground());

	// Costruzione della matrice di Proiezione
	Projection = ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");

	lsceltafs = glGetUniformLocation(programId, "sceltaFS");
	lsceltavs = glGetUniformLocation(programId, "sceltaVS");
	loc_time = glGetUniformLocation(programId, "time");
	loc_res = glGetUniformLocation(programId, "res");
}

/*
	The drawscene(void) method is called every time the screen is refreshed, creating the sense of animation.
	This means this method is called multiple times like a while loop.
*/
void drawscene(void)
{
	float time = glutGet(GLUT_ELAPSED_TIME);
	glUniform1f(loc_time, time);
	glUniform2f(loc_res, WIDTH, HEIGHT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));

	// Background
	Scene->getBackground()->choiceVS = 0;
	Scene->getBackground()->choiceFS = 1;
	glUniform1i(lsceltavs, Scene->getBackground()->choiceVS);
	glUniform1i(lsceltafs, Scene->getBackground()->choiceFS);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scene->getBackground()->Model));
	glBindVertexArray(Scene->getBackground()->VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, Scene->getBackground()->nv);
	glBindVertexArray(0);

	// Background Stars
	for (int i = 0; i < maxStars; i++)
	{
		Scene->getStars(i)->choiceVS = 1;
		Scene->getStars(i)->choiceFS = 0;
		glUniform1i(lsceltavs, Scene->getStars(i)->choiceVS);
		glUniform1i(lsceltafs, Scene->getStars(i)->choiceFS);
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scene->getStars(i)->Model));
		glBindVertexArray(Scene->getStars(i)->VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scene->getStars(i)->nv);
		glBindVertexArray(0);
	}

	// Bullet
	Scene->getBullet()->choiceVS = 0;
	Scene->getBullet()->choiceFS = 0;
	// Ricalcolo la matrice perch� devo gestire le animazioni e devo ricalcolare il tutto
	Scene->getBullet()->Model = mat4(1.0);
	Scene->getBullet()->Model = translate(Scene->getBullet()->Model, vec3(Scene->getBullet()->posx, Scene->getBullet()->posy, 0.0));
	Scene->getBullet()->Model = scale(Scene->getBullet()->Model, vec3(bulletScale, bulletScale, 1.0));
	// Bounding box transformations
	Scene->getBullet()->corner_t = Scene->getBullet()->Model * Scene->getBullet()->corner_t_obj;
	Scene->getBullet()->corner_b = Scene->getBullet()->Model * Scene->getBullet()->corner_b_obj;
	glUniform1i(lsceltavs, Scene->getBullet()->choiceVS);
	glUniform1i(lsceltafs, Scene->getBullet()->choiceFS);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scene->getBullet()->Model));
	glBindVertexArray(Scene->getBullet()->VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scene->getBullet()->nv);
	glBindVertexArray(0);

	// Mothership
	Scene->getMotherShip()->choiceVS = 1;
	Scene->getMotherShip()->choiceFS = 0;
	// Ricalcolo la matrice perch� devo gestire le animazioni e devo ricalcolare il tutto
	Scene->getMotherShip()->Model = mat4(1.0);
	Scene->getMotherShip()->Model = translate(Scene->getMotherShip()->Model, vec3(Scene->getMotherShip()->posx, Scene->getMotherShip()->posy, 0.0));
	Scene->getMotherShip()->Model = scale(Scene->getMotherShip()->Model, vec3(shipScale, shipScale, 1.0));
	glUniform1i(lsceltavs, Scene->getMotherShip()->choiceVS);
	glUniform1i(lsceltafs, Scene->getMotherShip()->choiceFS);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scene->getMotherShip()->Model));
	glBindVertexArray(Scene->getMotherShip()->VAO);
	if (hitboxOn)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, 0, Scene->getMotherShip()->nv);
	}
	else
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scene->getMotherShip()->nv);
	}
	glBindVertexArray(0);

	// Blob
	Scene->getBlob()->choiceVS = 1;
	Scene->getBlob()->choiceFS = 0;
	// Ricalcolo la matrice perch� devo gestire le animazioni e devo ricalcolare il tutto
	Scene->getBlob()->Model = mat4(1.0);
	Scene->getBlob()->Model = translate(Scene->getBlob()->Model, vec3(Scene->getBlob()->posx, Scene->getBlob()->posy, 0.0));
	Scene->getBlob()->Model = scale(Scene->getBlob()->Model, vec3(blobScale, blobScale, 1.0));
	// Bounding box transformations
	Scene->getBlob()->corner_t = Scene->getBlob()->corner_t_obj;
	Scene->getBlob()->corner_b = Scene->getBlob()->corner_b_obj;
	Scene->getBlob()->corner_t = Scene->getBlob()->Model * Scene->getBlob()->corner_t_obj;
	Scene->getBlob()->corner_b = Scene->getBlob()->Model * Scene->getBlob()->corner_b_obj;
	glUniform1i(lsceltavs, Scene->getBlob()->choiceVS);
	glUniform1i(lsceltafs, Scene->getBlob()->choiceFS);
	glBindVertexArray(Scene->getBlob()->VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scene->getBlob()->Model));
	if (hitboxOn)
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scene->getBlob()->nv - 6);
		glDrawArrays(GL_LINE_STRIP, Scene->getBlob()->nv - 6, 6);
	}
	else
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scene->getBlob()->nv - 6);
	}
	glBindVertexArray(0);

	// Usare queste istruzioni con cautela
	// Use this instructions carefully
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glBindVertexArray(0);

	glutSwapBuffers();
}

void main(int argc, char *argv[])
{
	// Initial setting
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// Display setting
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Zehlan's Journey");
	glutDisplayFunc(drawscene);
	glutReshapeFunc(resize);

	// Events
	glutKeyboardFunc(keyboardPressedEvent);
	glutMouseFunc(mouse);
	glutTimerFunc(66, update, 0);
	glutTimerFunc(66, updateBlob, 0);
	glutTimerFunc(66, scoreUp, 0);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();
}
