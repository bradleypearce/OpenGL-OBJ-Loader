#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 TexCoord0; 
out vec3 Normal0;
out vec3 WorldPos0;

uniform mat4 transform;
uniform mat4 m;

/// This vector sets the position of our vertices by multiplying it 
/// by our model view projection matrix. We then want to pass our texture 
/// coordinates along to our fragment shader so we define an out variable 
/// called texCoord0 and assign it the value of our attributes texture coordinate.
/// We then send our out normal in the same way, however changing it into a
/// world space vector and then doing the same with our vertex position. Providing these
/// in world space will come useful in our fragment shader when considering lighting 
/// calculations. 
void main()
{
  	gl_Position = transform * vec4(position, 1.0);

 	TexCoord0 = texCoord;

 	Normal0 = (m * vec4(normal, 0.0)).xyz;

  	WorldPos0 = (m * vec4(position, 1.0)).xyz;
 } 
