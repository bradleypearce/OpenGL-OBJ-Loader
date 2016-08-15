#include "Headers.h"

/// MAIN
///
/// Our main function begins by creating all the objects needed for our OBjLoader, so a display,
/// camera, a transform, our shaders, texture and of course our model(s). The main loop afterwards 
/// simply continues untill the window is closed. Within our main loop, we clear the screen, rotate 
/// our models using our transform, bind and update our shaders and then simply draw our mesh(s) and 
/// update our display. We also increment counter so our transform spins our models. Once the window is closed
/// the program exits. 
int main(int argc, char** argv)
{
	Display display(1920,1080, "3D Renderer");

	Transform transform;

	Camera camera(glm::vec3(0.0f,0.0f,5.0f), 70.0f,((1280.0f)/(720.0f)),0.01f,1000.0f);

	Shader shader("./res/basicShader");

	Texture texture("./res/bricks.jpg");

	Mesh mesh("./res/person.obj");

	Mesh mesh2("./res/monkey3.obj");
	
	Mesh mesh3("./res/face.obj");

	float counter = 0.0f;

	while(!display.IsClosed())
	{
		display.Clear(0.0f,0.0f,0.0f,1.0f);

		transform.GetRot().y = counter;

		shader.Bind();

		shader.Update(transform, camera);

		mesh.Draw();
		mesh2.Draw();
		mesh3.Draw();

		display.Update();

		counter += 0.016f;
	}

	return 0;
}