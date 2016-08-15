#pragma once

#include "Headers.h"

/// CLASS OBJ_Index
/// 
/// This class is used to contain our index values for all of our
/// indices. We also need to compare these so we can sort them later  
/// so we overload the < operator so we can sort them conditionaly in a function. 
struct OBJIndex
{
	unsigned int vertexIndex;
	unsigned int uvIndex;
	unsigned int normalIndex;

	bool operator<(const OBJIndex& _r) const { return vertexIndex < _r.vertexIndex; }
};

/// CLASS INDEXED_MODEL
///
/// This class is a container class for our different attributes of our model 
/// which has been indexed for better efficiency. We also have a function for
/// calculating our normals. 
class IndexedModel
{
public:
	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec2> m_texcoordinates;
	std::vector<glm::vec3> m_normals;
	std::vector<unsigned int> m_indices;

	void CalcNormals();
};

/// OBJ_LOADER
///
/// This class is used for containing our loaded in attributes found in an OBJ file  
/// such as vertex locations, uvs and normals. We also have functions to parse code 
/// from the obj file into indexes, vec2s or vec3 for use in our indexed model. We also
/// have a function for creating faces and find the last vertex index. 
class ObjLoader
{
public:
	ObjLoader(const std::string& _fileName);
	IndexedModel ToIndexedModel();

	std::vector<OBJIndex> m_OBJIndices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals;
    bool m_hasUVs;
    bool m_hasNormals;

private: 
	unsigned int FindLastVertexIndex(const std::vector<OBJIndex*>& _indexLookup, const OBJIndex* _currentIndex, const IndexedModel& _result);
    void CreateOBJFace(const std::string& _line);
	OBJIndex ParseOBJIndex(const std::string& _token, bool* _hasUVs, bool* _hasNormals);
    
    glm::vec2 ParseOBJVec2(const std::string& m_line);
    glm::vec3 ParseOBJVec3(const std::string& m_line);

	int m_faces;
    
};


