#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include "MathHelper.h"		// NEW!
#include "cube.h"
#include "shader.h"



#define NUM_VERTICES num_vertices
#define NUM_INDICES num_indices	
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint positionID, normalID;
GLuint indexBufferID; 

//all id variables
GLuint	perspectiveMatrixID, viewMatrixID, modelMatrixID;	
GLuint	allRotsMatrixID; 

//matricies for the different rotations
GLfloat* rotXMatrix;	
GLfloat* rotYMatrix;	
GLfloat* rotZMatrix;	
GLfloat* allRotsMatrix;	

//other transformation matricies
GLfloat* transMatrix;	
GLfloat* scaleMatrix;	
GLfloat* tempMatrix1;	

//matricies for the model view and camera
GLfloat* M;				
GLfloat* V;				
GLfloat* P;						


//light information
GLuint lightID;
GLfloat light[] = {1.0f,0.0f,0.0f,1.0f};


//variables to hold the different movements
GLfloat horizontal = 0.25f;
GLfloat vertical = -0.25f;
GLfloat theta = 160.00f;
GLfloat scaleAmount=1.0f;
GLfloat camX, camY, camZ;

void initMatrices() {

	scaleAmount = 0.5f;

	rotXMatrix = new GLfloat[16];	 MathHelper::makeIdentity(rotXMatrix);
	rotYMatrix = new GLfloat[16];	 MathHelper::makeIdentity(rotYMatrix);
	rotZMatrix = new GLfloat[16];	 MathHelper::makeIdentity(rotZMatrix);
	allRotsMatrix = new GLfloat[16]; MathHelper::makeIdentity(allRotsMatrix);
	transMatrix = new GLfloat[16];	 MathHelper::makeIdentity(transMatrix);
	scaleMatrix = new GLfloat[16];	 MathHelper::makeIdentity(scaleMatrix);
	tempMatrix1 = new GLfloat[16];	 MathHelper::makeIdentity(tempMatrix1);
	M = new GLfloat[16];			 MathHelper::makeIdentity(M);
	V = new GLfloat[16];			 MathHelper::makeIdentity(V);
	P = new GLfloat[16];			 MathHelper::makeIdentity(P);

	MathHelper::makePerspectiveMatrix(P, 60.0f, 1.0f, 1.0f, 1000.0f);

}

void changeViewport(int w, int h){
	glViewport(0, 0, w, h);
}


void render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	//setting up the model matrix
	MathHelper::makeScale(scaleMatrix, scaleAmount, scaleAmount, scaleAmount);	// Fill the scaleMatrix variable
	MathHelper::makeRotateY(rotYMatrix, theta);						// Fill the rotYMatrix variable
	MathHelper::makeTranslate(transMatrix, horizontal, vertical, -2.0f);
	MathHelper::matrixMult4x4(tempMatrix1, rotYMatrix, scaleMatrix);
	MathHelper::matrixMult4x4(M, transMatrix, tempMatrix1);				
	MathHelper::copyMatrix(rotYMatrix, allRotsMatrix);

	//seeting up the view matrix
	MathHelper::makeRotateY(rotYMatrix, 0);
	MathHelper::makeTranslate(transMatrix, camX, camY, camZ);
	MathHelper::matrixMult4x4(V, rotYMatrix, scaleMatrix);	
	
	//pass the information to the shaders
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, M);
	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, V);
	glUniformMatrix4fv(perspectiveMatrixID, 1, GL_TRUE, P);
	glUniformMatrix4fv(allRotsMatrixID, 1, GL_TRUE, allRotsMatrix);
	glUniform4fv(lightID, 1, light);
	
	glDrawElements (GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);
	glutSwapBuffers();
	glutPostRedisplay();
}


void keyboard( unsigned char key, int x, int y ){
    //update the variables based on key press
    switch( key ) {
	case 'a':
		horizontal -= 0.1;
		camX -=0.1f;
		break;
	case 'd':
		horizontal += 0.1;
		camX +=0.1f;
		break;
	case 'w':
		vertical += 0.1;
		camZ +=0.1f;
		break;
	case 's':
		vertical -= 0.1;
		camZ -=0.1f;
		break;
	case 'e':
		scaleAmount -= 0.1;
		if(scaleAmount<0)
			scaleAmount=0;
		break;
	case 'q':
		scaleAmount += 0.1;
		break;
	case 'r':
		theta += 0.1;
		break;
	case 033:
	    exit( EXIT_SUCCESS );
	    break;
    }
}

int main (int argc, char** argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Diffuse Lighting");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glewInit();

	initMatrices(); 

	// Make a shader
	shaderProgramID = createShaders();

	//set up vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//set up vbo
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	

	glBufferData(GL_ARRAY_BUFFER, 6*NUM_VERTICES*sizeof(GLfloat), NULL, GL_STATIC_DRAW);		// NEW!! - We're only loading vertices and normals (6 elements, not 7)
	
	//sub the data in
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3*NUM_VERTICES*sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3*NUM_VERTICES*sizeof(GLfloat),3*NUM_VERTICES*sizeof(GLfloat), normals);
	
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES*sizeof(GLuint), indices, GL_STATIC_DRAW);
	
	// Find the position of the variables in the shader
	positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
	normalID = glGetAttribLocation(shaderProgramID, "s_vNormal");
	lightID = glGetUniformLocation(shaderProgramID, "vLight");	// NEW
	
	perspectiveMatrixID = glGetUniformLocation(shaderProgramID, "mP");
	viewMatrixID = glGetUniformLocation(shaderProgramID, "mV");
	modelMatrixID = glGetUniformLocation(shaderProgramID, "mM");
	allRotsMatrixID = glGetUniformLocation(shaderProgramID, "mRotations");	// NEW

	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
	
	glUseProgram(shaderProgramID);
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(normalID);
	
	glutKeyboardFunc(keyboard);

	glEnable(GL_CULL_FACE);  
	glCullFace(GL_BACK);	
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();
	
	return 0;
}
