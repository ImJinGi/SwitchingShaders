#ifndef ___SPHERE_H
#define ___SPHERE_H

#include "GL/glew.h"
#include <GL/GL.h>
#include <glm/mat4x4.hpp>

#include <vector>

#include "ObjectColor.h"


class Sphere
{
public:
	Sphere();
	Sphere(float rad, GLuint sl, GLuint st);
	~Sphere();

	void setup(float rad, GLuint sl, GLuint st);
	void draw();
	int getVertexArrayHandle();

	GLuint VAO, VBO_position, VBO_normal, IBO;

	objectColor *sphereColor;
	
private:
	GLuint nVerts, elements;
	float radius;
	GLuint slices, stacks;

	void generateVerts(float *, float *, float *, GLuint *);
	void setup(GLfloat *v, GLfloat *n, unsigned int *el);

};


#endif
