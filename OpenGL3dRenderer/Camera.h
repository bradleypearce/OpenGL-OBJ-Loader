#include "Headers.h"

/// CLASS CAMERA
///
/// This class is simply where our 'eyes' in our 3D world are going to be. For this class we set up our  
/// projection matrices so the camera sees the objects in 3D. Here we also have functions to get the viewprojection 
/// matrix. We also need to take in a field of view (70 is a good approximation of the human eye), what ascept ratio 
/// our screen is along with the near and far values we need for our projection matrix. Other than the constructor, 
/// these functions are mostly getters for our view and viewprojection matrices. These help in our shader class as
/// we need to get the model view projection matrix for our vertex and fragment shaders. 
class Camera
{
public:
	Camera(glm::vec3 pos, float fov, float aspect,float z_near, float z_far)
	{
		m_projection = glm::perspective(fov,aspect,z_near,z_far);
		m_position = pos;
		m_forward = glm::vec3(0.0,0.0,-1.0);
		m_up = glm::vec3(0.0,1.0,0.0);

	}

	inline glm::mat4 getviewpojection() const
	{          //Projecton     //View 
		return m_projection * glm::lookAt(m_position,m_position + m_forward, m_up);
	}

	inline glm::mat4 getview() const
	{
		return glm::lookAt(m_position, m_position + m_forward, m_up);
	}

	inline glm::vec3 getposition() const
	{
		return m_position;
	}

private:
	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::mat4 m_projection;
};