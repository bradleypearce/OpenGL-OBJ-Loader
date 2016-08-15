#pragma once

#include "headers.h"

/// STRUCT BASELIGHT 
///
/// The BaseLight class is a container for variables that all types of  
/// light share. This includes our base colour, our ambient intensity and 
/// diffuse intensity. We use a constructor to initialise these variables 
/// to default values. 
struct BaseLight
{
	glm::vec3 color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight()
    {
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        AmbientIntensity = 0.2f;
        DiffuseIntensity = 0.5f;
    }
};

/// STRUCT DIRECTIONALLIGHT
///
/// For this type of light, other than our BaseLight variables which 
/// this struct derives from, we add a direction as a directional light
/// illuminates infinitely in a single direction. We initialise this to 
/// nothing so it always a default value. 
struct DirectionalLight : public BaseLight
{
	glm::vec3 Direction;

	DirectionalLight()
    {
		Direction = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};

/// STRUCT POINTLIGHT
///
/// For the point light, we define a position which represents where our 
/// point light will illuminate from in 3D space. We also require a new 
/// struct for our attenuation values. These values represent how quickly
/// the light dims from the source position. There are three different types of
/// attenuation (for the sake of our renderer), constant (illuminates constantly 
/// within fading), linear (which drops off based on the pixel's "distance" from the 
/// source position and exponential (which drops off by distance squared). 
struct PointLight : public BaseLight
{
	glm::vec3 position;

	struct
	{
		float Constant;
		float Linear;
		float Exp;
	} Attenuation;

	PointLight()
    {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        Attenuation.Constant = 1.0f;
        Attenuation.Linear = 0.0f;
        Attenuation.Exp = 0.0f;
    }
};

/// STRUCT SPOTLIGHT
///
/// A spotlight works similar to how a pointlight works so we inherit from pointlight 
/// to make this struct as simple of possible. For our spot light, we have a direction
/// it will illuminate in 3D space and a cutoff which represents the angle it illuminates 
/// at (how wide the spotlight is). We initialise these to 0 within the struct. 
struct SpotLight : public PointLight
{
    glm::vec3 Direction;
    float Cutoff;

    SpotLight()
    {
		Direction = glm::vec3(0.0f, 0.0f, 0.0f);
        Cutoff = 0.0f;
    }
};

/// CLASS SHADER 
///
/// Our shader class is for creating, loading and updating our shaders. Shaders in OpenGL 
/// are compiled at runtime on the GPU and, in this case, will read a text file containing 
/// our code. We also use this class to pass in our uniforms for use in our vertex and fragment 
/// shader. For each of our uniforms, we have a representative enum and GLuint as an array of uniforms.  
/// To coincide with our lights, we have 2 material values for our specular intensity and power. 
/// The shader class was mostly written by TheBennyBox Source: https://github.com/BennyQBD/ModernOpenGLTutorial
/// The reason i chose to use this code is because it reports errors to the console window and has been 
/// invaluable for debugging and working with shaders during development. It is also very rigid and simple 
/// to use.
class Shader
{
public:
	static const unsigned int MAX_POINT_LIGHTS = 1;

	Shader(const std::string& filename);

	void Bind();
	void Update(const Transform& transform, const Camera& camera);
	static std::string LoadShader(const std::string& fileName);
	static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	static GLuint CreateShader(const std::string& text, GLenum shadertype);

	void SetDirectionalLight(const DirectionalLight& _light);
	void SetPointLights(const PointLight& pLights);
	void SetSpotLights(const SpotLight& sLight);
	void SetMatSpecularIntensity(const float& Intensity);
	void SetMatSpecularPower(const float& Power);

	virtual ~Shader();

private:
	static const unsigned int NUM_SHADERS = 2;

	enum
	{
		TRANSFORM_U,

		MODEL_U,

		VIEW_U,

		L_COLOUR_U,

		L_AMBIENTINTENSITY_U,

		L_DIFFUSEINTENSITY_U,

		L_DIRECTION_U,

		PL_COLORS_U,

		PL_BASE_AMBIENTINTENSITYS_U,

		PL_BASE_DIFFUSE_U,

		PL_POSITIONS_U,

		PL_ATTEN_CONSTANT_U,

		PL_ATTEN_LINEAR_U,

		PL_ATTEN_EXP_U,

		SL_COLOR_U,

		SL_AMBIENTINTENSITY_U,

		SL_DIFFUSE_U,

		SL_POSITION_U,

		SL_DIRECTION_U,

		SL_CUTOFF_U,

		SL_ATTEN_CONSTANT_U,

		SL_ATTEN_LINEAR_U,

		SL_ATTEN_EXP_U,

		MAT_SPECULARINTENSITY_U,

		MAT_SPECULARPOWER_U,

		EYEPOS_U,

		NUM_UNIFORMS
	};

	GLuint m_program;
	GLuint m_shaders[NUM_SHADERS];
	GLuint m_uniforms[NUM_UNIFORMS];
	PointLight m_pointlight;
	DirectionalLight m_light;
	SpotLight m_spotlight;
	
	float m_specintensity;
	float m_specpower;
};


