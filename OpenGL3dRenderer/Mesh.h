#pragma once

#include "Headers.h"

/// CLASS INDEXED_MODEL
///
/// Forward decleration of Indexed Model defined in OBJLoader
class IndexedModel;

/// CLASS MESH
///
/// This class is used to contain our indexed model we load from our OBJLoader and 
/// then pipe the data to our GPU for use in our shaders. We use a vertex array object
/// and an array of buffer objects to pass our data to the GPU. 
class Mesh
{
public:
	Mesh(const std::string& filename);

	void Draw();

	~Mesh();

private:

	void InitMesh(const IndexedModel& model);
	
	enum
	{
		POSITION_VB, 

		TEXCOORD_VB,

		NORMAL_VB,

		INDEX_VB,

		NUM_BUFFERS
	};

	GLuint m_vertexArrayObject;
	GLuint m_vertexBufferObject[NUM_BUFFERS];
	unsigned int m_numIndices;
};

