#include "Mesh.h"

/// CONSTRUCTOR
///
/// In our constructor, we passed in our file name so we can seek our 
/// file. We then call the InitMesh function and as a parameter we use 
/// our OBJLoader and subsequently call 'ToIndexedModel' which returns an 
/// indexed model with our already loaded OBJ file. Using an indexed model 
/// is more efficient as we don't create copies of vertices whenever two faces
/// share an edge. This is all we require to make sure our mesh is ready to be 
/// drawn to screen. 
Mesh::Mesh(const std::string& filename)
{
	InitMesh(ObjLoader(filename).ToIndexedModel());
}

/// DECONSTRUCTOR
///
/// We use this function to delete our vertex array object 
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_vertexArrayObject);
}

/// DRAW
///
/// We use this function to bind our vertex array and 
/// then simply call glDrawElementsBaseVertex, quite simply
/// this function uses our element array buffer to gather our 
/// mesh data and efficiently draw it to screen. Then simply unbind it 
/// for neatness.
void Mesh::Draw()
{
	glBindVertexArray(m_vertexArrayObject);

	glDrawElementsBaseVertex(GL_TRIANGLES,m_numIndices,GL_UNSIGNED_INT,0,0);

	glBindVertexArray(0);
}

/// INIT_MESH
///
/// For this function we take in an indexed model and then define numindices 
/// as the size of our indices vector contained in our model. We then generate and 
/// bind our VAO to store our VBOs. Then in the same way generate however many VBOs 
/// we need (to the value of num_buffers). Afterwards, in order: we bind our buffer object
/// to the relevant array object. We send our relevant data to our VAO, so in position_vb's 
/// case this would be our vertices. Then we tell opengl which attribute we want this to be 
/// stored to, so in position's case it would be 0, for uvs it would be 1 ect. We use the enable attrib array
/// to do this and then vertexattribpointer which tells the difference between a new vertex and a new texture coordinate
/// as an example. At the end we simply unbind our VAO for neatness. 
void Mesh::InitMesh(const IndexedModel& model)
{
	m_numIndices = model.m_indices.size(); 


	glGenVertexArrays(1, &m_vertexArrayObject); 
	glBindVertexArray(m_vertexArrayObject); 


	glGenBuffers(NUM_BUFFERS, m_vertexBufferObject); 

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[POSITION_VB]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.m_positions[0]) * model.m_positions.size(), &model.m_positions[0], GL_STATIC_DRAW); 
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 


	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[TEXCOORD_VB]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.m_texcoordinates[0]) * model.m_texcoordinates.size(), &model.m_texcoordinates[0], GL_STATIC_DRAW); 
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBufferObject[INDEX_VB]); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.m_indices[0]) * model.m_indices.size(), &model.m_indices[0], GL_STATIC_DRAW); 

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject[NORMAL_VB]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.m_normals[0]) * model.m_normals.size(), &model.m_normals[0], GL_STATIC_DRAW); 
	glEnableVertexAttribArray(2); 
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0); 

	glBindVertexArray(0); 
}
