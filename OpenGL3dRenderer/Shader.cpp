#include "Shader.h"

/// CONSTRUCTOR 
///
/// For our shader's constructor, we take in the name of an external file which we'll need 
/// to pull our shaders from our resources folder. We then create our program and using our
/// array of array of GLuints we use our create and load shader functions to define our vertex shader
/// and our fragment shader. Afterwards we attach each of our shaders to our program using a simple for loop. 
/// 
/// For our vertices, uvs and normals, we associate them with our the positions, uvs and normals we load from our models. 
/// Then we link and validate our program to prevent it running if the code doesn't compile correctly. To do this we use two
/// functions which are commented later on in the cpp file. 
///
/// We then get the location of each of our uniforms in our program so we can assign their required values 
/// in update. We then simply assign the required values for our lights for use later. 
Shader::Shader(const std::string& filename)
{
	
	int m_numPointLights = 1;

	m_program = glCreateProgram();
	m_shaders[0] = CreateShader(LoadShader(filename + ".vs"),GL_VERTEX_SHADER);
	m_shaders[1] = CreateShader(LoadShader(filename + ".fs"),GL_FRAGMENT_SHADER);

	for(unsigned i = 0; i < NUM_SHADERS; i++)
	{
		glAttachShader(m_program, m_shaders[i]);
	}

	glBindAttribLocation(m_program, 0, "position");
	glBindAttribLocation(m_program, 1, "texCoord");
	glBindAttribLocation(m_program, 2, "normal");

	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "ERROR: Program linking failed  :");
	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "ERROR: Program is invalid  :");

	m_uniforms[TRANSFORM_U] = glGetUniformLocation(m_program,"transform");
	m_uniforms[MODEL_U] = glGetUniformLocation(m_program, "m");
	m_uniforms[VIEW_U] = glGetUniformLocation(m_program, "v");


	m_uniforms[L_COLOUR_U] = glGetUniformLocation(m_program, "gDirectionalLight.Base.Color");
	m_uniforms[L_AMBIENTINTENSITY_U] = glGetUniformLocation(m_program, "gDirectionalLight.Base.AmbientIntensity");
	m_uniforms[L_DIFFUSEINTENSITY_U] = glGetUniformLocation(m_program, "gDirectionalLight.Base.DiffuseIntensity");
	m_uniforms[L_DIRECTION_U] = glGetUniformLocation(m_program, "gDirectionalLight.Direction");

	m_uniforms[MAT_SPECULARINTENSITY_U] = glGetUniformLocation(m_program, "gMatSpecularIntensity");
	m_uniforms[MAT_SPECULARPOWER_U] = glGetUniformLocation(m_program, "gSpecularPower");
	m_uniforms[EYEPOS_U] = glGetUniformLocation(m_program, "gEyeWorldPos");
	m_numPointLights = glGetUniformLocation(m_program, "gNumPointLights");


	m_uniforms[PL_COLORS_U] = glGetUniformLocation(m_program,"PointLights[0].Base.Color");
	m_uniforms[PL_BASE_AMBIENTINTENSITYS_U] = glGetUniformLocation(m_program,"PointLights[0].Base.AmbientIntensity");
	m_uniforms[PL_BASE_DIFFUSE_U] = glGetUniformLocation(m_program,"PointLights[0].Base.DiffuseIntensity");
	m_uniforms[PL_POSITIONS_U] = glGetUniformLocation(m_program,"PointLights[0].Position");
	m_uniforms[PL_ATTEN_LINEAR_U] = glGetUniformLocation(m_program, "PointLights[0].Atten.Linear");
	m_uniforms[PL_ATTEN_CONSTANT_U] = glGetUniformLocation(m_program, "PointLights[0].Atten.Constant");
	m_uniforms[PL_ATTEN_EXP_U] = glGetUniformLocation(m_program,"PointLights[0].Atten.Exp");

	m_uniforms[SL_COLOR_U] = glGetUniformLocation(m_program, "SpotLights[0].Base.Base.Color");
	m_uniforms[SL_AMBIENTINTENSITY_U] = glGetUniformLocation(m_program, "SpotLights[0].Base.Base.AmbientIntensity");
	m_uniforms[SL_DIFFUSE_U] = glGetUniformLocation(m_program, "SpotLights[0].Base.Base.DiffuseIntensity");
	m_uniforms[SL_POSITION_U] = glGetUniformLocation(m_program, "SpotLights[0].Base.Position");
	m_uniforms[SL_DIRECTION_U] = glGetUniformLocation(m_program, "SpotLights[0].Direction");
	m_uniforms[SL_CUTOFF_U] = glGetUniformLocation(m_program, "SpotLights[0].Cutoff");
	m_uniforms[SL_ATTEN_CONSTANT_U] = glGetUniformLocation(m_program, "SpotLights[0].Base.Atten.Constant");
	m_uniforms[SL_ATTEN_LINEAR_U] = glGetUniformLocation(m_program, "SpotLights[0].Base.Atten.Linear");
	m_uniforms[SL_ATTEN_EXP_U] = glGetUniformLocation(m_program, "SpotLights[0].Base.Atten.Exp");
	
	m_light.color = glm::vec3(1.0,0.5,0.5);
	m_light.AmbientIntensity = 0.1f;
	m_light.Direction = glm::vec3(0.0,-1.5,-1.0);
	m_light.DiffuseIntensity = 0.2f;

	m_specintensity = 1.0f;
	m_specpower = 8;

	m_pointlight.AmbientIntensity = 0.1f;
	m_pointlight.color = glm::vec3(0.5f,0.5f, 1.0f);
	m_pointlight.DiffuseIntensity = 2.0f;
	m_pointlight.position = glm::vec3(0.0,0.0, -.75f);
	m_pointlight.Attenuation.Linear = 1.5f;

	m_spotlight.position = glm::vec3(0.0f,3.0f,0.0f);
	m_spotlight.AmbientIntensity = 0.1f;
	m_spotlight.DiffuseIntensity = 3.0f;
	m_spotlight.Attenuation.Linear = 1.0f;
	m_spotlight.color = glm::vec3(0.5f,1.0f,0.5f);
	m_spotlight.Cutoff = 60.0f;
	m_spotlight.Direction = glm::vec3(0.0f,-1.0f,0.0f);

}

/// DECONSTRUCTOR
///
/// This function detaches and then deletes each of our shaders from our program
/// and then once all shaders are deleted we delete the program from memory. 
Shader::~Shader()
{
	for(unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		glDetachShader(m_program,m_shaders[i]);
		glDeleteShader(m_shaders[i]);
	}

	glDeleteProgram(m_program);
}

/// BIND
///
/// This function simply tells opengl to 'use' this program 
/// as part of the current rendering state. In short, it tells
/// Opengl to install and 'use' this our member program defined earlier.
void Shader::Bind()
{
	glUseProgram(m_program);
}

/// LOADSHADER
///
/// For loading our shader we use ifstream to open our txt file and we 
/// iterate through everyline of our file in a while loop and appending 
/// the line to our output string. Once the file has ended, file.good will 
/// be false and therefore break out of the loop. Once we have our output, 
/// we simply return the output string. If file is not open, then something
/// has gone wrong and we report we are unable to load the shader. 
std::string Shader::LoadShader(const std::string& fileName) 
{ 
	std::ifstream file; 
	file.open((fileName).c_str()); 


	std::string output; 
	std::string line; 


	if(file.is_open()) 
	{ 
		while(file.good()) 
		{ 
			getline(file, line); 
			output.append(line + "\n"); 
		} 
	} 
	else 
	{ 
		std::cerr << "Unable to load shader: " << fileName << std::endl; 
	} 

	return output; 
} 

/// CHECKSHADERERROR
///
/// This function check if a shader has been compiled, linked or validated properly using a simple flag and if it isn't then 
/// we can specify an error message as a perameter. We create a GLuint called success and also a GLchar
/// array. If our isProgram parameter is true (or false), we check if the last link operation on the program was 
/// sucessful and assign it's value to success. Then we say if success is equal to GL_FALSE then check the we 
/// write the shader info log to our error array then subsequently print out our passed in error message with 
/// the relevant infolog providing more details on the shader error. 
void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage) 
{ 
	GLint success = 0; 
	GLchar error[1024] = { 0 }; 


	if(isProgram) 
	{
		glGetProgramiv(shader, flag, &success); 
	}
	else 
	{
		glGetShaderiv(shader, flag, &success); 
	}


	if(success == GL_FALSE) 
	{ 
		if(isProgram) 
		{
			glGetProgramInfoLog(shader, sizeof(error), NULL, error); 
		}
		else 
		{
			glGetShaderInfoLog(shader, sizeof(error), NULL, error); 
		}


		std::cerr << errorMessage << ": '" << error << "'" << std::endl; 
	} 
} 

/// CREATESHADER
///
/// This function creates a shader of the same type of what we pass in. So this 
/// could be a vertex or a fragment shader. Do a simple error check to be safe and 
/// then define a source string array to write our shader to. We then assign our source string
/// to our passed in shader text and also create a source string length variable and assign it 
/// to the length of our text string. Then we replace the source code of our shader object 
/// using the glShaderSource function. We then compile the shader and check for any error 
/// on it's compile status and then simply return shader. 
GLuint Shader::CreateShader(const std::string& text, GLenum shadertype) 
{ 
	GLuint shader = glCreateShader(shadertype); 

	if(shader == 0) 
	{
		std::cerr << "Error compiling shader type " << shadertype << std::endl; 
	}

	const GLchar* ShaderSourceStrings[1]; 
	ShaderSourceStrings[0] = text.c_str(); 
	GLint ShaderSourceStringsLengths[1]; 
	ShaderSourceStringsLengths[0] = text.length(); 

	glShaderSource(shader, 1, ShaderSourceStrings, ShaderSourceStringsLengths); 
	glCompileShader(shader); 


	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!"); 


	return shader; 
} 

/// UPDATE
/// 
/// In this function, we want to assign the value of all of our uniforms 
/// per frame for them to be of any use to us. So first of all, we need to 
/// assign our matrices using our transform and our camera class. We simply create
/// an identity matrix for m_model and use the position of our camera for our eyepos
/// uniform. M_transform is essentially our modelviewprojection matrix. Once we've assigned
/// our matrices, we 'use' our program and set our uniforms to the relevant variable for use 
/// in our shaders. Once we've assigned these variables, we also need to pass in our lights' variables.
/// I've defined functions for each of these operations to reduce the size of this function but the principle 
/// is the same for our lights. We use glUniform[datatype] and then they can be used in our shader. 
void Shader::Update(const Transform& transform, const Camera& camera)
{
	glm::mat4 m_transform = camera.getviewpojection() * transform.GetModel();
	glm::mat4 m_model = glm::mat4(1.0);
	glm::mat4 m_view = camera.getview();
	glm::vec3 m_eyepos = camera.getposition();

	glUseProgram(m_program);
	
	glUniformMatrix4fv(m_uniforms[TRANSFORM_U],1,GL_FALSE,&m_transform[0][0]);
	glUniformMatrix4fv(m_uniforms[MODEL_U],1,GL_FALSE,&m_model[0][0]);
	glUniformMatrix4fv(m_uniforms[VIEW_U],1,GL_FALSE,&m_view[0][0]);
	glUniform3f(m_uniforms[EYEPOS_U], m_eyepos.x, m_eyepos.y, m_eyepos.z);

	SetPointLights(m_pointlight);
	SetDirectionalLight(m_light);
	SetSpotLights(m_spotlight);
	SetMatSpecularIntensity(m_specintensity);
	SetMatSpecularPower(m_specpower);
}

/// SET_DIRECTIONAL_LIGHT
///
/// For this function we pass in a directional light and then set our 
/// desired uniform values to the relevant variable. So colour obviously takes in 
/// colour, ambient takes in ambient ect. 
void Shader::SetDirectionalLight(const DirectionalLight& _light)
{
	glUniform3f(m_uniforms[L_COLOUR_U], _light.color.x, _light.color.y, _light.color.z);
	glUniform1f(m_uniforms[L_AMBIENTINTENSITY_U], _light.AmbientIntensity);
	glUniform1f(m_uniforms[L_DIFFUSEINTENSITY_U], _light.DiffuseIntensity);
	glUniform3f(m_uniforms[L_DIRECTION_U], _light.Direction.x, _light.Direction.y, _light.Direction.z);
}

/// SET_MAT_SPECULAR_INTENSITY
///
/// Sets our specular intensity to the value of the passed in 
/// float value for use in our shaders. 
void Shader::SetMatSpecularIntensity(const float& Intensity)
{
	glUniform1f(m_uniforms[MAT_SPECULARINTENSITY_U], Intensity);
}

/// SET_MAT_SPECULAR_POWER
///
/// Sets our specular power to the value of our passed in 
/// 'power' variable for use in our shader. 
void Shader::SetMatSpecularPower(const float& power)
{
	glUniform1f(m_uniforms[MAT_SPECULARPOWER_U], power);
}

/// SET_POINT_LIGHTS
///
/// Same as the above, passing in our pointlight variables 
/// into our uniforms for use in our shaders. 
void Shader::SetPointLights(const PointLight& pLight)
{
	glUniform3f(m_uniforms[PL_COLORS_U], pLight.color.x, pLight.color.y, pLight.color.z);
	glUniform1f(m_uniforms[PL_BASE_AMBIENTINTENSITYS_U], pLight.AmbientIntensity);
	glUniform1f(m_uniforms[PL_BASE_DIFFUSE_U], pLight.DiffuseIntensity);
	glUniform3f(m_uniforms[PL_POSITIONS_U], pLight.position.x, pLight.position.y, pLight.position.z);
	glUniform1f(m_uniforms[PL_ATTEN_CONSTANT_U], pLight.Attenuation.Constant);
	glUniform1f(m_uniforms[PL_ATTEN_LINEAR_U], pLight.Attenuation.Linear);
    glUniform1f(m_uniforms[PL_ATTEN_EXP_U], pLight.Attenuation.Exp);
    
}

/// SET_SPOT_LIGHTS
///
/// Same as the above, passing in our spotlight variables 
/// into our uniforms for use in our shaders. 
void Shader::SetSpotLights(const SpotLight& sLight)
{
	glUniform3f(m_uniforms[SL_COLOR_U], sLight.color.x, sLight.color.y, sLight.color.z);
	glUniform3f(m_uniforms[SL_POSITION_U], sLight.position.x, sLight.position.y, sLight.position.z);
	glm::vec3 Direction = sLight.Direction;
	glm::normalize(Direction);
	glUniform3f(m_uniforms[SL_DIRECTION_U], sLight.Direction.x, sLight.Direction.y, sLight.Direction.z);
	glUniform1f(m_uniforms[SL_DIFFUSE_U], sLight.DiffuseIntensity);
	glUniform1f(m_uniforms[SL_AMBIENTINTENSITY_U], sLight.AmbientIntensity);
	glUniform1f(m_uniforms[SL_ATTEN_CONSTANT_U], sLight.Attenuation.Constant);
	glUniform1f(m_uniforms[SL_ATTEN_EXP_U], sLight.Attenuation.Exp);
	glUniform1f(m_uniforms[SL_ATTEN_LINEAR_U], sLight.Attenuation.Linear);
	glUniform1f(m_uniforms[SL_CUTOFF_U], cosf(glm::radians(sLight.Cutoff)));
}

