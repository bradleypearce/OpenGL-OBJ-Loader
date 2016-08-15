#include "ObjLoader.h"

/// Predefining static functions for use in this cpp file. 
static bool CompareOBJIndexPtr(const OBJIndex* _a, const OBJIndex* _b);
static inline unsigned int FindNextChar(unsigned int _start, const char* _str, unsigned int _length, char _token);
static inline unsigned int ParseOBJIndexValue(const std::string& _token, unsigned int _start, unsigned int _end);
static inline float ParseOBJFloatValue(const std::string& _token, unsigned int _start, unsigned int _end);
static inline std::vector<std::string> SplitString(const std::string &_s, char _delim);

/// CONSTRUCTOR
///
/// For our constructor, we take in our file name of the OBJ we want to load. We then 
/// set our faces variable to 0 and our hasuvs and hasnormals bool to false, which will 
/// be used later to change the behaviour based on if our OBJ has normals or not. We then 
/// want to use ifstream to read our file. So we define our ifstream object as our file 
/// and use file to open our passed in file. We then define a string to represent each line 
/// in our OBJ file. We then only execute the next part if our file is open, otherwise we report 
/// an error loading the obj. We then for safety check if the file has no errors when reading (or if
/// the file has ended). We then get 
///
///
ObjLoader::ObjLoader(const std::string& _fileName)
{
	m_faces = 0;
	m_hasUVs = false;
	m_hasNormals = false;
	
	std::ifstream file;

	file.open(_fileName.c_str());

	std::string line;
	if(file.is_open())
	{
		while(file.good())
		{
			getline(file, line);

			unsigned int linelength = line.length();

			if(linelength < 2)
			{
				continue;
			}

			const char* lineCStr = line.c_str();

			switch(lineCStr[0])
			{
			case 'v':
				if(lineCStr[1] == 't')
				{
					this->m_uvs.push_back(ParseOBJVec2(line));
				}
				else if(lineCStr[1] == 'n')
				{
					this->m_normals.push_back(ParseOBJVec3(line));
				}
                else if(lineCStr[1] == ' ' || lineCStr[1] == '\t')
				{
                        this->m_vertices.push_back(ParseOBJVec3(line));
				}

				break;

			case 'f': 
				CreateOBJFace(line);
				break;

			default: break;
			};
		}

		std::cout << "The amount of vertices" << _fileName << " has is:" << m_vertices.size() << std::endl;
		std::cout << "The amount of faces" << _fileName <<" has is:" << m_faces << std::endl;
	}
	else
	{
		std::cerr << "Unable to load mesh: " << _fileName << std::endl;
	}
}

/// CALC_NORMALS 
///
/// This function is used to calculate normals if they are not provided with our 
/// OBJ. We essentially use our indices vector in a for loop and add 3 every loop 
/// to i. We create i0, i1 and i2 and also create v1 and v2 (a vector to represent
/// i1 - 0 (v1) and i2 - i0 (v2)) We then calculate our normal based on the cross 
/// product of v1 and v2 (normalised). Giving us a normal of two vertices (minus i0). 
/// We then add normal to our normals vector at index i0, i1 and i2. We then repeat the 
/// the process untill i is more than or equal to the size of our indices vector. Once this 
/// is done, we define another loop which loops reletive to the size of our positions and 
/// we normalise each of our normals contained in our normals vector (at the index of i) giving 
/// us normals for our models even if our OBJ doesn't have normals. 
void IndexedModel::CalcNormals()
{
	for(unsigned int i = 0; i < m_indices.size(); i += 3)
    {
        int i0 = m_indices[i];
        int i1 = m_indices[i + 1];
        int i2 = m_indices[i + 2];

        glm::vec3 v1 = m_positions[i1] - m_positions[i0];
        glm::vec3 v2 = m_positions[i2] - m_positions[i0];
        
        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
            
        m_normals[i0] += normal;
        m_normals[i1] += normal;
        m_normals[i2] += normal;
	}
	
	for(unsigned int i = 0; i < m_positions.size(); i++)
	{
        m_normals[i] = glm::normalize(m_normals[i]);
	}
}

/// TO_INDEXED_MODEL 

/// This function turns all of our data into an indexed model for use in our Mesh class. This data will 
/// what we will pump to our GPU. So we create a result indexed model and we also create a seperate indexed model 
/// for our normal model. We create a variable to represent the size of our indices and create a vector of OBJindexes 
/// for use in looking up our indices. we then push all of our indices to our index look up vector. We use our std::sort 
/// to sort our indices from the beginning of our indices vector to the end in order of our index's value using our compareobjindexptr 
/// function. We then use a map container to pair our indices with an int value for our normalmodel's indices and another for our result 
/// model. Then using a for loop (looping to the size of num indices). We use i as an index of our objindices and assign this to our currentindex 
/// pointer. We then create a currentposition vector which we assign to our current index's vertex index. We create a currenttexture coordinate and 
/// a current normal vector. We only assign these to our current index's uv index and normal index if our model has uvs and normals respectively, using
/// our has normals and has uvs variables. If these are not true, we set the current uv index and normal index to zero vectors. We then create two int values
/// for our normal and result model indexes for use in our map containers. We then define an interator which finds our current index in our normal index map. 
/// We then check if our iterator is equal to the end of our normal model map container and if so, we set our normalmodelindex to the size of our positions 
/// vector. We insert our current index along with our normal model's index as pair for our map container. Then simply push our current position, uv and normal 
/// to our normal model relevant container. If our iterator is not equal to the end of our map, we set our normal model's index to our current index's second 
/// (the current index's paired index). We the find our previous vertex by using our 'find last vertex' function using our index look up, our current index and result
/// as parameters. Then we check if our previous vertex is equal to minus 1, we then set our result model index to the size of our result's positions vector and add our 
/// current position, uv and normal to our relevant containers. If it's not minus 1 we set our result model index to the value of our previous vertex location. We then add 
/// our normalmodelindex to our normal model's indices vector, add our resultmodelindex to our result model's indices vector and insert our result model's index alomg with our 
/// normal model index as a pair in our regular model's index map. We repeat this until i is equal to the amount of indices in our obj loader's indices vector. The map containers 
/// are only used if our model doesn't have normals. If our model doesn't have normals, we calculate our normals using our calc normals function, then use of for loop the size of our 
/// result model's positions vector and add our normalmodel's normals vector at the index of our index map. Once this is done, we return our result model which has all of our positions, 
/// textures, normals and our index values ready to be piped to the GPU from our mesh class. 
IndexedModel ObjLoader::ToIndexedModel()
{
	IndexedModel result;
	IndexedModel normalModel;

	unsigned int numindicies = m_OBJIndices.size();

	std::vector<OBJIndex*> indexlookup;

	for(unsigned int i = 0; i < numindicies; i++)
	{
		indexlookup.push_back(&m_OBJIndices[i]);
	}

	std::sort(indexlookup.begin(), indexlookup.end(), CompareOBJIndexPtr);
    
    std::map<OBJIndex, unsigned int> normalModelIndexMap;
    std::map<unsigned int, unsigned int> indexMap;
    
    for(unsigned int i = 0; i < numindicies; i++)
    {
        OBJIndex* currentIndex = &m_OBJIndices[i];
        
        glm::vec3 currentPosition = m_vertices[currentIndex->vertexIndex];
        glm::vec2 currentTexCoord;
        glm::vec3 currentNormal;
        
        if(m_hasUVs)
		{
            currentTexCoord = m_uvs[currentIndex->uvIndex];
		}
        else
		{
			currentTexCoord = glm::vec2(0,0);
		}
            
        if(m_hasNormals)
		{
            currentNormal = m_normals[currentIndex->normalIndex];
		}
        else
		{
            currentNormal = glm::vec3(0,0,0);
		}
        
        unsigned int normalModelIndex;
        unsigned int resultModelIndex;
        
        std::map<OBJIndex, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
        if(it == normalModelIndexMap.end())
        {
            normalModelIndex = normalModel.m_positions.size();
        
            normalModelIndexMap.insert(std::pair<OBJIndex, unsigned int>(*currentIndex, normalModelIndex));
            normalModel.m_positions.push_back(currentPosition);
			normalModel.m_texcoordinates.push_back(currentTexCoord);
            normalModel.m_normals.push_back(currentNormal);
        }
        else
		{
            normalModelIndex = it->second;
		}
        
        unsigned int previousVertexLocation = FindLastVertexIndex(indexlookup, currentIndex, result);
        
        if(previousVertexLocation == (unsigned int)-1)
        {
            resultModelIndex = result.m_positions.size();
        
            result.m_positions.push_back(currentPosition);
            result.m_texcoordinates.push_back(currentTexCoord);
            result.m_normals.push_back(currentNormal);
        }
        else
		{
            resultModelIndex = previousVertexLocation;
		}
        
        normalModel.m_indices.push_back(normalModelIndex);
        result.m_indices.push_back(resultModelIndex);
        indexMap.insert(std::pair<unsigned int, unsigned int>(resultModelIndex, normalModelIndex));
    }
    
	if(!m_hasNormals)
    {
        normalModel.CalcNormals();
        
        for(unsigned int i = 0; i < result.m_positions.size(); i++)
		{
            result.m_normals[i] = normalModel.m_normals[indexMap[i]];
		}
    }
    
    return result;
}

/// FIND_LAST_VERTEX_INDEX 
///
/// For this function, we take in an vector of ObjIndices, our current index (of which we'll use to find the previous vertex index) and the 
/// the result model we want the function to be reletive to. We use a start variable and set it to zero. An end variable which we assign to the 
/// size of our index look up vector's size. We need a current value which we assign based on end minus start (first) and then divide this result by two
/// and add start. We also require a previous variable which we assign to the value of start. Then we loop on the condition that current is not equal to 
/// previous (so it will end when out of range unless it returns before the loop ends). We then define an obj index which we use to test against our parameter.
/// We assign our test index to the value of our index look up vector (at current). We then test if our vertex index is equal to our current index's vertex index. 
/// If true, we create a count start variable which we assign to current. Then create a for loop which iterates i every loop until i is equal to current. Within this 
/// loop, we create an possible index which is equal to our index look up container (at current - i) and then compare the possible index to our current index. If true we
/// continue the loop and if our possible index's vertex index is not equal to our current index's vertex index then we break out of the loop. Otherwise, we take one away from
/// count start. So we break out of the loop on if our vertex index is not equal to our current index's vertex index or if i exceeds our current variable. We then create a loop just 
/// after which loops reletive to countstart's value and ends when i is equal to the size of our index look up vector minus countstart. We then define another possible index variable 
/// and assign this to current plus i, and compare the possible index every loop with our current index, if it's equal, continue and if the possible index's vertex index is not equal to our 
/// current's vertex index we break out of the loop. If it's equal we then check if: 
///
///														1. It doesn't have UVs OR 
///														our possible index's uv index is equal to our current idex's uv index. 
///														AND
///														2. It doesn't have normals OR 
///														our possible index's norma index is equal to our current index's normal index. 
///
/// If both of either of these or comparisons return true, we create a vector for each of our 'current' attributes (positions, uvs, normals ect.) and assign our currentposition to our 
/// current index's vertex index. Then, if our model has UVS, we set our current uv to our current index's uv index, else we set the vector to zero. We do the very same for our current normal vector
/// Then once again we create a for loop within the current for loop, which iterates the result model positions vector's size and uses J as an iterator. Within this loop, we test if our currentposition 
/// is both eqaul to our result's positions at j AND if either the model has no uvs or our current texture coordinate is equal to our result model's uv's container (at index j) and either our model has no normals
/// or our currentnormal is equal to our results normals (at index j). If these are all true, we return J as this is our 'last' or previous vertex index. If our test index's vertex index is not equal to our current 
/// index's vertex index then we test if our test index's vertex index then we test if our test index's vertex index is less than our current index's vertex index and if so, we set start to current or if it's more 
/// than we set end equal to current. This narrows our range and then assigns our current and previous the same way we do when we instantiated them. If we happen to break out of the while or for loop the function 
/// will return minus 1 indicating it cannot find the previous vertex. 
unsigned int ObjLoader::FindLastVertexIndex(const std::vector<OBJIndex*>& _indexLookup, const OBJIndex* _currentIndex, const IndexedModel& _result)
{
	unsigned int start = 0;
	unsigned int end = _indexLookup.size();
    unsigned int current = (end - start) / 2 + start;
    unsigned int previous = start;
    
    while(current != previous)
    {
        OBJIndex* testIndex = _indexLookup[current];
        
        if(testIndex->vertexIndex == _currentIndex->vertexIndex)
        {
            unsigned int countStart = current;
        
            for(unsigned int i = 0; i < current; i++)
            {
                OBJIndex* possibleIndex = _indexLookup[current - i];
                
                if(possibleIndex == _currentIndex)
				{
                    continue;
				}
                    
                if(possibleIndex->vertexIndex != _currentIndex->vertexIndex)
				{
                    break;
				}   

                countStart--;
            }
            
            for(unsigned int i = countStart; i < _indexLookup.size() - countStart; i++)
            {
                OBJIndex* possibleIndex = _indexLookup[current + i];
                
                if(possibleIndex == _currentIndex)
				{
                    continue;
				}
                    
                if(possibleIndex->vertexIndex != _currentIndex->vertexIndex)
				{
                    break;
				}

                else if((!m_hasUVs || possibleIndex->uvIndex == _currentIndex->uvIndex) && (!m_hasNormals || possibleIndex->normalIndex == _currentIndex->normalIndex))
                {
                    glm::vec3 currentPosition = m_vertices[_currentIndex->vertexIndex];
                    glm::vec2 currentTexCoord;
                    glm::vec3 currentNormal;
                    
                    if(m_hasUVs)
					{
                        currentTexCoord = m_uvs[_currentIndex->uvIndex];
					}
                    else
					{
                        currentTexCoord = glm::vec2(0,0);
					}
                        
                    if(m_hasNormals)
					{
                        currentNormal = m_normals[_currentIndex->normalIndex];
					}
                    else
					{
                        currentNormal = glm::vec3(0,0,0);
					}
                    
                    for(unsigned int j = 0; j < _result.m_positions.size(); j++)
                    {
                        if(currentPosition == _result.m_positions[j] && ((!m_hasUVs || currentTexCoord == _result.m_texcoordinates[j]) && (!m_hasNormals || currentNormal == _result.m_normals[j])))
                        {
                            return j;
                        }
                    }
                }
            }
        
            return -1;
        }
        else
        {
            if(testIndex->vertexIndex < _currentIndex->vertexIndex)
			{
                start = current;
			}
            else
			{
                end = current;
			}
        }
    
        previous = current;
        current = (end - start) / 2 + start;
    }
    
    return -1;
}

/// CREATE_OBJ_FACE
///
/// This function essentially creates our faces by adding to our indices vector. 
/// We take in the line we want to modify and split our string into a vector of strings
/// by using a ' ' to seperate them. We then add to our indices vector by parsing our 
/// indices, assuming the model is made up of tris, we simply use an index value of 1, 2 
/// and 3 (0 would be 'f' so we don't need this). If our model is made up of squares, we check
/// if our token string's size is more than 4 and if so, we only use 1, 3 and 4 to make up our triangle 
/// and parse our index in the same way resulting in a triangulated model. Once this indices have been added
/// we add one to faces for our faces count. 
void ObjLoader::CreateOBJFace(const std::string& _line)
{
	std::vector<std::string> tokens = SplitString(_line, ' ');

    this->m_OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->m_hasUVs, &this->m_hasNormals));
    this->m_OBJIndices.push_back(ParseOBJIndex(tokens[2], &this->m_hasUVs, &this->m_hasNormals));
    this->m_OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->m_hasUVs, &this->m_hasNormals));

    if((int)tokens.size() > 4)
    {
        this->m_OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->m_hasUVs, &this->m_hasNormals));
        this->m_OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->m_hasUVs, &this->m_hasNormals));
        this->m_OBJIndices.push_back(ParseOBJIndex(tokens[4], &this->m_hasUVs, &this->m_hasNormals));
    }

	m_faces++;
}

/// PARSE_OBJ_INDEX 
///
/// This function is for parsing our indices contained in our OBJ file. This is necessary for our 
/// faces. we take in our string we want to parse, along with this, we take in the value of has uvs 
/// and has normals so we can modify them if we find the model does have uvs and normals. We first of all create 
/// variables to represent the length of our string and another to represent our string as a cstring. We define a 
/// index start and end variable. We set start as 0 naturally and then end is defined based on the result of FindNextchar 
/// which is seeking a '/' (how indices are mostly represented in OBJ files). We then create a result OBJindex which we will 
/// return once we've parsed our indices. We then add our index value to our vertex index using our parseobjindexvalue function. 
/// We pass in our index start and end as parameters to get the correct result. We initially set our uvindex and normal index to 0. 
/// We then check if our indexend is more than or equal to the length of our string and if so, we return the resulting objindex, if 
/// not we carry on and set our index start to end + 1 to carry on along our string. Then using find next char we find the next char 
/// which will be our index end (again). Then we parse our obj index value and set has uvs to true, the function would not continue if
/// the model doesn't have uvs as our index end would be equal to the end of our token string. We then repeat the same process for our 
/// normals. Obviously if the model doesn't have normals, our parameter will not be set to true and our function will return result. 
/// We don't need to parse any more of our string as we only want uvs and normals from our indices so after normals we simply return 
/// result. 
OBJIndex ObjLoader::ParseOBJIndex(const std::string& _token, bool* _hasUVs, bool* _hasNormals)
{
	unsigned int tokenLength = _token.length();
    const char* tokenString = _token.c_str();
    
    unsigned int vertIndexStart = 0;
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    OBJIndex result;
    result.vertexIndex = ParseOBJIndexValue(_token, vertIndexStart, vertIndexEnd);
    result.uvIndex = 0;
    result.normalIndex = 0;
    
    if(vertIndexEnd >= tokenLength)
        return result;
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    result.uvIndex = ParseOBJIndexValue(_token, vertIndexStart, vertIndexEnd);
	*_hasUVs = true;
    
    if(vertIndexEnd >= tokenLength)
        return result;
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    result.normalIndex = ParseOBJIndexValue(_token, vertIndexStart, vertIndexEnd);
	*_hasNormals = true;
    
    return result;
}

/// PARSE_OBJ_VEC3 
///
/// For this function we take in a line from our obj file and then create variables  
/// to represent the length of our passed in string and a char* to convert our string 
/// to a c string. Then we create a startindex equal to 2 (given that positions and normals 
/// a presented as a V and an N with a space afterwards). We then start a loop which ends when 
/// our start value equal to or more than the length of our string. Within this loop, if at the location
/// of index start in our string is not equal to ' ' we break out of the loop so we dont read in spaces. 
/// We then use our IndexEnd to find the next space in our line by using our find next char function. We then  
/// assign our x variable by parsing the float value using our start and end indexes we created earlier. We then 
/// reassign start to end + 1 to avoid whitespaces and find the next char in our line using our Findnextchar function. 
/// For y and z we repeat the process until we have the three elements of our vector. Then we simply return a vector3 containing
/// x, y and z. 
glm::vec3 ObjLoader::ParseOBJVec3(const std::string& line) 
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 2;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float z = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    return glm::vec3(x,y,z);
}

/// PARSE_OBJ_VEC2
///
/// For this function, we take in a line from our OBJ file, create two variables to store
/// the length of the string passed in and the string converted to a cstring. We then create
/// a start index which is 3 (due to our uvs starting with "vt " which is three spaces along).
/// We then start a loop which stops when our indexstart value is more than or equal to the length
/// of our string. Within this loop, if the character at index "indexstart" of our string is not equal
/// to ' ' then we break out of the loop as we now have our values we want to parse. We create an index end
/// and use our findnextchar function to find the end of the value we want to read by checking for a ' '. 
/// The we define our x value which we parse using our ParseOBJFloatValue function to get the value we want to store. 
/// Then we set start as end + 1 as in OBJ files coordinate elements are seperated by spaces. We then set the end 
/// using findnextchar and then read in the value of y from our line in the same way we did x. Then we return a 2D vector
/// with x and y obviously being passed as the corresponding x and y. 
glm::vec2 ObjLoader::ParseOBJVec2(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 3;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
		{
            break;
		}
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    return glm::vec2(x,y);
}

/// COMPARE_OBJ_INDEX_PTR
///
/// This functions takes in two obj indexes and returns the value
/// of a < b, so it will return either a true or false value
static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b)
{
    return a->vertexIndex < b->vertexIndex;
}

/// FIND_NEXT_CHAR
///
/// This function takes in the a start value, a string, the length of the string and a token char
/// which acts as stop, in most cases the token would be ' ' (a white space). We start by setting result
/// as our passed in start value, and then whilst result is less than length, we add to result. Then, use 
/// result as an index to our passed in string and check if it's equal to token, if it is, we break out of the loop
/// and result result. Thus giving us the next char 
static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
{
    unsigned int result = start;
    while(result < length)
    {
        result++;
        if(str[result] == token)
            break;
    }
    
    return result;
}

/// PARSE_OBJ_INDEX_VALUE
///
/// For this function we take in a token string and we set a start and end value. 
/// Then we simply return a substring, we enter our start value as the first character to be copied
/// and we use end to take away from start, giving us the length of our substring. We then convert the resulting 
/// c string to an integer and take away 1 to get the correct index value. 
static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atoi(token.substr(start, end - start).c_str()) - 1;
}

/// PARSE_OBJ_FLOAT_VALUE
///
/// As with the above, we take in our token string, a start value and an end value
/// and use our start value to mark where our substring starts and minus start from end
/// to get the size of our substring. We then convert this substring to a float value. 
/// We need to cast it as a float to avoid getting a double value. 
static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end)
{
    return (float)atof(token.substr(start, end - start).c_str());
}

/// SPLIT_STRING
///
/// For this function, we take in the string we want to split and delimiter (something we don't want to take in).
/// We first of all create a vector of strings we call elems. We then convert our passed in string to a cstring and store
/// it as a char*. We then get the length of the string and create a variable for this also. We then define a start and end 
/// int for us in our loop. Then, we start a loop which only ends when end is less than or equal to the length of our passed
/// in string. Inside this loop, we have another loop with the same condition which adds one to end each inner loop unless position end
/// of our string is equal to our delimiter. If it is, we break out of our inner loop, add a substring which starts at position start 
/// and adds (end - start) amount of characters. Then, to make sure we keep reading in the right values, we assign start to end + 1 and 
/// then assign end to the value of start to start the cycle again. Once we have exceeded the length of the passed in string we simply return
/// our elements vector. 
static inline std::vector<std::string> SplitString(const std::string &s, char delim)
{
    std::vector<std::string> elems;
        
    const char* cstr = s.c_str();
    unsigned int strLength = s.length();
    unsigned int start = 0;
    unsigned int end = 0;
        
    while(end <= strLength)
    {
        while(end <= strLength)
        {
            if(cstr[end] == delim)
                break;
            end++;
        }
            
        elems.push_back(s.substr(start, end - start));
        start = end + 1;
        end = start;
    }
        
    return elems;
}
