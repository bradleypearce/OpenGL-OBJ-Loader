#include "Texture.h"

/// Constructor 
///
/// For our constructor we take in the name of the file you want to load,
/// in this case the name of our texture. We use stbi to load our texture 
/// and then check if our imagedata is valid, if not we report it in the console. 
/// Then afterwards we bind our texture for use in our fragment shader. We also tell
/// the texture to wrap around the model's UVs using glTexParameteri and make use of 
/// mipmapping as well. After that we send the texture to our GPU and then free the imagedata
/// so we don't get memory leaks. 
Texture::Texture(const std::string& filename)
{
	int width, height, numComponents;
	unsigned char* imagedata = stbi_load(filename.c_str(), &width, &height, &numComponents, 4);

	if(imagedata == NULL)
	{
		std::cerr << "Texture loading failed:" << filename << std::endl; 
	}

	glGenTextures(1,&m_texture);
	glBindTexture(GL_TEXTURE_2D,m_texture);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagedata);

	stbi_image_free(imagedata);
}

/// Deconstructor 
///
/// Simply makes use of the delete texture function and deletes our texture. 
Texture::~Texture()
{
	glDeleteTextures(1,&m_texture);
}
