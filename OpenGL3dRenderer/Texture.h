#pragma once

#include "Headers.h"

/// CLASS TEXTURE 
///
/// The texture class is simply a class to load and pipe textures to our GPU
/// for use in our fragment shader. All we need is a simple GLuint which we've named 
/// m_texture and then use STB Image to load our texture as quickly and simply as possible. 
class Texture
{
public:
	Texture(const std::string& filename);

	~Texture();
private:
	GLuint m_texture;
};

