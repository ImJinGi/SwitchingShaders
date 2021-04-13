

#include "floor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

std::vector<glm::vec4> floor_vertices;
std::vector<glm::vec3> floor_normals;

int _nvert;

Floor::Floor()
{
	floorColor = new objectColor();
	setup(50, 16);
}

void Floor::setup(float size, int nSquares)
{
	std::vector <glm::vec4> vlists;
	std::vector <glm::vec3> nlists;

	// parameters:
	float maxX = size / 2, maxY = size / 2;
	float minX = -size / 2, minY = -size / 2;

	int x, y, v[3], i;
	float xp, yp, xd, yd;
	v[2] = 0;
	xd = (maxX - minX) / ((float)nSquares);
	yd = (maxY - minY) / ((float)nSquares);

	for (x = 0, xp = minX; x < nSquares; x++, xp += xd) 
	{
		for (y = 0, yp = minY, i = x; y < nSquares; y++, i++, yp += yd) 
		{
			//floor_vertices.push_back(glm::vec4(xp, 0, yp, 1));
			//floor_vertices.push_back(glm::vec4(xp, 0, yp + yd, 1));
			//floor_vertices.push_back(glm::vec4(xp + xd, 0, yp + yd, 1));

			//floor_vertices.push_back(glm::vec4(xp, 0, yp, 1));
			//floor_vertices.push_back(glm::vec4(xp + xd, 0, yp + yd, 1));
			//floor_vertices.push_back(glm::vec4(xp + xd, 0, yp, 1));

			computeNormal(glm::vec4(xp, 0, yp, 1), glm::vec4(xp, 0, yp + yd, 1), glm::vec4(xp + xd, 0, yp + yd, 1));
			computeNormal(glm::vec4(xp, 0, yp, 1), glm::vec4(xp + xd, 0, yp + yd, 1), glm::vec4(xp + xd, 0, yp, 1));
			
		} // end of for j
	}// end of for i

	_nvert = floor_vertices.size();

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	//create vbo for vertices
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*floor_vertices.size() * 4, floor_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, // attribute
		4,                 // number of elements per vertex, here (x,y,z,1)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);
	glEnableVertexAttribArray(0);


	//create vbo for colors
	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*floor_normals.size() * 3, floor_normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		1, // attribute
		3,                 // number of elements per vertex, here (R,G,B)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Floor::draw()
{
	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, _nvert * 3);
	glBindVertexArray(0);
}


void Floor::computeNormal(std::vector<glm::vec4> &vertices,
	std::vector<glm::vec3> &normals,
	glm::vec4 pos[],
	int a, int b, int c, int d)
{
	vertices.push_back(pos[a]);
	vertices.push_back(pos[b]);
	vertices.push_back(pos[c]);

	glm::vec3 vec_a = glm::vec3((pos[b] - pos[a]).x, (pos[b] - pos[a]).y, (pos[b] - pos[a]).z);
	glm::vec3 vec_b = glm::vec3((pos[c] - pos[a]).x, (pos[c] - pos[a]).y, (pos[c] - pos[a]).z);
	glm::vec3 n1 = glm::cross(vec_a, vec_b);
	glm::normalize(n1);

	normals.push_back(n1);
	normals.push_back(n1);
	normals.push_back(n1);

	vertices.push_back(pos[a]);
	vertices.push_back(pos[c]);
	vertices.push_back(pos[d]);

	glm::vec3 vec_c = glm::vec3((pos[c] - pos[a]).x, (pos[c] - pos[a]).y, (pos[c] - pos[a]).z);
	glm::vec3 vec_d = glm::vec3((pos[d] - pos[a]).x, (pos[d] - pos[a]).y, (pos[d] - pos[a]).z);
	glm::vec3 n2 = glm::cross(vec_c, vec_d);
	glm::normalize(n2);

	normals.push_back(n2);
	normals.push_back(n2);
	normals.push_back(n2);
}


void Floor::computeNormal(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3)
{
	glm::vec3 v1 = glm::vec3(p2 - p1);
	v1 = glm::normalize(v1);
	glm::vec3 v2 = glm::vec3(p3 - p1);
	v2 = glm::normalize(v2);

	glm::vec3 n1 = glm::cross(v1, v2);

	floor_vertices.push_back(p1);
	floor_vertices.push_back(p2);
	floor_vertices.push_back(p3);

	floor_normals.push_back(n1);
	floor_normals.push_back(n1);
	floor_normals.push_back(n1);
}