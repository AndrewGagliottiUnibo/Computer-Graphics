#pragma once
#include "Lib.h"
#include "Strutture.h"
#include "enum.h"
extern float cameraSpeed;
extern bool visualizzaAncora;
extern vec3 asse;
extern string Operazione;
extern string stringa_asse;
extern vector<Mesh> Scena;

void modifyModelMatrix(vec3 translation_vector, vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
{
	// ricordare che mat4(1) costruisce una matrice identit� di ordine 4
	mat4 traslation = translate(mat4(1), translation_vector);
	mat4 scales = scale(mat4(1), vec3(scale_factor, scale_factor, scale_factor));
	mat4 rotation = rotate(mat4(1), angle, rotation_vector);

	//Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotarion*traslation

	Scena[selected_obj].ModelM = Scena[selected_obj].ModelM * scales * rotation * traslation;

	glutPostRedisplay();
}

void moveCameraForward()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position += ViewSetup.direction * cameraSpeed;
}

void moveCameraBack()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position -= ViewSetup.direction * cameraSpeed;
}

void moveCameraLeft()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
	// e muovo la camera a sinistra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = cross(vec3(ViewSetup.direction), vec3(ViewSetup.upVector));
	ViewSetup.position -= vec4((direzione_scorrimento), .0) * cameraSpeed;
	ViewSetup.target -= vec4((direzione_scorrimento), .0) * cameraSpeed;
}

void moveCameraRight()
{
	// Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
	// e muovo la camera a destra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = cross(vec3(ViewSetup.direction), vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position += vec4(direzione_scorrimento, 0);
	ViewSetup.target += vec4(direzione_scorrimento, 0);
}

void moveCameraUp()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;  //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	vec3 direzione_scorrimento = normalize(cross(vec3(ViewSetup.direction), vec3(ViewSetup.upVector)));
	vec3 upDirection = cross(vec3(ViewSetup.direction), direzione_scorrimento) * cameraSpeed;
	ViewSetup.position -= vec4(upDirection, 0.0);
	ViewSetup.target -= vec4(upDirection, 0.0);
}

void moveCameraDown()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = normalize(cross(vec3(ViewSetup.direction), vec3(ViewSetup.upVector)));
	vec3 upDirection = cross(vec3(ViewSetup.direction), direzione_scorrimento) * cameraSpeed;
	ViewSetup.position += vec4(upDirection, 0.0);
	ViewSetup.target += vec4(upDirection, 0.0);
}

//Gestione eventi tastiera per il movimento della telecamera
void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'a':
			moveCameraLeft(); 
			break;
		case 'd':
			moveCameraRight();
			break;
		case 'w':
			moveCameraForward();
			break;
		case 's':
			moveCameraBack();
			break;
		case 'q':
			moveCameraUp();
			break;
		case 'e':
			moveCameraDown();
			break;
		case 'v':  //Visualizzazione ancora
			visualizzaAncora = TRUE;
			break;
		case 27:
			glutLeaveMainLoop();
			break;
		default:
			break;
		}

	glutPostRedisplay();
}
