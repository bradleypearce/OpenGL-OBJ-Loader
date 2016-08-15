/// HEADERS
///
/// This header file is used to contain all of the libraries and header files  
/// for our project. I chose to store them here as keeping track of our libraries and
/// headers can be messy and lead to many errors whilst developing. I found it more efficient
/// to contain my headers in a simple header file to avoid such errors and increase productivity.

#ifndef __HEADERS__ 

#define __HEADERS__

#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <fstream>
#include <SDL2\SDL.h>
#include <cassert>
#include <algorithm>
#include <map>
#include <math.h>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "GLEW\GL\glew.h"
#include "glm\glm.hpp"
#include "Display.h"
#include "ObjLoader.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
#include "stb_image.h"
#include "Shader.h"
#include "Texture.h"

#endif